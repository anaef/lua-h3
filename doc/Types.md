# Types

This section describes the types used by H3 and Lua H3.

> [!NOTE]
> If you are new to H3, you may want to read the
> [aggregation](https://h3geo.org/docs/highlights/aggregation) and
> [indexing](https://h3geo.org/docs/highlights/indexing) introductions, and get familiar with
> some [H3 terminology](https://h3geo.org/docs/library/terminology).


## Index

H3 uses [indexes](https://h3geo.org/docs/core-library/h3Indexing) to represent
[cells](https://h3geo.org/docs/library/index/cell),
[directed edges](https://h3geo.org/docs/library/index/directededge),
and [vertexes](https://h3geo.org/docs/library/index/vertex). These indexes are 64-bit
integers internally and in Lua.


## Latitude and Longitude

Latitude and longitude are internally processed as double values expressed in radians. In Lua,
they are numbers expressed in degrees. The binding automatically converts between radians and
degrees.


## Resolution

H3 supports hierarchical [resolutions](https://h3geo.org/docs/core-library/restable) ranging
from `0` (coarsest) to `15` (finest). Resolutions are integers.


## List and Set

This documentations uses the terms _list_ and _set_ to refer to the ordinary array use of Lua
tables, i.e., tables with integer keys.

> [!IMPORTANT]
> The term _set_ is used whenever uniqueness of the contained values is required, i.e., when
> a list must not contain duplicates.
