#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <limits.h>


#define checkarray(L)  (BitArray*)luaL_checkudata(L, 1, "LuaBook.array")

#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))
#define I_WORD(i) ((unsigned int)(i) / BITS_PER_WORD)
#define I_BIT(i)  (1 << ((unsigned int)(i) % BITS_PER_WORD))

typedef struct BitArray {
    int size; 
    unsigned int values[1];
} BitArray;


static int newArray(lua_State *L) {
    int i; 
    size_t nbytes;
    BitArray *a;

    // 比特位的个数
    int n = (int)luaL_checkinteger(L, 1);
    luaL_argcheck(L, n >= 1, 1, "invalid size");
    nbytes = sizeof(BitArray) + I_WORD(n - 1) * sizeof(unsigned int);
    a = (BitArray *)lua_newuserdata(L, nbytes);

    a->size = n;
    for (i = 0; i <= I_WORD(n - 1); i++)
        a->values[i] = 0;  // 初始化数组

    luaL_getmetatable(L, "LuaBook.array");
    lua_setmetatable(L, -2);

    return 1;  // 新的用户数组已经位于栈中    
}

static unsigned int *getparams(lua_State *L, unsigned int *mask) {
    BitArray *a = checkarray(L);
    int index = (int)luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    *mask = I_BIT(index); // 访问指定比特位的掩码
    return &a->values[I_WORD(index)];  // 字所在的地址
}


static int setarray(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);
    luaL_checkany(L, 3);
    if (lua_toboolean(L, 3))
        *entry |= mask;
    else 
        *entry &= ~mask;
    return 0;
}

static int getarray(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);
    lua_pushboolean(L, *entry & mask);
    return 1;
}

static int getsize(lua_State *L) {
    BitArray *a = checkarray(L);
    lua_pushinteger(L, a->size);
    return 1;
}

int array2string(lua_State *L) {
    BitArray *a = checkarray(L);
    lua_pushfstring(L, "array(%d)", a->size);
    return 1;
}


static const struct luaL_Reg arraylib_f[] = {
    {"new", newArray},
    {NULL, NULL}
};

static const struct luaL_Reg arraylib_m[] = {
    {"set", setarray},
    {"get", getarray},
    {"size", getsize},
    {"__tostring", array2string},
    {NULL, NULL}
};


int luaopen_array(lua_State *L) {
    luaL_newmetatable(L, "LuaBook.array");
    lua_pushvalue(L, -1); // 复制元表
    lua_setfield(L, -2, "__index"); // mt.__index = mt
    luaL_setfuncs(L, arraylib_m, 0); // 注册元方法
    luaL_newlib(L, arraylib_f); // 创建库
    return 1;
}