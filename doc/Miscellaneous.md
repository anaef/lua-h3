# Miscellaneous Functions

Lua H3 binds the following [miscellaneous functions](https://h3geo.org/docs/api/misc).


## `h3.hexagonavg (res, quantity [, unit])`

Returns average quantities of a hexagon at the specified resolution. The argument `quantity`
must be `"area"` or `"edge"` to query the average area or edge length, respectively. The
optional `unit` argument can take the values `"m"` (the default) or `"km"` to query the quantity
in (square) meters or (square) kilometers, respectively.


## `h3.cellarea (cell [, unit])`

Returns the area of the specified cell. The optional `unit` argument can take the values
`"m"` (the default), `"km"`, or `"rad"` to query the area in square meters, square kilometers,
or square radians, respectively.


## `h3.edgelength (edge [, unit])`

Returns the length of the specified edge. The optional `unit` argument can take the values
`"m"` (the default), `"km"`, or `"rad"` to query the length in meters, kilometers, or radians,
respectively.


## `h3.numcells (res)`

Returns the total number of cells at the specified resolution.


## `h3.res0cells ()`

Returns a list of the cells at resolution `0`.


## `h3.pentagons (res)`

Returns a list of the pentagons at the specified resolution.


## `h3.greatcircledistance (lat1, lng1, lat2, lng2 [, unit])`

Returns the great circle distance between two coordinates. The optional `unit` argument can take
the values `"m"` (the default), `"km"`, or `"rad"` to query the distance in meters, kilometers,
or radians, respectively.
