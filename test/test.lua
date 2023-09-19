local h3 = require("h3")

-- test parameters
local LAT, LNG = 47, 8
local RES, TOL = 8, 0.01

-- version
local v, major, minor, patch = h3.version()
assert(type(v) == "string" and string.len(v) >= 5)
assert(type(major) == "number" and major >= 0)
assert(type(minor) == "number" and minor >= 0)
assert(type(patch) == "number" and patch >= 0)

-- indexing
local cell = h3.latlngtocell(LAT, LNG, RES)
assert(type(cell) == "number")
assert(math.type(cell) == "integer")
local lat, lng = h3.celltolatlng(cell)
assert(math.abs(lat - LAT) < TOL)
assert(math.abs(lng - LNG) < TOL)
local boundary = h3.celltoboundary(cell)
assert(#boundary == 6)
for _, entry in ipairs(boundary) do
	assert(math.abs(entry[1] - LAT) < TOL)
	assert	(math.abs(entry[2] - LNG) < TOL)
end

-- inspection
local cell = h3.latlngtocell(LAT, LNG, RES)
assert(h3.resolution(cell) == RES)
assert(h3.basecellnumber(cell) == 15)
assert(h3.stringtoh3(string.format("%x", cell)) == cell)
assert(h3.h3tostring(cell) == string.format("%x", cell))
assert(h3.iscell(cell))
assert(not h3.iscell(0))
assert(not h3.isresclassiii(cell))
assert(h3.isresclassiii(h3.latlngtocell(LAT, LNG, RES - 1)))
assert(not h3.ispentagon(cell))
local faces = h3.icosahedronfaces(cell)
assert(#faces > 0)
for _, face in ipairs(faces) do
	assert(face >= 0 and face <= 19)
end

-- traversal
local cell = h3.latlngtocell(LAT, LNG, RES)
local cells = h3.griddisk(cell, 0)
assert(#cells == 1 and cells[1] == cell)
local cells = h3.griddisk(cell, 1)
assert(#cells == 7)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local cells, distances = h3.griddisk(cell, 1, "d")
assert(#cells == 7 and #distances == 7)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
for _, distance in ipairs(distances) do
	assert(math.type(distance) == "integer" and distance == 0 or distance == 1)
end
local cells = h3.griddisk(cell, 1, "u")
assert(#cells == 7)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local cells, distances = h3.griddisk(cell, 1, "ud")
assert(#cells == 7 and #distances == 7)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
for _, distance in ipairs(distances) do
	assert(math.type(distance) == "integer" and distance == 0 or distance == 1)
end
local cells = h3.gridring(cell, 0)
assert(#cells == 1 and cells[1] == cell)
for i = 1, 31 do
	local cells = h3.gridring(cell, i)
	assert(#cells == i * 6)
	for _, cell in ipairs(cells) do
		assert(h3.iscell(cell))
	end
end
local cell1 = h3.latlngtocell(LAT + 1, LNG + 1, RES)
local cells = h3.gridpathcells(cell, cell1)
assert(#cells > 100)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local distance = h3.griddistance(cell, cell1)
assert(distance == #cells - 1)
local i, j = h3.celltolocalij(cell, cell1)
assert(math.type(i) == "integer" and math.type(j) == "integer")
assert(h3.localijtocell(cell, i, j) == cell1)

-- hierarchy
local cell = h3.latlngtocell(LAT, LNG, RES)
local parent = h3.celltoparent(cell, RES - 1)
assert(h3.resolution(parent) == RES - 1)
local children = h3.celltochildren(parent, RES)
assert(#children == 7)
local found = nil
for _, child in ipairs(children) do
	if child == cell then
		found = true
		break
	end
end
assert(found)
local centerchild = h3.celltocenterchild(parent, RES)
local distance = h3.griddistance(centerchild, cell)
assert(distance == 0 or distance == 1)
local compactcells = h3.compactcells(children)
assert(#compactcells == 1)
assert(compactcells[1] == parent)
local uncompactcells = h3.uncompactcells({parent}, RES)
assert(#uncompactcells == 7)
for _, cell in ipairs(uncompactcells) do
	h3.iscell(cell)
end

-- region
local polygon = {
	{ LAT, LNG },
	{ LAT + 1, LNG },
	{ LAT + 1, LNG + 1 },
	{ LAT, LNG + 1 }
}
local cells = h3.polygontocells({polygon}, 8)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local hole = {
	{ LAT + 0.25, LNG + 0.25 }, 
	{ LAT + 0.25, LNG + 0.75 }, 
	{ LAT + 0.75, LNG + 0.75 }, 
	{ LAT + 0.75, LNG + 0.25 }
}
local partialCells = h3.polygontocells({polygon, hole}, 8)
assert(#partialCells < #cells)
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local polygons = h3.cellstopolygons(cells)
assert(#polygons == 1)
local polygon = polygons[1]
assert(#polygon > 0)
local loop = polygon[1]
assert(#loop > 100)
for _, latLng in ipairs(loop) do
	assert(#latLng == 2)
	assert(math.abs(latLng[1] - LAT) < 1 + TOL)
	assert(math.abs(latLng[2] - LNG) < 1 + TOL)
end

-- directed edge
local cell = h3.latlngtocell(LAT, LNG, RES)
local parent = h3.celltoparent(cell, RES - 1)
local children = h3.celltochildren(parent, RES)
local center = h3.celltocenterchild(parent, RES)
local neighbor = nil
for _, child in ipairs(children) do
	if child == center then
		assert(not h3.areneighborcells(center, child))
	else
		assert(h3.areneighborcells(center, child))
		if not neighbor then
			neighbor = child
		end
	end
end
local edge = h3.cellstoedge(center, neighbor)
assert(math.type(edge) == "integer")
assert(h3.isedge(edge))
assert(not h3.isedge(center))
local origin = h3.edgetocells(edge, "origin")
assert(origin == center)
local destination = h3.edgetocells(edge, "destination")
assert(destination == neighbor)
local origin, destination = h3.edgetocells(edge)
assert(origin == center and destination == neighbor)
local edges = h3.origintoedges(center)
assert(#edges == 6)
local found = nil
for _, _edge in ipairs(edges) do
	assert(h3.isedge(_edge))
	found = found or _edge == edge
end
assert(found)
local boundary = h3.edgetoboundary(edge)
assert(#boundary >= 2)
for _, entry in ipairs(boundary) do
	assert(math.abs(entry[1] - LAT) < TOL)
	assert(math.abs(entry[2] - LNG) < TOL)
end

-- vertex
local cell = h3.latlngtocell(LAT, LNG, RES)
local vertex = h3.celltovertexes(cell, 1)
assert(math.type(vertex) == "integer")
assert(h3.isvertex(vertex))
assert(not h3.isvertex(cell))
local vertexes = h3.celltovertexes(cell)
assert(#vertexes == 6)
local found = nil
for _, _vertex in ipairs(vertexes) do
	assert(h3.isvertex(_vertex))
	found = found or _vertex == vertex
end
assert(found)
local lat, lng = h3.vertextolatlng(vertex)
assert(math.abs(lat - LAT) < TOL)
assert(math.abs(lng - LNG) < TOL)

-- miscellaneous
local areaAvgM = h3.hexagonavg(RES, "area")
assert(areaAvgM >= 600000 and areaAvgM <= 900000)
local areaAvgKm = h3.hexagonavg(8, "area", "km")
assert(areaAvgKm >= 0.6 and areaAvgKm <= 0.9)
assert(math.abs(areaAvgM / areaAvgKm - 1000000) < 1e-06)
local edgeLengthAvgM = h3.hexagonavg(RES, "edge")
assert(edgeLengthAvgM >= 400 and edgeLengthAvgM <= 500)
local edgeLengthAvgKm = h3.hexagonavg(RES, "edge", "km")
assert(edgeLengthAvgKm >= 0.4 and edgeLengthAvgKm <= 0.5)
assert(math.abs(edgeLengthAvgM / edgeLengthAvgKm - 1000) < 1e-06)
local cell = h3.latlngtocell(LAT, LNG, RES)
local cellAreaM = h3.cellarea(cell)
assert(math.abs(cellAreaM - areaAvgM) < 100000)
local cellAreaKm = h3.cellarea(cell, "km")
assert(math.abs(cellAreaKm - areaAvgKm) < 0.1)
assert(math.abs(cellAreaM / cellAreaKm - 1000000) < 1e-06)
local cellAreaRad = h3.cellarea(cell, "rad")
assert(cellAreaRad > 0 and cellAreaRad < 1e-07)
local edges = h3.origintoedges(cell)
local edge = edges[1]
local edgeLengthM = h3.edgelength(edge)
assert(math.abs(edgeLengthM - edgeLengthAvgM) < 100)
local edgeLengthKm = h3.edgelength(edge, "km")
assert(math.abs(edgeLengthM / edgeLengthKm - 1000) < 1e-06)
local edgeLengthRad = h3.edgelength(edge, "rad")
assert(edgeLengthRad > 0 and edgeLengthRad < 0.0001)
assert(h3.numcells(0) == 122)
assert(h3.numcells(RES - 1) < h3.numcells(RES))
assert(h3.numcells(RES) < h3.numcells(RES + 1))
local cells = h3.res0cells()
assert(#cells == h3.numcells(0))
for _, cell in ipairs(cells) do
	assert(h3.iscell(cell))
end
local pentagons = h3.pentagons(RES)
assert(#pentagons == 12)
for _, cell in ipairs(pentagons) do
	assert(h3.iscell(cell))
	assert(h3.ispentagon(cell))
end
local distanceM = h3.greatcircledistance(LAT, LNG, LAT + 1, LNG + 1)
assert(distanceM >= 100000 and distanceM <= 150000)
local distanceKm = h3.greatcircledistance(LAT, LNG, LAT + 1, LNG + 1, "km")
assert(distanceKm >= 100 and distanceKm <= 150)
assert(math.abs(distanceM / distanceKm - 1000) < 1e-06)
local distanceRad = h3.greatcircledistance(LAT, LNG, LAT + 1, LNG + 1, "rad")
assert(distanceRad >= 0.01 and distanceRad <= 0.03)
