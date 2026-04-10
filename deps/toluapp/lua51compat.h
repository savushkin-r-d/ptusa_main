/* lua51compat.h
** Lua 5.1 compatibility shim for building toluapp with Lua 5.2+.
** Force-included before each toluapp translation unit via -include.
*/

#ifndef LUA_51_COMPAT_H
#define LUA_51_COMPAT_H

/* Include Lua headers first so LUA_VERSION_NUM is available. */
#include "lua.h"
#include "lauxlib.h"

#if LUA_VERSION_NUM >= 502

/* Lua 5.4+ invokes __gc on tables (not just userdata).  toluapp's
** class_gc_event does *((void**)lua_touserdata(L,1)) which segfaults
** when arg 1 is a table (lua_touserdata returns NULL).
** Provide a safe wrapper: when the value is not userdata, return a
** pointer to a static NULL so the dereference yields NULL instead of
** crashing.  The rest of class_gc_event handles a NULL 'u' safely. */
static void* _lua51compat_null_ud = (void*)0;
static inline void* _lua_touserdata_safe(lua_State* L, int idx)
    {
    void *p = (lua_touserdata)(L, idx);
    return p ? p : &_lua51compat_null_ud;
    }
#define lua_touserdata(L, idx) _lua_touserdata_safe(L, idx)

/* luaL_getn() was removed in Lua 5.2; use lua_rawlen() instead. */
#ifndef luaL_getn
#define luaL_getn(L, i) ((int)(lua_rawlen)(L, i))
#endif

/* lua_getfenv()/lua_setfenv() were removed in Lua 5.2.
** Full userdata now stores its associated value via user values. */
static inline void lua_getfenv(lua_State* L, int idx)
    {
    lua_getuservalue(L, idx);
    }

static inline int lua_setfenv(lua_State* L, int idx)
    {
    lua_setuservalue(L, idx);
    return 1;
    }

/* LUA_GLOBALSINDEX was removed in Lua 5.2.
** Provide a sentinel constant and override lua_pushvalue so that
** lua_pushvalue(L, LUA_GLOBALSINDEX) pushes the global table. */
#ifndef LUA_GLOBALSINDEX
#define LUA_GLOBALSINDEX (-10002)

/* Rename the real function so the macro can call it without recursion. */
#define _lua_pushvalue_real(L, idx) (lua_pushvalue)(L, idx)

#define lua_pushvalue(L, idx) \
    do { \
        if ((idx) == LUA_GLOBALSINDEX) \
            lua_pushglobaltable(L); \
        else \
            _lua_pushvalue_real(L, idx); \
    } while (0)
#endif /* LUA_GLOBALSINDEX */

#endif /* LUA_VERSION_NUM >= 502 */

#endif /* LUA_51_COMPAT_H */
