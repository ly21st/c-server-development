#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


double f (lua_State *L, double x, double y) {
    int isnum;
    double z; 
    
    char fname[] = "exam1.lua";
    if (luaL_loadfile(L, fname)) {
        printf("loadfile error\n");
        error(L, "error running function 'f': %s", lua_tostring(L, -1));
    }
    
    lua_getglobal(L, "f");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    

    if (lua_pcall(L, 2, 1, 0) != LUA_OK)
        error(L, "error running function 'f': %s",
            lua_tostring(L, -1));

    z = lua_tonumberx(L, -1, &isnum);
    if (!isnum)
        error(L, "function 'f' should return a number");
    lua_pop(L, 1);
    return z;
}



int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    
    double x = 3; 
    double y = 4; 
    double z = 0;

    z = f(L, x, y);

    printf("z=%f", z);
    lua_close(L);
    return 0;
}