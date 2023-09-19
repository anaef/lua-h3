# Indexing Functions

Lua H3 binds the following indexing functions.


## `h3.latlngtocell (lat, lng, res)`

Returns the cell of a latitude and longitude at the specified resolution.


## `h3.celltolatlng (cell)`

Returns the latitude and longitude of a cell.


## `h3.celltoboundary (cell)`

Returns a list of lists of latitudes and longitudes representing the boundary of the cell.
