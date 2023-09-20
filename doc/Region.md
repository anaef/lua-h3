# Region Functions

Lua H3 binds the following [region functions](https://h3geo.org/docs/api/regions).


## `h3.polygontocells (polygon, res)`

Returns a list of cells at the specified resolution that are contained by a polygon. Containment
is determined by the centroid of each cell. The polygon is represented as a list of its outer
ring followed by zero or more holes. Rings and holes are each represented as a list of lists
of latitude and longitude.

> [!IMPORTANT]
> Following [GeoJSON](https://geojson.org/), rings must be counterclockwise, and holes must be
> clockwise. Both must be closed, and have at least four positions.

Example:

```lua
local LAT, LNG = 47, 8
local ring = {
	{ LAT, LNG },
	{ LAT, LNG + 1},
	{ LAT + 1, LNG + 1 },
	{ LAT + 1, LNG },
	{ LAT, LNG }
}
local hole = {
	{ LAT + 0.25, LNG + 0.25 }, 
	{ LAT + 0.75, LNG + 0.25 }, 
	{ LAT + 0.75, LNG + 0.75 }, 
	{ LAT + 0.25, LNG + 0.75 },
	{ LAT + 0.25, LNG + 0.25 } 
}
local cells = h3.polygontocells({ ring, hole }, 8)
```


## `h3.cellstopolygon (cells)`

Returns a list of polygons representing the specified set of same-resolution cells. Each polygon
has the same format as described above.
