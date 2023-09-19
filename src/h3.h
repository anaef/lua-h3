/*
 * Lua H3
 *
 * Copyright (c) 2022-2023 Andre Naef
 */


#ifndef _LUAH3_INCLUDED
#define _LUAH3_INCLUDED

#include <lua.h>


#define LUAH3_GEOPOLYGON_METATABLE        "h3.geopolygon"
#define LUAH3_LINKEDGEOPOLYGON_METATABLE  "h3.linkedgeopolygon"
#define LUAH3_STACK_MAX                    128                   /* maximum indexes on stack */


int luaopen_h3(lua_State *L);


#endif  /* _LUAH3_INCLUDED */
