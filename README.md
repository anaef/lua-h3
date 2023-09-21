# Lua H3

## Introduction

Lua H3 provides a Lua binding for the [H3](https://h3geo.org/) library. H3 is a hexagonal
hierarchical geospatial indexing system.

Here is a quick example:

```lua
local h3 = require("h3")

local lat, lng = 47.37, 8.55
print(string.format("Coordinates: %.4f %.4f", lat, lng))
local cell = h3.latlngtocell(47.37, 8.55, 8)
print(string.format("Cell at resolution 8: %x", cell))
local lat, lng = h3.celltolatlng(cell)
print(string.format("Centroid coordinates: %.4f %.4f", lat, lng))
local parentCell = h3.celltoparent(cell, 7)
print(string.format("Parent cell at resolution 7: %x", parentCell))
local childCells = h3.celltochildren(parentCell, 8)
for i, childCell in ipairs(childCells) do
        childCells[i] = h3.h3tostring(childCell)
end
print(string.format("Child cells at resolution 8: %s", table.concat(childCells, " ")))
```

Output:

```
Coordinates: 47.3700 8.5500
Cell at resolution 8: 881f8ed957fffff
Centroid coordinates: 47.3711 8.5488
Parent cell at resolution 7: 871f8ed95ffffff
Child cells at resolution 8: 881f8ed951fffff 881f8ed953fffff 881f8ed955fffff 881f8ed957fffff 881f8ed959fffff 881f8ed95bfffff 881f8ed95dfffff
```


## Build, Test, and Install

### Dependencies

You may need to build and install H3 in order to build Lua H3. Please see the
[H3 installation documentation](https://h3geo.org/docs/installation).

To build a shared library, you can add `-DBUILD_SHARED_LIBS=1` to the H3 cmake call.


### Building and Installing with LuaRocks

To build and install with LuaRocks, run:

```
luarocks install lua-h3
```


### Building, Testing and Installing with Make

Lua H3 comes with a simple Makefile. Please adapt the Makefile to your environment, and then
run:

```
make
make test
make install
```

## Release Notes

Please see the [release notes](NEWS.md) document.


## Documentation

Please browse the [documentation](doc/) folder for the extensive documentation.


## Limitations

Lua H3 supports Lua 5.3, and Lua 5.4.

Lua H3 has been built and tested on Ubuntu Linux (64-bit).


## License

Lua H3 is released under the MIT license. See LICENSE for license terms.
