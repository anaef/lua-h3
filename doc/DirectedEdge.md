# Directed Edge Functions

Lua H3 binds the following [directed edge functions](https://h3geo.org/docs/api/uniedge).


## `h3.areneighborcells (origin, dest)`

Returns whether the specified origin and destination cells are neighbors.


## `h3.cellstoedge (origin, dest)`

Returns the directed edge between the specified origin and destination cells.


## `h3.isedge (index)`

Returns whether the specified index represents a directed edge.


## `h3.edgetocells (edge [, mode])`

If mode is `"both"` (the default), the function returns the origin and destination cells of the
specified directed edge. If mode is `"origin"`, the function returns only the origin cell, and if
mode is `"destination"`, the function returns only the destination cell.


## `h3.origintoedges (cell)`

Returns a list of the up to six directed edges of the specified origin cell.


## `h3.edgetoboundary (edge)`

Returns the coordinates of the specified directed edge as a list of lists of latitude and
longitude.
