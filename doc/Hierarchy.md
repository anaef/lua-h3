# Hierarchy Functions

Lua H3 binds the following hierarchy functions.


## `h3.celltoparent (cell, parentres)`

Returns the parent cell of the specified cell at the specified coarser resolution.


## `h3.celltochildren (cell, childres)`

Returns a list of child cells of the specified cell at the specified finer resolution.


## `h3.celltocenterchild (cell, childres)`

Returns the center child cell of the specified cell at the specified finer resolution.


## `h3.compactcells (cells)`

Returns a set of cells that best compacts the provided set of same-resolution cells.


## `h3.uncompactcells (cells, res)`

Returns a set of cells at the specified resolution that uncompacts the provided set of cells.
