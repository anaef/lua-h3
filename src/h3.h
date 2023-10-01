/*
 * Lua H3
 *
 * Copyright (c) 2022-2023 Andre Naef
 */


#ifndef _H3_INCLUDED
#define _H3_INCLUDED


#include <lua.h>


#define H3_GEOPOLYGON        "h3.geopolygon"        /* GeoPolygon metatable */
#define H3_LINKEDGEOPOLYGON  "h3.linkedgeopolygon"  /* LinkedGeoPolygon metatable */
#define H3_STACK_MAX         128                    /* maximum indexes on stack */


int luaopen_h3(lua_State *L);


#endif  /* _H3_INCLUDED */
