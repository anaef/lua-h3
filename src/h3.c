/*
 * Lua H3
 *
 * Copyright (c) 2022-2023 Andre Naef
 */

#include "h3.h"
#include <string.h>
#include <stdio.h>
#include <lauxlib.h>
#include <h3/h3api.h>


static void check(lua_State *L, H3Error error);
static int geopolygon_gc(lua_State *L);
static int linkedgeopolygon_gc(lua_State *L);

static int h3_version(lua_State *L);

static int h3_latlngtocell(lua_State *L);
static int h3_celltolatlng(lua_State *L);
static int h3_celltoboundary(lua_State *L);

static int h3_resolution(lua_State *L);
static int h3_basecellnumber(lua_State *L);
static int h3_stringtoh3(lua_State *L);
static int h3_h3tostring(lua_State *L);
static int h3_iscell(lua_State *L);
static int h3_isresclassiii(lua_State *L);
static int h3_ispentagon(lua_State *L);
static int h3_icosahedronfaces(lua_State *L);

static int h3_griddisk(lua_State *L);
static int h3_gridring(lua_State *L);
static int h3_gridpathcells(lua_State *L);
static int h3_griddistance(lua_State *L);
static int h3_celltolocalij(lua_State *L);
static int h3_localijtocell(lua_State *L);

static int h3_celltoparent(lua_State *L);
static int h3_celltochildren(lua_State *L);
static int h3_celltocenterchild(lua_State *L);
static int h3_compactcells(lua_State *L);
static int h3_uncompactcells(lua_State *L);

static void geoloop (lua_State *L, int index, int loopindex, GeoLoop *loop);
static int h3_polygontocells(lua_State *L);
static int h3_cellstopolygons(lua_State *L);

static int h3_areneighborcells(lua_State *L);
static int h3_cellstoedge(lua_State *L);
static int h3_isedge(lua_State *L);
static int h3_edgetocells(lua_State *L);
static int h3_origintoedges(lua_State *L);
static int h3_edgetoboundary(lua_State *L);

static int h3_celltovertexes(lua_State *L);
static int h3_vertextolatlng(lua_State *L);
static int h3_isvertex(lua_State *L);

static int h3_hexagonavg(lua_State *L);
static int h3_cellarea(lua_State *L);
static int h3_edgelength(lua_State *L);
static int h3_numcells(lua_State *L);
static int h3_res0cells(lua_State *L);
static int h3_pentagons(lua_State *L);
static int h3_greatcircledistance(lua_State *L);


static const char *const H3_ERROR_MESSAGES[] = {
	NULL,
	"operation failed",
	"bad argument",
	"bad latitude or longitude",
	"bad resolution",
	"bad cell",
	"bad directed edge",
	"bad unidirected edge",
	"bad vertex",
	"pentagon encountered",
	"duplicate input",
	"not neighbors",
	"resolution mismatch",
	"out of memory",
	"memory too small",
	"bad option",
};

static const char *const EDGE_OPTIONS[] = {
	"both",
	"origin",
	"destination",
	NULL
};

static const char *const HEXAGON_QUANTITIES[] = {
	"area",
	"edge",
	NULL
};

static const char *const HEXAGON_UNITS[] = {
	"m",
	"km",
	NULL
};

static const char *const GEO_UNITS[] = {
	"m",
	"km",
	"rad",
	NULL
};


/*
 * utilities
 */

static void check (lua_State *L, H3Error error) {
	if (error != E_SUCCESS) {
		if (error >= E_FAILED && error <= E_OPTION_INVALID) {
			luaL_error(L, H3_ERROR_MESSAGES[error]);
		} else {
			luaL_error(L, "unknown error (%d)", error);
		}
	}
}

static int geopolygon_gc (lua_State *L) {
	int          i;
	GeoPolygon  *polygon;

	polygon = luaL_checkudata(L, 1, LUAH3_GEOPOLYGON);
	free(polygon->geoloop.verts);
	for (i = 0; i < polygon->numHoles; i++) {
		free(polygon->holes[i].verts);
	}
	free(polygon->holes);
	return 0;
}

static int linkedgeopolygon_gc (lua_State *L) {
	LinkedGeoPolygon  *polygon;

	polygon = luaL_checkudata(L, 1, LUAH3_LINKEDGEOPOLYGON);
	destroyLinkedMultiPolygon(polygon);
	return 0;
}


/*
 * version
 */

static int h3_version (lua_State *L) {
	lua_pushfstring(L, "%d.%d.%d", H3_VERSION_MAJOR, H3_VERSION_MINOR, H3_VERSION_PATCH);
	lua_pushinteger(L, H3_VERSION_MAJOR);
	lua_pushinteger(L, H3_VERSION_MINOR);
	lua_pushinteger(L, H3_VERSION_PATCH);
	return 4;
}


/*
 * indexing
 */

static int h3_latlngtocell (lua_State *L) {
	int      res;
	LatLng   g;
	H3Index  out;

	g.lat = degsToRads(luaL_checknumber(L, 1));
	g.lng = degsToRads(luaL_checknumber(L, 2));
	res = luaL_checkinteger(L, 3);
	check(L, latLngToCell(&g, res, &out));
	lua_pushinteger(L, out);
	return 1;
}

static int h3_celltolatlng (lua_State *L) {
	LatLng   g;
	H3Index  cell;

	cell = luaL_checkinteger(L, 1);
	check(L, cellToLatLng(cell, &g));
	lua_pushnumber(L, radsToDegs(g.lat));
	lua_pushnumber(L, radsToDegs(g.lng));
	return 2;
}

static int h3_celltoboundary (lua_State *L) {
	int           i;
	H3Index       cell;
	CellBoundary  bndry;

	cell = luaL_checkinteger(L, 1);
	check(L, cellToBoundary(cell, &bndry));
	lua_createtable(L, bndry.numVerts, 0);
	for (i = 0; i < bndry.numVerts; i++) {
		lua_createtable(L, 2, 0);
		lua_pushnumber(L, radsToDegs(bndry.verts[i].lat));
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, radsToDegs(bndry.verts[i].lng));
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}


/*
 * inspection
 */

static int h3_resolution (lua_State *L) {
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	lua_pushinteger(L, getResolution(h));
	return 1;
}

static int h3_basecellnumber (lua_State *L) {
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	lua_pushinteger(L, getBaseCellNumber(h));
	return 1;
}

static int h3_stringtoh3 (lua_State *L) {
	H3Index      out;
	const char  *str;

	str = luaL_checkstring(L, 1);
	check(L, stringToH3(str, &out));
	lua_pushinteger(L, out);
	return 1;
}

static int h3_h3tostring (lua_State *L) {
	char     str[17];
	H3Index  h;
	
	h = luaL_checkinteger(L, 1);
	check(L, h3ToString(h, str, sizeof(str)));
	lua_pushstring(L, str);
	return 1;
}

static int h3_iscell (lua_State *L) {
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	lua_pushboolean(L, isValidCell(h));
	return 1;
}

static int h3_isresclassiii (lua_State *L) {
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	lua_pushboolean(L, isResClassIII(h));
	return 1;
}

static int h3_ispentagon (lua_State *L) {
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	lua_pushboolean(L, isPentagon(h));
	return 1;
}

static int h3_icosahedronfaces (lua_State *L) {
	int      num, i, j;
	int     *out;
	H3Index  h;
	
	h = luaL_checkinteger(L, 1);
	check(L, maxFaceCount(h, &num));
	out = alloca(num * sizeof(int));
	check(L, getIcosahedronFaces(h, out));
	lua_createtable(L, num, 0);
	i = 0;
	for (j = 0; j < num; j++) {
		if (out[j] >= 0) {
			lua_pushinteger(L, out[j]);
			lua_rawseti(L, -2, ++i);
		}
	}
	return 1;
}


/*
 * traversal
 */

static int h3_griddisk (lua_State *L) {
	int          k;
	int*         distances;
	int64_t      num, i;
	H3Index      origin, *out;
	const char  *mode;

	origin = luaL_checkinteger(L, 1);
	k = luaL_checkinteger(L, 2);
	mode = luaL_optstring(L, 3, "");
	check(L, maxGridDiskSize(k, &num));
	if (strchr(mode, 'd')) {
		if (num <= LUAH3_STACK_MAX / 2) {
			out = alloca(num * sizeof(H3Index));
			distances = alloca(num * sizeof(int));
		} else {
			out = lua_newuserdata(L, num * (sizeof(H3Index) + sizeof(int)));
			distances = (int *)(out + num);
		}
		if (strchr(mode, 'u')) {
			check(L, gridDiskDistancesUnsafe(origin, k, out, distances));
		} else {
			check(L, gridDiskDistances(origin, k, out, distances));
		}
		lua_createtable(L, num, 0);
		lua_createtable(L, num, 0);
		for (i = 0; i < num; i++) {
			lua_pushinteger(L, out[i]);
			lua_rawseti(L, -3, i + 1);
			lua_pushinteger(L, distances[i]);
			lua_rawseti(L, -2, i + 1);
		}
		return 2;
	} else {
		if (num <= LUAH3_STACK_MAX) {
			out = alloca(num * sizeof(H3Index));
		} else {
			out = lua_newuserdata(L, num * sizeof(H3Index));
		}
		if (strchr(mode, 'u')) {
			check(L, gridDiskUnsafe(origin, k, out));
		} else {
			check(L, gridDisk(origin, k, out));
		}
		lua_createtable(L, num, 0);
		for (i = 0; i < num; i++) {
			lua_pushinteger(L, out[i]);
			lua_rawseti(L, -2, i + 1);
		}
		return 1;
	}
}

static int h3_gridring (lua_State *L) {
	int      k;
	int64_t  numOuter, numInner, num, i;
	H3Index  origin, *out;

	origin = luaL_checkinteger(L, 1);
	k = luaL_checkinteger(L, 2);
	check(L, maxGridDiskSize(k, &numOuter));
	if (k > 0) {
		check(L, maxGridDiskSize(k - 1, &numInner));
	} else {
		numInner = 0;
	}
	num = numOuter - numInner;
	if (num <= LUAH3_STACK_MAX) {
		out = alloca(num * sizeof(H3Index));
	} else {
		out = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, gridRingUnsafe(origin, k, out));
	lua_createtable(L, num, 0);
	for (i = 0; i < num; i++) {
		lua_pushinteger(L, out[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_gridpathcells (lua_State *L) {
	int64_t  num, i;
	H3Index  start, end, *out;

	start = luaL_checkinteger(L, 1);
	end = luaL_checkinteger(L, 2);
	check(L, gridPathCellsSize(start, end, &num));
	if (num <= LUAH3_STACK_MAX) {
		out = alloca(num * sizeof(H3Index));
	} else {
		out = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, gridPathCells(start, end, out));
	lua_createtable(L, num, 0);
	for (i = 0; i < num; i++) {
		lua_pushinteger(L, out[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_griddistance (lua_State *L) {
	int64_t  distance;
	H3Index  origin, h3;

	origin = luaL_checkinteger(L, 1);
	h3 = luaL_checkinteger(L, 2);
	check(L, gridDistance(origin, h3, &distance));
	lua_pushinteger(L, distance);
	return 1;
}

static int h3_celltolocalij (lua_State *L) {
	CoordIJ  out;
	H3Index  origin, h3;

	origin = luaL_checkinteger(L, 1);
	h3 = luaL_checkinteger(L, 2);
	check(L, cellToLocalIj(origin, h3, 0, &out));
	lua_pushinteger(L, out.i);
	lua_pushinteger(L, out.j);
	return 2;
}

static int h3_localijtocell (lua_State *L) {
	CoordIJ  ij;
	H3Index  origin, out;

	origin = luaL_checkinteger(L, 1);
	ij.i = luaL_checkinteger(L, 2);
	ij.j = luaL_checkinteger(L, 3);
	check(L, localIjToCell(origin, &ij, 0, &out));
	lua_pushinteger(L, out);
	return 1;
}


/*
 * hierarchy
 */

static int h3_celltoparent (lua_State *L) {
	int      parentRes;
	H3Index  cell, parent;

	cell = luaL_checkinteger(L, 1);
	parentRes = luaL_checkinteger(L, 2);
	check(L, cellToParent(cell, parentRes, &parent));
	lua_pushinteger(L, parent);
	return 1;
}

static int h3_celltochildren (lua_State *L) {
	int      childRes;
	int64_t  num, i;
	H3Index  cell, *children;

	cell = luaL_checkinteger(L, 1);
	childRes = luaL_checkinteger(L, 2);
	check(L, cellToChildrenSize(cell, childRes, &num));
	if (num <= LUAH3_STACK_MAX) {
		children = alloca(num * sizeof(H3Index));
	} else {
		children = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, cellToChildren(cell, childRes, children));
	lua_createtable(L, num, 0);
	for (i = 0; i < num; i++) {
		lua_pushinteger(L, children[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_celltocenterchild (lua_State *L) {
	int      childRes;
	H3Index  cell, child;

	cell = luaL_checkinteger(L, 1);
	childRes = luaL_checkinteger(L, 2);
	check(L, cellToCenterChild(cell, childRes, &child));
	lua_pushinteger(L, child);
	return 1;
}

static int h3_compactcells (lua_State *L) {
	size_t    len, i;
	H3Index  *cellSet, *compactedSet;

	luaL_checktype(L, 1, LUA_TTABLE);
	len = lua_rawlen(L, 1);
	if (len <= LUAH3_STACK_MAX / 2) {
		cellSet = alloca(len * sizeof(H3Index));
		compactedSet = alloca(len * sizeof(H3Index));
	} else {
		cellSet = lua_newuserdata(L, (len + len) * sizeof(H3Index));
		compactedSet = cellSet + len;
	}
	for (i = 0; i < len; i++) {
		if (lua_rawgeti(L, 1, i + 1) != LUA_TNUMBER) {
			return luaL_error(L, "bad cell");
		}
		cellSet[i] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	memset(compactedSet, 0, len * sizeof(H3Index));
	check(L, compactCells(cellSet, compactedSet, len));
	while (len > 0 && compactedSet[len - 1] == 0) {
		len--;
	}
	lua_createtable(L, len, 0);
	for (i = 0; i < len; i++) {
		lua_pushinteger(L, compactedSet[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_uncompactcells (lua_State *L) {
	int       res;
	size_t    len, i;
	int64_t   num, j;
	H3Index  *compactedSet, *cellSet;

	luaL_checktype(L, 1, LUA_TTABLE);
	res = luaL_checkinteger(L, 2);
	len = lua_rawlen(L, 1);
	if (len <= LUAH3_STACK_MAX / 2) {
		compactedSet = alloca(len * sizeof(H3Index));
	} else {
		compactedSet = lua_newuserdata(L, len * sizeof(H3Index));
	}
	for (i = 0; i < len; i++) {
		if (lua_rawgeti(L, 1, i + 1) != LUA_TNUMBER) {
			return luaL_error(L, "bad cell");
		}
		compactedSet[i] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	check(L, uncompactCellsSize(compactedSet, len, res, &num));
	if (num <= LUAH3_STACK_MAX / 2) {
		cellSet = alloca(num * sizeof(H3Index));
	} else {
		cellSet = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, uncompactCells(compactedSet, len, cellSet, num, res));
	lua_createtable(L, num, 0);
	for (j = 0; j < num; j++) {
		lua_pushinteger(L, cellSet[j]);
		lua_rawseti(L, -2, j + 1);
	}
	return 1;
}


/*
 * region
 */

static void geoloop (lua_State *L, int index, int loopindex, GeoLoop *loop) {
	size_t  len, i;

	if (lua_rawgeti(L, index, loopindex) != LUA_TTABLE) {
		luaL_error(L, "bad polygon");
	}
	len = lua_rawlen(L, -1);
	if (len < 3) {
		luaL_error(L, "bad polygon");
	}
	loop->verts = malloc(len * sizeof(LatLng));
	if (loop->verts == NULL) {
		luaL_error(L, "out of memory");
	}
	loop->numVerts = len;
	for (i = 0; i < len; i++) {
		if (lua_rawgeti(L, -1, i + 1) != LUA_TTABLE) {
			luaL_error(L, "bad polygon");
		}
		if (lua_rawgeti(L, -1, 1) != LUA_TNUMBER || lua_rawgeti(L, -2, 2) != LUA_TNUMBER) {
			luaL_error(L, "bad polygon");
		}
		loop->verts[i].lat = degsToRads(lua_tonumber(L, -2));
		loop->verts[i].lng = degsToRads(lua_tonumber(L, -1));
		lua_pop(L, 3);
	}
	lua_pop(L, 1);
}

static int h3_polygontocells (lua_State *L) {
	int          res;
	size_t       len, i;
	int64_t      num, numSet, j, k;
	GeoPolygon  *polygon;
	H3Index     *out;

	luaL_checktype(L, 1, LUA_TTABLE);
	len = lua_rawlen(L, 1);
	luaL_argcheck(L, len > 0, 1, "bad polygon");
	res = luaL_checkinteger(L, 2);
	polygon = lua_newuserdata(L, sizeof(GeoPolygon));
	memset(polygon, 0, sizeof(GeoPolygon));
	luaL_getmetatable(L, LUAH3_GEOPOLYGON);
	lua_setmetatable(L, -2);
	geoloop(L, 1, 1, &polygon->geoloop);
	if (len > 1) {
		polygon->holes = malloc((len - 1) * sizeof(GeoLoop));
		if (polygon->holes == NULL) {
			luaL_error(L, "out of memory");
		}
		memset(polygon->holes, 0, (len - 1) * sizeof(GeoLoop));
		polygon->numHoles = len - 1;
		for (i = 1; i < len; i++) {
			geoloop(L, 1, i + 1, &polygon->holes[i - 1]);
		}
	}
	check(L, maxPolygonToCellsSize(polygon, res, 0, &num));
	if (num <= LUAH3_STACK_MAX) {
		out = alloca(num * sizeof(H3Index));
	} else {
		out = lua_newuserdata(L, num * sizeof(H3Index));
	}
	memset(out, 0, num * sizeof(H3Index));
	check(L, polygonToCells(polygon, res, 0, out));
	numSet = 0;
	for (j = 0; j < num; j++) {
		if (out[j] != H3_NULL) {
			numSet++;
		}
	}
	lua_createtable(L, numSet, 0);
	k = 0;
	for (j = 0; j < num; j++) {
		if (out[j] != H3_NULL) {
			lua_pushinteger(L, out[j]);
			lua_rawseti(L, -2, ++k);
		}
	}
	return 1;
}

static int h3_cellstopolygons (lua_State *L) {
	size_t             len, i, j, k;
	H3Index           *h3Set;
	LinkedLatLng      *latLng;
	LinkedGeoLoop     *loop;
	LinkedGeoPolygon  *polygon;

	luaL_checktype(L, 1, LUA_TTABLE);
	len = lua_rawlen(L, 1);
	if (len <= LUAH3_STACK_MAX) {
		h3Set = alloca(len * sizeof(H3Index));
	} else {
		h3Set = lua_newuserdata(L, len * sizeof(H3Index));
	}
	for (i = 0; i < len; i++) {
		if (lua_rawgeti(L, 1, i + 1) != LUA_TNUMBER) {
			return luaL_error(L, "bad cell");
		}
		h3Set[i] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	polygon = lua_newuserdata(L, sizeof(LinkedGeoPolygon));
	memset(polygon, 0, sizeof(LinkedGeoPolygon));
	luaL_getmetatable(L, LUAH3_LINKEDGEOPOLYGON);
	lua_setmetatable(L, -2);
	check(L, cellsToLinkedMultiPolygon(h3Set, len, polygon));
	lua_newtable(L);
	i = 0;
	while (polygon != NULL) {
		lua_newtable(L);
		j = 0;
		loop = polygon->first;
		while (loop != NULL) {
			lua_newtable(L);
			k = 0;
			latLng = loop->first;
			while (latLng != NULL) {
				lua_createtable(L, 2, 0);
				lua_pushnumber(L, radsToDegs(latLng->vertex.lat));
				lua_rawseti(L, -2, 1);
				lua_pushnumber(L, radsToDegs(latLng->vertex.lng));
				lua_rawseti(L, -2, 2);
				lua_rawseti(L, -2, ++k);
				latLng = latLng->next;
			}
			lua_rawseti(L, -2, ++j);
			loop = loop->next;
		}
		lua_rawseti(L, -2, ++i);
		polygon = polygon->next;
	}
	return 1;
}


/*
 * directed edges
 */

static int h3_areneighborcells (lua_State *L) {
	int       out;
	H3Index   origin, destination;

	origin = luaL_checkinteger(L, 1);
	destination = luaL_checkinteger(L, 2);
	check(L, areNeighborCells(origin, destination, &out));
	lua_pushboolean(L, out);
	return 1;
}

static int h3_cellstoedge (lua_State *L) {
	H3Index  origin, destination, out;

	origin = luaL_checkinteger(L, 1);
	destination = luaL_checkinteger(L, 2);
	check(L, cellsToDirectedEdge(origin, destination, &out));
	lua_pushinteger(L, out);
	return 1;
}

static int h3_isedge (lua_State *L) {
	H3Index  edge;

	edge = luaL_checkinteger(L, 1);
	lua_pushboolean(L, isValidDirectedEdge(edge));
	return 1;
}

static int h3_edgetocells (lua_State *L) {
	int      mode;
	H3Index  edge, originDestination[2];

	edge = luaL_checkinteger(L, 1);
	mode = luaL_checkoption(L, 2, "both", EDGE_OPTIONS);
	switch (mode) {
	case 0:
		check(L, directedEdgeToCells(edge, originDestination));
		lua_pushinteger(L, originDestination[0]);
		lua_pushinteger(L, originDestination[1]);
		return 2;

	case 1:
		check(L, getDirectedEdgeOrigin(edge, originDestination));
		lua_pushinteger(L, originDestination[0]);
		return 1;

	case 2:
		check(L, getDirectedEdgeDestination(edge, originDestination));
		lua_pushinteger(L, originDestination[0]);
		return 1;

	default:  /* not reached */
		return 0;
	}
}

static int h3_origintoedges (lua_State *L) {
	int      i, j;
	H3Index  origin, edges[6];

	origin = luaL_checkinteger(L, 1);
	check(L, originToDirectedEdges(origin, edges));
	lua_createtable(L, 6, 0);
	i = 0;
	for (j = 0; j < 6; j++) {
		if (edges[j] != H3_NULL) {
			lua_pushinteger(L, edges[j]);
			lua_rawseti(L, -2, ++i);
		}
	}
	return 1;
}

static int h3_edgetoboundary (lua_State *L) {
	int           i;
	H3Index       edge;
	CellBoundary  gb;

	edge = luaL_checkinteger(L, 1);
	check(L, directedEdgeToBoundary(edge, &gb));
	lua_createtable(L, gb.numVerts, 0);
	for (i = 0; i < gb.numVerts; i++) {
		lua_createtable(L, 2, 0);
		lua_pushnumber(L, radsToDegs(gb.verts[i].lat));
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, radsToDegs(gb.verts[i].lng));
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}


/*
 * vertex
 */

static int h3_celltovertexes (lua_State *L) {
	int      i, j;
	int      vertexNum;
	H3Index  origin, out[6];

	origin = luaL_checkinteger(L, 1);
	if (lua_gettop(L) == 1) {
		check(L, cellToVertexes(origin, out));
		lua_createtable(L, 6, 0);
		i = 0;
		for (j = 0; j < 6; j++) {
			if (out[j] != H3_NULL) {
				lua_pushinteger(L, out[j]);
				lua_rawseti(L, -2, ++i);
			}
		}
	} else {
		vertexNum = luaL_checkinteger(L, 2);
		check(L, cellToVertex(origin, vertexNum - 1, out));
		lua_pushinteger(L, out[0]);
	}
	return 1;
}

static int h3_vertextolatlng (lua_State *L) {
	LatLng   point;
	H3Index  vertex;

	vertex = luaL_checkinteger(L, 1);
	check(L, vertexToLatLng(vertex, &point));
	lua_pushnumber(L, radsToDegs(point.lat));
	lua_pushnumber(L, radsToDegs(point.lng));
	return 2;
}

static int h3_isvertex (lua_State *L) {
	H3Index  vertex;

	vertex = luaL_checkinteger(L, 1);
	lua_pushboolean(L, isValidVertex(vertex));
	return 1;
}


/*
 * miscellaneous
 */

static int h3_hexagonavg (lua_State *L)  {
	int     res, quantity, unit;
	double  out;

	res = luaL_checkinteger(L, 1);
	quantity = luaL_checkoption(L, 2, NULL, HEXAGON_QUANTITIES);
	unit = luaL_checkoption(L, 3, "m", HEXAGON_UNITS);
	switch (quantity) {
	case 0:
		switch (unit) {
		case 0:
			check(L, getHexagonAreaAvgM2(res, &out));
			break;

		case 1:
			check(L, getHexagonAreaAvgKm2(res, &out));
			break;

		default:
			return 0; /* not reached */
		}
		break;

	case 1:
		switch (unit) {
		case 0:
			check(L, getHexagonEdgeLengthAvgM(res, &out));
			break;


		case 1:
			check(L, getHexagonEdgeLengthAvgKm(res, &out));
			break;

		default:
			return 0;  /* not reached */
		}
		break;

	default:
		return 0;  /* not reached */
	}
	lua_pushnumber(L, out);
	return 1;
}

static int h3_cellarea (lua_State *L)  {
	int      unit;
	double   out;
	H3Index  h;

	h = luaL_checkinteger(L, 1);
	unit = luaL_checkoption(L, 2, "m", GEO_UNITS);
	switch (unit) {
	case 0:
		check(L, cellAreaM2(h, &out));
		break;

	case 1:
		check(L, cellAreaKm2(h, &out));
		break;

	case 2:
		check(L, cellAreaRads2(h, &out));
		break;

	default:
		return 0;  /* not reached */
	}
	lua_pushnumber(L, out);
	return 1;
}

static int h3_edgelength (lua_State *L)  {
	int     unit;
	double  length;
	H3Index edge;

	edge = luaL_checkinteger(L, 1);
	unit = luaL_checkoption(L, 2, "m", GEO_UNITS);
	switch (unit) {
	case 0:
		check(L, edgeLengthM(edge, &length));
		break;

	case 1:
		check(L, edgeLengthKm(edge, &length));
		break;

	case 2:
		check(L, edgeLengthRads(edge, &length));
		break;

	default:
		return 0;  /* not reached */
	}
	lua_pushnumber(L, length);
	return 1;
}

static int h3_numcells (lua_State *L) {
	int      res;
	int64_t  out;

	res = luaL_checkinteger(L, 1);
	check(L, getNumCells(res, &out));
	lua_pushinteger(L, out);
	return 1;
}

static int h3_res0cells (lua_State *L) {
	int64_t   num, i;
	H3Index  *out;

	num = res0CellCount();
	if (num <= LUAH3_STACK_MAX) {
		out = alloca(num * sizeof(H3Index));
	} else {
		out = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, getRes0Cells(out));
	lua_createtable(L, num, 0);
	for (i = 0; i < num; i++) {
		lua_pushinteger(L, out[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_pentagons (lua_State *L) {
	int       res;
	int64_t   num, i;
	H3Index  *out;

	res = luaL_checkinteger(L, 1);
	num = pentagonCount();
	if (num <= LUAH3_STACK_MAX) {
		out = alloca(num * sizeof(H3Index));
	} else {
		out = lua_newuserdata(L, num * sizeof(H3Index));
	}
	check(L, getPentagons(res, out));
	lua_createtable(L, num, 0);
	for (i = 0; i < num; i++) {
		lua_pushinteger(L, out[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int h3_greatcircledistance (lua_State *L)  {
	int     unit;
	double  distance;
	LatLng  a, b;

	a.lat = degsToRads(luaL_checknumber(L, 1));
	a.lng = degsToRads(luaL_checknumber(L, 2));
	b.lat = degsToRads(luaL_checknumber(L, 3));
	b.lng = degsToRads(luaL_checknumber(L, 4));
	unit = luaL_checkoption(L, 5, "m", GEO_UNITS);
	switch (unit) {
	case 0:
		distance = greatCircleDistanceM(&a, &b);
		break;

	case 1:
		distance = greatCircleDistanceKm(&a, &b);
		break;

	case 2:
		distance = greatCircleDistanceRads(&a, &b);
		break;

	default:
		return 0;  /* not reached */
	}
	lua_pushnumber(L, distance);
	return 1;
}


/*
 * interface
 */

int luaopen_h3 (lua_State *L) {
	static const luaL_Reg FUNCTIONS[] = {
		/* version */
		{ "version", h3_version },

		/* indexing */
		{ "latlngtocell", h3_latlngtocell },
		{ "celltolatlng", h3_celltolatlng },
		{ "celltoboundary", h3_celltoboundary },

		/* inspection */
		{ "resolution", h3_resolution },
		{ "basecellnumber", h3_basecellnumber },
		{ "stringtoh3", h3_stringtoh3 },
		{ "h3tostring", h3_h3tostring },
		{ "iscell", h3_iscell },
		{ "isresclassiii", h3_isresclassiii },
		{ "ispentagon", h3_ispentagon },
		{ "icosahedronfaces", h3_icosahedronfaces },

		/* traversal */
		{ "griddisk", h3_griddisk },
		{ "gridring", h3_gridring },
		{ "gridpathcells", h3_gridpathcells },
		{ "griddistance", h3_griddistance },
		{ "celltolocalij", h3_celltolocalij },
		{ "localijtocell", h3_localijtocell },

		/* hierarchy */
		{ "celltoparent", h3_celltoparent },
		{ "celltochildren", h3_celltochildren },
		{ "celltocenterchild", h3_celltocenterchild },
		{ "compactcells", h3_compactcells },
		{ "uncompactcells", h3_uncompactcells },

		/* region */
		{ "polygontocells", h3_polygontocells },
		{ "cellstopolygons", h3_cellstopolygons },

		/* directed edges */
		{ "areneighborcells", h3_areneighborcells },
		{ "cellstoedge", h3_cellstoedge },
		{ "isedge", h3_isedge },
		{ "edgetocells", h3_edgetocells },
		{ "origintoedges", h3_origintoedges },
		{ "edgetoboundary", h3_edgetoboundary },

		/* vertex */
		{ "celltovertexes", h3_celltovertexes },
		{ "vertextolatlng", h3_vertextolatlng },
		{ "isvertex", h3_isvertex },

		/* miscellaneous */
		{ "hexagonavg", h3_hexagonavg },
		{ "cellarea", h3_cellarea },
		{ "edgelength", h3_edgelength },
		{ "numcells", h3_numcells },
		{ "res0cells", h3_res0cells },
		{ "pentagons", h3_pentagons },
		{ "greatcircledistance", h3_greatcircledistance },
		
		{ NULL, NULL }
	};

	/* register functions */
	luaL_newlib(L, FUNCTIONS);

	/* metatables */
	luaL_newmetatable(L, LUAH3_GEOPOLYGON);
	lua_pushcfunction(L, geopolygon_gc);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	luaL_newmetatable(L, LUAH3_LINKEDGEOPOLYGON);
	lua_pushcfunction(L, linkedgeopolygon_gc);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);

	return 1;
}
