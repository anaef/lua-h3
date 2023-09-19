# Vertex Functions

Lua H3 binds the following vertex functions.


## `h3.celltovertexes (origin [, num])`

Returns the list of vertexes of the specified origin cell.

If the argument `num` is present, the function returns a single vertex. Valid values for `num`
are `1` to `6` for hexagonal cells, and `1` to `5` for pentagonal cells.


## `h3.vertextolatlng (vertex)`

Returns the latitude and longitude of the specified vertex.


## `h3.isvertex (index)`

Returns whether the specified index represents a vertex.
