# Inspection Functions

Lua H3 binds the following [inspection functions](https://h3geo.org/docs/api/inspection).


## `h3.resolution (index)`

Returns the resolution of the specified index.


## `h3.basecellnumber (index)`

Returns the base cell number of the specified index.


## `h3.stringtoh3 (str)`

Converts the specified string representation to an index.


## `h3.h3tostring (index)`

Converts the specified index to a string representation.


## `h3.iscell (index)`

Returns whether the specified index is a cell.


## `h3.isresclassiii (index)`

Returns whether the specified index is
[class III](https://h3geo.org/docs/core-library/coordsystems).


## `h3.ispentagon (index)`

Returns whether the specified index is a pentagonal cell.


## `h3.icosahedronfaces (index)`

Returns a list of icosahedron faces intersected by the specified index.
