#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdarg.h>

lua_State *L;
void load_lua(lua_State **L, char *filename)
{
    *L = luaL_newstate();
    luaL_openlibs(*L);
    if (luaL_loadfile(*L, filename) || lua_pcall(*L, 0, 0, 0))
    {
        luaL_error(*L, "load file error! %s", lua_tostring(*L, -1));
    }
}
int main(int argc, char *argv[])
{
    load_lua(&L, "raw.lua"); //此处若直接传入L会出错
    lua_getglobal(L, "gettable");
    if (lua_pcall(L, 0, 1, 0) != 0)
    {
        luaL_error(L, "pcall wrong %s", lua_tostring(L, -1));
    }
    luaL_checktype(L, 1, LUA_TTABLE);
    int n = lua_objlen(L, 1);
    printf("n = %d\n", n);
    lua_pushstring(L, "ee");
    lua_rawseti(L, 1, 5); //t[n]=v,n为第三个参数，v是栈顶元素
    n = lua_objlen(L, 1);
    printf("n = %d\n", n);
    int i;
    for (i = 1; i <= n; i++)
    {
        lua_rawgeti(L, 1, i);
        printf("%s\n", lua_tostring(L, -1));
    }
    return 0;
}
