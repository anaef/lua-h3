# Directed Edge Functions

Lua H3 binds the following directed edge functions.


## `h3.areneighborcells (origin, dest)`

Returns whether the specified cells are neighbors.


## `h3.cellstoedge (origin, dest)`

Returns the directed edge between an origin and a destination cell.


## `h3.isedge (index)`

Returns whether the index represents a directed edge.


## `h3.edgetocells (edge [, mode])`

If mode is `"both"` (the default), the function returns the origin and destination cell of the
directed edge; if mode is `"origin"`, the function returns the origin cell of the directed edge;
if mode is `"destination`", the function returns the destination cell of the directed edge.


## `h3.origintoedges (cell)`

Returns a list of the up to six directed eges of an origin cell.


## `h3.edgetoboundary (edge)`

Returns the coordinates of the directed edge as a list of lists of latitutde and longitude.
