#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdarg.h>

void call_va(lua_State *L, const char *func, const char *sig, ...) {
    va_list vl;
    int narg, nres;

    va_start(vl, sig);
    lua_getglobal(L, func);
    for (narg = 0; *sig; narg++) {
        luaL_checkstack(L, 1, "too many arguments");
        switch (*sig++) {
            case 'd':
                lua_pushnumber(L, va_arg(vl, double));
                break;
            case 'i':
                lua_pushinteger(L, va_arg(vl, int));
                break;
            case 's':
                lua_pushstring(L, va_arg(vl, char*));
                break;
            case '>':
                goto endargs;
            default:
                error(L, "invalid option(%c)", *(sig - 1));
        }
    }
    endargs:


    nres = strlen(sig);
    int err = 0;
    if ((err = lua_pcall(L, narg, nres, 0)) != 0) {
        printf("narg=%d, nres=%d, err = %d\n", narg, nres, err);
        error(L, "error calling '%s:%s", func, lua_tostring(L, -1));
    }


    nres = -nres;
    while(*sig) {
        switch(*sig++) {
            case 'd': {
                int isnum;
                double n = lua_tonumberx(L, nres, &isnum);
                if (!isnum) 
                    error(L, "wrong result type");
                *va_arg(vl, double *) = n;
                break;
            }
            case 'i': {
                int isnum;
                int n = lua_tointegerx(L, nres, &isnum);
                if (!isnum)
                    error(L, "wrong result type");
                *va_arg(vl, int*) = n;
                break;
            }
            case 's': {
                const char *s = lua_tostring(L, nres);
                if (s == NULL)
                    error(L, "wrong result type");
                *va_arg(vl, const char **) = s;
                break;
            }
            default:
                error(L, "invalid option (%c)", *(sig - 1));
        }
        nres++;
    }

    va_end(vl);
}



int main() {
    char buff[256];
    int error;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    char fname[] = "exam1.lua";
    error = luaL_loadfile(L, fname);
    if (error) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    double x = 3; 
    double y = 4; 
    double z = 0;
    call_va(L, "f", "dd>d", x, y, &z);

    printf("z=%f", z);

    lua_close(L);
    return 0;
}
