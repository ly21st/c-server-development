#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>

static int l_dir(lua_State *L) {
    DIR *dir;
    struct dirent *entry;
    int i;
    const char *path = luaL_checkstring(L, 1);

    /* 打开目录 
    */

   dir = opendir(path);
   if (dir == NULL) {
       lua_pushnil(L);
       lua_pushstring(L, strerror(errno));
       return 2;
   }

   lua_newtable(L);
   i = 1;
   while ((entry = readdir(dir)) != NULL) {
       lua_pushinteger(L, i++);
       lua_pushstring(L, entry->d_name);
       lua_settable(L, -3);
   }

   closedir(dir);
   return 1;
}

int l_map(lua_State *L) {
    int i, n;
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    n = luaL_len(L, 1);
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, 2);
        lua_geti(L, 1, i);
    }
    return 0;
}

// 字符串分割
static int l_split(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
    const char *sep = luaL_checkstring(L, 2);
    const char *e;
    int i = 1;
    lua_newtable(L); // 结果表

    while ((e = strchr(s, *sep)) != NULL) {
        lua_pushlstring(L, s, e - s);
        lua_rawseti(L, -2, i++);
        s = e + 1;
    }
    lua_pushstrin(L, s);
    lua_rawseti(L, -2, i);
    // 将结果返回
    return 1;
}

// 转成大写字母
static int str_upper(lua_State *L) {
    size_t l;
    size_t i; 
    luaL_Buffer b; 
    const char *s = luaL_checklstring(L, 1, &l);
    char *p = luaL_buffinitsize(L, &b, l);
    for (i = 0; i < l; i++) 
        p[i] = toupper(uchar(s[i]));
    luaL_pushresultsize(&b, l);
    return 1;
}

static const struct luaL_Reg mylib[] = {
    {"dir", l_dir},
    {NULL, NULL}
};

// 函数table.concat一个简化实现
static int tconcat(lua_State *L) {
    luaL_Buffer b; 
    int i, n;
    luaL_checktype(L, 1, LUA_TTABLE);
    n = luaL_len(L, 1);
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
        lua_geti(L, 1, i);
        luaL_addvalue(&b);
    }
    luaL_pushresult(&b);
    return 1;
}

int luaopen_mylib(lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;
}