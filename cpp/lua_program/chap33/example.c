#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <pthread.h>

typedef struct Proc {
    lua_State *L;
    pthread_t thread;
    pthread_cond_t cond; 
    const char *channel;
    struct Proc *previous, *next;
} Proc;

static Proc *waitsend = NULL;
static Proc *waitreceive = NULL;

static pthread_mutex_t kernel_access = PTHREAD_MUTEX_INITIALIZER;


static void openlibs(lua_State *L);



// 从指定的状态中获取相关联的Proc结构体
static Proc *getself(lua_State *L) {
    Proc *p;
    lua_getfield(L, LUA_REGISTRYINDEX, "_SELF");
    p = (Proc*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return p;
}



// 将值从发送进程移动到接收进程
// 将发送进程的栈中所有的值（除了第一个，它是通道）移动到接收进程的栈中,
// 请注意，在压入任意数量的元素时，需要检查栈空间
static void movevalues(lua_State *send, lua_State *rec) {
    int n = lua_gettop(send);
    int i;
    luaL_checkstack(rec, n, "too many results");
    for (i = 2; i <= n; i++) // 将值传递给接收进程
        lua_pushstring(rec, lua_tostrig(send, i));
}

// 用于寻找等待通道的进程
static Proc* searchmatch(const char *channel, Proc **list) {
    Proc *node;
    // 遍历列表
    for (node = *list; node != NULL; node = node->next) {
        // 匹配？
        if (strcmp(channel, node->channel) == 0) {
            // 将结点从列表移除
            // 结点是否为第一个元素?
            if (*list == node) 
                *list = (node->next == node)? NULL: node->next;
            node->previous->next = node->next;
            node->next->previous = node->previous;
            return node;
        }
    }
    return NULL; // 没有找到匹配
}

// 用于在等待列表中新增一个进程
static void waitonlist(lua_State *L, const char* channel, Proc **list) {
    Proc *p = getself(L);
    // 讲其自身放到链表的末尾
    if (*list == NULL) {
        *list = p;
        p->previous = p->next = p;
    } else {
        p->previous = (*list)->previous;
        p->next = *list;
        p->previous->next = p->next->previous = p;
    }

    p->channel = channel; // 等待的通道

    // 等待其他条件变量
    do {
        pthread_cond_wait(&p->cond, &kernel_access);
    } while (p->channel);
}


// 用于发送和接收消息的函数
static int ll_send(lua_State *L) {
    Proc *p;
    const char *channel = luaL_checkstring(L, 1);
    pthread_mutex_lock(&kernel_access);

    p = searchmatch(channel, &waitreceive);

    // 找到匹配的接收线程
    if (p) {
        movevalues(L, p->L); // 将值传递给接收线程
        p->channel = NULL; // 标记接收线程无需再等待
        pthread_cond_signal(&p->cond);  // 唤醒接收线程
    } else 
        waitonlist(L, channel, &waitsend);

    pthread_mutex_unlock(&kernel_access);
    return 0;
}


static int ll_receive(lua_State *L) {
    Proc *p;
    const char *channel = luaL_checkstring(L, 1);
    lua_settop(L, 1);

    pthread_mutex_lock(&kernel_access);
    p = searchmatch(channel, &waitsend);

    // 找到匹配的发送线程
    if (p) {
        movevalues(p->L, L); // 从发送线程获取值
        p->channel = NULL; // 标记发送线程无需再等待
        pthread_cond_signal(&p->cond); // 唤醒发送线程
    } else 
        waitonlist(L, channel, &waitreceive);

    pthread_mutex_unlock(&kernel_access);
    // 返回除通道外的栈中的值
    return lua_gettop(L) - 1;
}

static void *ll_thread(void *arg);

// 用于创建进程的函数
static int ll_start(lua_State *L) {
    pthread_t thread;
    const char *chunk = luaL_checkstring(L, 1);
    lua_State *L1 = luaL_newstate();

    if (L1 == NULL) 
        luaL_error(L, "unable to create new state");

    if (luaL_loadstring(L1, chunk) != 0)
        luaL_error(L, "error in thread body: %s", lua_tostring(L1, -1));

    if (pthread_create(&thread, NULL, ll_thread, L1) != 0)
        luaL_error(L, "unable to create new thread");

    pthread_detach(thread);
    return 0;
}


// 新线程的线程体
int luaopen_lproc(lua_State *L);

static void *ll_thread(void *arg) {
    lua_State *L = (lua_State *)arg;
    Proc *self; // 线程自身的控制块

    openlibs(L);
    luaL_requiref(L, "lproc", luaopen_lproc, 1);
    lua_pop(L, 1);   //  移除之前调用的结果

    self = (Proc*)lua_newuserdata(L, sizeof(Proc));
    lua_setfield(L, LUA_REGISTRYINDEX, "_SELF");
    self->L = L;
    self->thread = pthread_self();
    self->channel = NULL;
    pthread_cond_init(&self->cond, NULL);

    if (lua_pcall(L, 0, 0, 0) != 0) // 调用主代码段
        fprintf(stderr, "thread error:%s", lua_tostrig(L, -1));

    pthread_cond_destroy(&getself(L)->cond);
    lua_close(L);
    return NULL;
}

// 模块lproc的其他函数
static int ll_exit(lua_State *L) {
    pthread_exit(NULL);
    return 0;
}


static const struct luaL_Reg ll_funcs[] = {
    {"start", ll_start},
    {"send", ll_send},
    {"receive", ll_receive},
    {"exit", ll_exit},
    {NULL, NULL}
};


int luaopen_lproc(lua_State *L) {
    luaL_newlib(L, ll_funcs);
    return 1;
}


// 注册按需打开的库
static void registerlib(lua_State *L, const char *name, lua_CFunction f) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");  // 获取package.preload
    lua_pushcfunction(L, f);
    lua_setfield(L, -2, name); // package.preload[name] = f
    lua_pop(L, 2);   // 弹出package和preload

}

static void openlibs(lua_State *L) {
    luaL_requiref(L, "_G", luaopen_base, 1);
    luaL_requiref(L, "package", luaopen_package, 1);
    lua_pop(L, 2); // 移除之前调用的结果
    registerlib(L, "coroutine", luaopen_coroutine);
    registerlib(L, "table", luaopen_table);
    registerlib(L, "io", luaopen_io);
    registerlib(L, "os", luaopen_os);
    registerlib(L, "string", luaopen_string);
    registerlib(L, "math", luaopen_math);
    registerlib(L, "utf8", luaopen_utf8);
    registerlib(L, "debug", luaopen_debug);
}