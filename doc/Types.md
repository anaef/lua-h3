# Types

## Index

H3 uses indexes to represent cells, directed edges, and vertexes. These indexes are 64-bit
integers internally and in Lua.


## Latitude and Longitude

Latitude and longitude are internally processed as double values expressed in radians. In Lua,
they are numbers expressed in degrees. The binding automatically converts between radians and
degrees.


## Resolution

H3 supports resolutions `0` (coarsest) to `15` (finest). Resolutions are integers.
