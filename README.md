# Lua H3

## Introduction

Lua H3 provides Lua bindings for the [H3](https://h3geo.org/) library. H3 is a hexagonal
hierarchical geospatial indexing system.


## Build, Test, and Install

### Dependencies

You may need to install H3 first in order to build Lua H3. Please see the
[H3 installation documentation](https://h3geo.org/docs/installation).

To build a shared library, you can add `-DBUILD_SHARED_LIBS=1` to the cmake call.


### Building with Make

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
