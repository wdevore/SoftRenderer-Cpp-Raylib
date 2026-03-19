# How to push into the std::\<vector>
Once the struct is named TransformedVertex, here are three common ways you can push elements into your vector, depending on your preferred style:

1. Creating an instance first (Most explicit)
You can declare the variable, set its fields, and then pass it to push_back().
```cpp
#include <vector>
#include "TransformedVertex.h"

std::vector<TransformedVertex> vertices;

TransformedVertex v;
v.screenPos = Vector3f(10.0f, 20.0f, 0.5f);
v.color = CColor(255, 0, 0, 255); // Assuming RGBA
v.clipped = false;

vertices.push_back(v);
```

2. Inline brace initialization (Clean and concise)
Since C++11, you can use aggregate initialization to construct the struct right inside the push_back() call. The compiler automatically figures out the types based on the struct's definition.

```cpp
std::vector<TransformedVertex> vertices;

// screenPos {x, y, z}, color {r, g, b, a}, clipped
vertices.push_back({ {15.0f, 30.0f, 1.0f}, {0, 255, 0, 255}, true }); 
```

3. Using emplace_back (Modern & efficient)
emplace_back is generally preferred in modern C++ because it can construct objects in-place directly in the vector's memory, avoiding unnecessary copies. For aggregate structs using brace initialization, you still provide the type name:

```cpp
std::vector<TransformedVertex> vertices;

vertices.emplace_back(TransformedVertex{ {100.0f, 200.0f, 1.5f}, {0, 0, 255, 255}, false });
```