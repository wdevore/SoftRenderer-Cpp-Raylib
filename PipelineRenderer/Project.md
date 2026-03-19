# Description
A simple pipeline software rasterizer.

# 3D Objects
A 3D object is split into two parts:
- Vertices
- Indices

Vertices are stored in a single collection, but virtually broken into groups. For example, a Cube and Plane both have their vertices in a single collection but there indices are stored in separate objects.

Thus objects simply hold integer references back to the vertex collection.

When rendering all vertices are transformed into a separate transformed collection.

Transforming can be done in several steps depending on what the vertices are used for.
