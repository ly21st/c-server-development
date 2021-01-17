#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


static int counter(lua_State *L);

int newCounter(lua_State *L) {
    lua_pusinteger(L, 0);
    lua_pushclousure(L, &counter, 1);
    return 1;
}

static int counter(lua_State *L) {
    int val = lua_tointeger(L, lua_upvalueindex(1));
    lua_pushinteger(L, ++val);
    lua_copy(L, -1, lua_upvalueindex(1));
    return 1;
}


// 元组的实现
int t_tuple(lua_State *L) {
    lua_Integer op = luaL_optinteger(L, 1, 0);
    if (op == 0) {
        int i; 
        for (i = 1; !lua_isnone(L, lua_upvalueindex(i)); i++)
            lua_pushvalue(L, lua_upvalueindex(i));
        return i - 1;
    } else {
        luaL_argcheck(L, 0 < op && op <= 256, 1, "index out of range");
        if (lua_isnone(L, lua_upvalueindex(op)))
            return 0;
        lua_pushvalue(L, lua_upvalueindex(op));
        return 1;
    }
}

int t_new(lua_State *L) {
    int top = lua_gettop(1);
    lua_argcheck(L, top < 256, top, "too many fields");
    lua_pushcclosure(L, t_tuple, top);
    return 1;
}


static const struct luaL_Reg tuplelib[] = {
    {"new", t_new},
    {NULL, NULL}
};

int luaopen_tuple(lua_State *L) {
    luaL_newlib(L, tuplelib);
    return 1;
}