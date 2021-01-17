#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <limits.h>

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
    return 1;  // 新的用户数组已经位于栈中    
}

static int setarray(lua_State *L) {
    BitArray *a = (BitArray *)lua_touserdata(L, 1);
    int index = (int)luaL_checkinteger(L, 2) - 1;

    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    luaL_checkany(L, 3);

    if (lua_toboolean(L, 3))
        a->values[I_WORD(index)] |= I_BIT(index); // 置位
    else 
        a->values[I_WORD(index)] &= ~I_BIT(index);
    return 0;
}

static int getarray(lua_State *L) {
    BitArray *a = (BitArray *)lua_touserdata(L, 1);
    int index = (int)luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, a!=NULL, 1, "'array' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    lua_pushboolean(L, a->values[I_WORD(index)] & I_BIT(index));
    return 1;
}

static int getsize(lua_State *L) {
    BitArray *a = (BitArray *)lua_touserdata(L, 1);
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    lua_pushinteger(L, a->size);
    return 1;
}


static const struct luaL_Reg arraylib[] = {
    {"new", newArray},
    {"set", setarray},
    {"get", getarray},
    {"size", getsize},
    {NULL, NULL}
};

int luaopen_array(lua_State *L) {
    luaL_newlib(L, arraylib);
    return 1;
}