# Region Functions

Lua H3 binds the following region functions.


## `h3.polygontocells (polygon, res)`

Returns a list of cells at the specified resolution that are contained by the specified polygon.
Containment is determined by the centroid of each cell. The polygon is represented as a list of
its outer loop followed by zero or more holes. Loops and holes are represented as lists of lists
of latitude and longitude.


Example:

```lua
local LAT, LNG = 47, 8
local loop = {
	{ LAT, LNG },
	{ LAT + 1, LNG },
	{ LAT + 1, LNG + 1 },
	{ LAT, LNG + 1 }
}
local hole = {
	{ LAT + 0.25, LNG + 0.25 }, 
	{ LAT + 0.25, LNG + 0.75 }, 
	{ LAT + 0.75, LNG + 0.75 }, 
	{ LAT + 0.75, LNG + 0.25 }
}
local cells = h3.polygontocells({ loop, hole }, 8)
```

## `h3.cellstopolygon (cells)`

Returns a list of polygons representing the specified set of same-resolution cells.
