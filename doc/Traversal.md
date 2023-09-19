# Traversal Functions

Lua H3 binds the following traversal functions.


## `h3.griddisk (origin, k [, mode])`

Returns a list of cell indexes within `k` distance of the origin cell.

If `mode` contains the letter `'d'`, the function additionally returns a list of distances.

If `mode` contains the letter `'u'`, the function uses a faster, but unsafe implementation
which generates an error when a pentagonal distortion is encountered. By default, the function
tries the unsafe version, and transparently falls back to the safe, but slower version in case of
an error.


## `h3.gridring (origin, k)`

Returns a list of cell indexes within `k` distance of the origin cell, excluding the origin cell.

The function generates an error if a pentagonal distortion is encountered.


## `h3.gridpathcells (origin, dest)`

Returns a list of cells on the line between an origin and a desination cell.


## `h3.griddistance (origin, dest)`

Returns the distance in cells between an origin and a destination cell.


## `h3.celltolocalij (origin, dest)`

Returns local IJ coordinates for a destination cell relative to an origin cell.


## `h3.localijtocell (origin, i, j)`

Returns a destination cell for an origin cell and local IJ coordinates.
