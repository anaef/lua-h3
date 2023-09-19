# Traversal Functions

Lua H3 binds the following traversal functions.


## `h3.griddisk (origin, k [, mode])`

Returns a list of cells _within_ `k` distance of the specified origin cell.

If `mode` contains the letter `'d'`, the function additionally returns a list of distances from
the origin cell, expressed in cells.

If `mode` contains the letter `'u'`, the function uses a faster, but unsafe implementation
which generates an error when a pentagonal distortion is encountered. By default, the function
tries the unsafe version, and transparently falls back to the safe, but slower version in case of
an error.


## `h3.gridring (origin, k)`

Returns a list of cells  _at_ `k` distance of the specified origin cell.

The function generates an error if a pentagonal distortion is encountered.


## `h3.gridpathcells (origin, dest)`

Returns a list of cells on the line between the specified origin and destination cells.


## `h3.griddistance (origin, dest)`

Returns the distance in cells between the specified origin and destination cells.


## `h3.celltolocalij (origin, dest)`

Returns local IJ coordinates for the specified destination cell relative to the specified origin
cell.


## `h3.localijtocell (origin, i, j)`

Returns the destination cell for the specified origin cell and local IJ coordinates.
