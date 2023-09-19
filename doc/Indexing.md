# Indexing Functions

Lua H3 binds the following indexing functions.


## `h3.latlngtocell (lat, lng, res)`

Returns the cell containing the specified latitude and longitude at the specified resolution.


## `h3.celltolatlng (cell)`

Returns the latitude and longitude of the centroid of the specified cell.


## `h3.celltoboundary (cell)`

Returns a list of lists of latitude and longitude representing the boundary of the specified
cell.
