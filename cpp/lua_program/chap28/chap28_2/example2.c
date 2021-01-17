#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void get_color(lua_State *L) {
    lua_getglobal(L, "background");
    if (!lua_istable(L, -1))
        error(L, "'background' is not a table");

    int red = getcolorfield(L, "red");
    int green = getcolorfield(L, "green");
    int blue = getcolorfield(L, "blue"); 
}

#define MAX_COLOR 255

int getcolorfield(lua_State *L, const char *key) {
    int result, isnum;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    result = (int)lua_tonumberx(L, -1, &isnum) * MAX_COLOR;
    if (!isnum) 
        error(L, "invalid component '%s' in color", key);
    lua_pop(L, 1);
    return result;
}

struct ColorTable {
    char *name;
    unsigned char red, green, blue;
} colortable[] = {
    {"WHITE", MAX_COLOR, MAX_COLOR, MAX_COLOR},
    {"RED", MAX_COLOR, 0, 0},
    {"GREEN", 0, MAX_COLOR, 0},
    {"BLUE", 0, 0, MAX_COLOR},
    {NULL, 0, 0, 0}
};

void setcolorfield(lua_State *L, const char *index, int value) {
    lua_pushstring(L, index);
    lua_pushnumber(L, (double)value / MAX_COLOR);
    lua_settable(L, -3);
}

void setcolor(lua_State *L, struct ColorTable *ct) {    
    lua_newstable(L);
    setcolorfield(L, "red", ct->red);
    setcolorfield(L, "green", ct->green);
    setcolorfield(L, "blue", ct->blue);
    lua_settable(L, ct->name);
}

void my_setcolor(lua_State *L) {
    int i = 0;
    while (colortable[i].name != NULL)
        setcolor(L, &colortable[i++]);
}