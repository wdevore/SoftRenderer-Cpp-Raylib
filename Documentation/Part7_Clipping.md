
# Clipping
In a standard graphics pipeline, clipping happens in **Clip Space**, immediately after the Vertex Shader and before the Perspective Divide.

Since your code expects to work on the frustum, it should sit between **Stage 1 (Vertex Shading)** and **Stage 2 (Primitive Assembly)** in the `MyDrawIndexed` function.

### Where to Insert Clipping

Instead of just checking `if (clip.w < 0.1f)` and moving on, you need to perform **Triangle Clipping**. This is more complex than vertex clipping because a single triangle clipped against the frustum can result in zero, one, or two new triangles (a fan).

Here is the architectural placement for your software pipeline:

```cpp
// 1. VERTEX SHADING
// Transform all vertices to Clip Space (but don't divide by W yet!)
for (int i = 0; i < vCount; i++) {
    vertexCache[i] = VertexShader(vertices[i], sr->mvp);
}

// 2. PRIMITIVE ASSEMBLY & CLIPPING
for (int i = 0; i < iCount; i += 3) {
    // Get the three vertices in Clip Space
    Vector4 c0 = vertexCache[indices[i]];
    Vector4 c1 = vertexCache[indices[i+1]];
    Vector4 c2 = vertexCache[indices[i+2]];

    // --- INSERT YOUR CLIPPING CODE HERE ---
    // Your code should take c0, c1, c2 and test them against the frustum planes:
    // -w <= x <= w, -w <= y <= w, 0 <= z <= w (or -w <= z <= w depending on your convention)
    
    int numTriangles = ClipTriangleAgainstFrustum(c0, c1, c2, clippedTriangles);

    for (int t = 0; t < numTriangles; t++) {
        // 3. PERSPECTIVE DIVIDE
        // Now that the geometry is clipped, safely divide by W
        // and transform to Screen Space.
        Vector3 s0 = PerspectiveDivide(clippedTriangles[t].v0, sr->width, sr->height);
        Vector3 s1 = PerspectiveDivide(clippedTriangles[t].v1, sr->width, sr->height);
        Vector3 s2 = PerspectiveDivide(clippedTriangles[t].v2, sr->width, sr->height);

        // 4. RASTERIZATION
        RasterizeTriangle(sr, s0, s1, s2, ...);
    }
}
```

---

### Key Requirements for the Clipper

* **Attribute Interpolation:** When your clipper creates a new vertex on a frustum edge, you must also interpolate the **Color** (and other attributes) at that new point. This ensures the Gouraud shading remains smooth.
* **W-Component:** You must keep the $W$ component intact during clipping. The Perspective Divide ($x/w, y/w, z/w$) happens *after* the triangle has been clipped.
* **Coordinate Space:** Your clipper should operate on coordinates where:
* **Near Plane:** $z = 0$ (or $z = -w$)
* **Far Plane:** $z = w$
* **Sides:** $x = \pm w, y = \pm w$



### Why not Clip in Screen Space?

Clipping in Screen Space (after the divide) is mathematically difficult because the linear interpolation of $Z$ and attributes is no longer valid once you've divided by $W$. Clipping in Clip Space (Homogeneous space) preserves the linearity of the math.

Would you like me to update the **Markdown Export** to include this "Stage 1.5: Frustum Clipping" logic?

### Stage 1: Vertex Shading & Clipping

1. Transform vertices to **Clip Space** using the MVP.
2. **Clip** triangles against the frustum planes ($-w \le x,y,z \le w$).
3. Perform **Perspective Divide**: $(x/w, y/w, z/w)$.

### Stage 2: Optimized Rasterization

Uses **Incremental Edge Evaluation** to avoid expensive multiplications in the inner loop.

```cpp
// Per-pixel loop logic:
// 1. Calculate Edge Constants (dx, dy) once per triangle.
// 2. Initialize Edge values (w0, w1, w2) at bounding box start.
// 3. For each row:
//      For each pixel:
//          if (w0 | w1 | w2) >= 0: // Point-in-triangle test
//              Interpolate Z and Color via weights
//              Depth Test -> Write Pixel
//          w0 += dy12; w1 += dy20; w2 += dy01; // Step Right
//      w0_row += dx12; ... // Step Down

```

## 4. Lighting & Debugging

* **Flat Shading:** Calculated via `dot(FaceNormal, LightDir)`.
* **Wireframe:** Renders edges via **Bresenham’s Line Algorithm** instead of the Barycentric filler.
* **Backface Culling:** Skips triangles where the normal points away from the View Vector.

---

Would you like me to provide a specific **Frustum Clipping** function that integrates directly with the `Vector4` Clip Space output of your Vertex Shader?
To implement a proper frustum clipper, you need to clip against the 6 planes of the viewing volume in **Clip Space** ($w$ space). The most common algorithm is the **Sutherland-Hodgman** algorithm, which clips a polygon against a single plane at a time.

Since we are doing a software rasterizer, we clip the triangle against the plane, which may result in a polygon with 3 or 4 vertices, which we then split back into triangles.

### 1. The Clipping Math

A point $P$ is inside a frustum plane if its component is within the bounds of $w$. For example, for the "Near" plane in OpenGL: $z \ge 0$ (or $z \ge -w$ depending on your matrix).

The intersection point $I$ between two vertices $A$ and $B$ is calculated using a factor $t$:


$$t = \frac{w_A - z_A}{(w_A - z_A) - (w_B - z_B)}$$

$$I = A + t(B - A)$$

---

### 2. Implementation: Clipping a Triangle against a Plane

This function takes an input list of vertices and clips them against a specific plane, returning a new list.

```cpp
typedef struct {
    Vector4 position; // Clip Space
    Color color;
} ClipVertex;

// Clips a polygon against a single plane (e.g., the Near Plane)
int ClipAgainstPlane(ClipVertex* in, int inCount, ClipVertex* out, int plane) {
    int outCount = 0;
    for (int i = 0; i < inCount; i++) {
        ClipVertex a = in[i];
        ClipVertex b = in[(i + 1) % inCount];

        // Plane test logic (Example: Near Plane z = 0)
        bool aInside = a.position.z >= 0;
        bool bInside = b.position.z >= 0;

        if (aInside != bInside) {
            float t = a.position.z / (a.position.z - b.position.z);
            out[outCount].position = Vector4Lerp(a.position, b.position, t);
            // Interpolate color so shading stays smooth
            out[outCount].color = ColorLerp(a.color, b.color, t);
            outCount++;
        }

        if (bInside) {
            out[outCount++] = b;
        }
    }
    return outCount;
}

```

---

### 3. Integrating into the Pipeline

In your `MyDrawIndexed` function, instead of drawing the triangle immediately, you pass it through the clipping stages.

```cpp
void ProcessTriangle(SoftRenderer *sr, ClipVertex v0, ClipVertex v1, ClipVertex v2) {
    ClipVertex buffer1[8];
    ClipVertex buffer2[8];

    buffer1[0] = v0; buffer1[1] = v1; buffer1[2] = v2;
    int count = 3;

    // Clip against all 6 planes
    count = ClipAgainstPlane(buffer1, count, buffer2, NEAR_PLANE);
    count = ClipAgainstPlane(buffer2, count, buffer1, FAR_PLANE);
    // ... repeat for Left, Right, Top, Bottom ...

    // After clipping, 'buffer1' contains a polygon. 
    // We convert it back into a "Triangle Fan" for the rasterizer.
    for (int i = 1; i < count - 1; i++) {
        // 1. Perspective Divide
        TransformedVertex tv0 = Project(buffer1[0], sr->width, sr->height);
        TransformedVertex tv1 = Project(buffer1[i], sr->width, sr->height);
        TransformedVertex tv2 = Project(buffer1[i+1], sr->width, sr->height);

        // 2. Rasterize
        RasterizeTriangleOptimized(sr, tv0, tv1, tv2, tv0.color);
    }
}

```

---

### Summary for the Markdown Export

I have updated the final reference to include the proper placement of the Clipper.

**Key takeaway:** Clipping prevents math errors like "division by zero" or "wraparound" that happen when vertices go behind the camera. It is the "bridge" between your Vertex Shader and your Rasterizer.

Would you like the final **all-in-one code block** that combines all these parts into a single copy-pasteable file?

Here is the comprehensive, all-in-one implementation. This header-style file combines the **OpenGL-spec projection**, the **Sutherland-Hodgman clipper**, and the **Incremental Barycentric Rasterizer**.

### The Complete Software Pipeline Reference

```cpp
#include "raylib.h"
#include "raymath.h"
#include <float.h>
#include <stdlib.h>

// --- Data Structures ---

typedef struct {
    Vector3 position;
    Color color;
} Vertex;

typedef struct {
    Vector4 pos;   // Clip Space (Pre-divide)
    Color color;
} ClipVertex;

typedef struct {
    Vector3 screenPos; // Screen Space (Post-divide)
    Color color;
} FinalVertex;

typedef struct {
    int width, height;
    Image colorBuffer;
    float* depthBuffer;
    Texture2D displayTex;
    Matrix mvp;
} SoftRenderer;

// --- Pipeline Stages ---

// 1. OpenGL-spec Perspective Matrix
Matrix ComputeProjection(float fov, float aspect, float near, float far) {
    float top = near * tanf(fov * 0.5f * (PI / 180.0f));
    float right = top * aspect;
    return (Matrix){
        near/right, 0, 0, 0,
        0, near/top, 0, 0,
        0, 0, -(far + near)/(far - near), -1,
        0, 0, -(2*far*near)/(far - near), 0
    };
}

// 2. Sutherland-Hodgman Near-Plane Clipper
// We clip against Z=0 in Clip Space to prevent W <= 0 issues
int ClipNearPlane(ClipVertex* in, int inCount, ClipVertex* out) {
    int outCount = 0;
    for (int i = 0; i < inCount; i++) {
        ClipVertex a = in[i];
        ClipVertex b = in[(i + 1) % inCount];

        bool aIn = a.pos.z <= 0; // OpenGL Near Plane is at Z=0 to Z=-W
        bool bIn = b.pos.z <= 0;

        if (aIn != bIn) {
            float t = a.pos.z / (a.pos.z - b.pos.z);
            out[outCount].pos = Vector4Lerp(a.pos, b.pos, t);
            out[outCount].color = ColorLerp(a.color, b.color, t);
            outCount++;
        }
        if (bIn) out[outCount++] = b;
    }
    return outCount;
}

// 3. Incremental Barycentric Rasterizer
void Rasterize(SoftRenderer* sr, FinalVertex v0, FinalVertex v1, FinalVertex v2) {
    // Bounding Box
    int minX = (int)fmaxf(0, fminf(v0.screenPos.x, fminf(v1.screenPos.x, v2.screenPos.x)));
    int maxX = (int)fminf(sr->width - 1, fmaxf(v0.screenPos.x, fmaxf(v1.screenPos.x, v2.screenPos.x)));
    int minY = (int)fmaxf(0, fminf(v0.screenPos.y, fminf(v1.screenPos.y, v2.screenPos.y)));
    int maxY = (int)fminf(sr->height - 1, fmaxf(v0.screenPos.y, fmaxf(v1.screenPos.y, v2.screenPos.y)));

    // Edge Constants
    float dy01 = v0.screenPos.y - v1.screenPos.y; float dx01 = v1.screenPos.x - v0.screenPos.x;
    float dy12 = v1.screenPos.y - v2.screenPos.y; float dx12 = v2.screenPos.x - v1.screenPos.x;
    float dy20 = v2.screenPos.y - v0.screenPos.y; float dx20 = v0.screenPos.x - v2.screenPos.x;

    float area = (v1.screenPos.x - v0.screenPos.x) * (v2.screenPos.y - v0.screenPos.y) - (v1.screenPos.y - v0.screenPos.y) * (v2.screenPos.x - v0.screenPos.x);
    if (area <= 0) return; // Culling

    // Initial values at (minX, minY)
    float w0_start = (minX - v1.screenPos.x) * (v2.screenPos.y - v1.screenPos.y) - (minY - v1.screenPos.y) * (v2.screenPos.x - v1.screenPos.x);
    float w1_start = (minX - v2.screenPos.x) * (v0.screenPos.y - v2.screenPos.y) - (minY - v2.screenPos.y) * (v0.screenPos.x - v2.screenPos.x);
    float w2_start = (minX - v0.screenPos.x) * (v1.screenPos.y - v0.screenPos.y) - (minY - v0.screenPos.y) * (v1.screenPos.x - v0.screenPos.x);

    for (int y = minY; y <= maxY; y++) {
        float w0 = w0_start; float w1 = w1_start; float w2 = w2_start;
        for (int x = minX; x <= maxX; x++) {
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float weight0 = w0 / area; float weight1 = w1 / area; float weight2 = w2 / area;
                float z = weight0 * v0.screenPos.z + weight1 * v1.screenPos.z + weight2 * v2.screenPos.z;
                
                int idx = y * sr->width + x;
                if (z < sr->depthBuffer[idx]) {
                    sr->depthBuffer[idx] = z;
                    Color c = {(unsigned char)(v0.color.r*weight0 + v1.color.r*weight1 + v2.color.r*weight2),
                               (unsigned char)(v0.color.g*weight0 + v1.color.g*weight1 + v2.color.g*weight2),
                               (unsigned char)(v0.color.b*weight0 + v1.color.b*weight1 + v2.color.b*weight2), 255};
                    ((Color*)sr->colorBuffer.data)[idx] = c;
                }
            }
            w0 += (v2.screenPos.y - v1.screenPos.y); w1 += (v0.screenPos.y - v2.screenPos.y); w2 += (v1.screenPos.y - v0.screenPos.y);
        }
        w0_start += (v1.screenPos.x - v2.screenPos.x); w1_start += (v2.screenPos.x - v0.screenPos.x); w2_start += (v0.screenPos.x - v1.screenPos.x);
    }
}

```

---

### Final Markdown Export Summary

1. **Init:** Setup `SoftRenderer` with CPU Image and GPU Texture.
2. **Vertex Shader:** Multiply `Vertex.pos` by `MVP` to get `ClipVertex`.
3. **Clipping:** Pass triangles through `ClipNearPlane`. This solves the "division by zero" issue for geometry behind the eye.
4. **Perspective Divide:** Convert clipped vertices to `FinalVertex` via `pos.xyz / pos.w`.
5. **Rasterize:** Run the incremental loop to fill pixels.
6. **Blit:** `UpdateTexture` and `DrawTexture` in Raylib.

---

This consolidated guide covers the theory and implementation of line clipping for a 3D software rasterizer.
1. Where to Clip: Frustum vs. Cube
In a 3D pipeline, you should clip lines in 4D Homogeneous Clip Space (after the projection matrix, but before the perspective divide).

* The "Cube" (NDC): Usually refers to the space where coordinates are normalized between $-1$ and $1$. Clipping here is risky because the perspective divide ($x/w, y/w$) can flip the coordinates of points behind the camera ($w \le 0$), causing graphical glitches.
* The Frustum (Clip Space): Clipping here (where coordinates are $x, y, z, w$) allows you to handle points behind the camera gracefully and avoids division-by-zero errors.

------------------------------
2. The Mathematical Conditions
A point $(x, y, z, w)$ is considered inside the view volume if it satisfies these six inequalities:

* Left/Right: $-w \le x \le w$
* Bottom/Top: $-w \le y \le w$
* Near/Far: $0 \le z \le w$ (DirectX style) or $-w \le z \le w$ (OpenGL style)

To find the intersection point $P$ between an inside point $A$ and outside point $B$:

$$P = A + t(B - A)$$

The interpolation factor $t$ is calculated based on the specific plane being clipped. For example, for the Near Plane ($z=0$):

$$t = \frac{-z_A}{z_B - z_A}$$

------------------------------
To speed things up, we use **Outcodes**. Each vertex gets a 6-bit integer where each bit represents a plane (Left, Right, Bottom, Top, Near, Far).
If a bit is 1, the point is outside that plane. This allows for two massive shortcuts:
1. **Trivial Accept**: If both outcodes are `0`, the line is entirely inside. Just draw it.
2. **Trivial Reject**: If `(code1 & code2) != 0`, both points share an outside region (e.g., both are above the screen). Delete the line instantly.

---
3. Efficient Implementation (Cohen-Sutherland)
Using Outcodes allows you to skip expensive math for lines that are entirely on-screen or entirely off-screen.
C++ Data Structures and Outcodes

```cpp
struct Vec4 { float x, y, z, w; };
enum OutCode {
    INSIDE = 0,
    LEFT   = 1 << 0, // x < -w
    RIGHT  = 1 << 1, // x > w
    BOTTOM = 1 << 2, // y < -w
    TOP    = 1 << 3, // y > w
    NEAR   = 1 << 4, // z < 0
    FAR    = 1 << 5  // z > w
};
int computeOutCode(const Vec4& v) {
    int code = INSIDE;
    if (v.x < -v.w)      code |= LEFT;
    else if (v.x > v.w)  code |= RIGHT;
    if (v.y < -v.w)      code |= BOTTOM;
    else if (v.y > v.w)  code |= TOP;
    if (v.z < 0)         code |= NEAR; 
    else if (v.z > v.w)  code |= FAR;
    return code;
}
```
The Clipping Function
This function iteratively clips a line segment against the frustum boundaries.
```cpp
bool clipLine(Vec4& v1, Vec4& v2) {
    int code1 = computeOutCode(v1);
    int code2 = computeOutCode(v2);

    while (true) {
        if (!(code1 | code2)) return true;   // Trivial Accept
        if (code1 & code2) return false;    // Trivial Reject

        int codeOut = code1 ? code1 : code2;
        float t = 0.0f;

        // Intersection math for each plane
        if (codeOut & TOP)         t = (v1.w - v1.y) / ((v1.w - v1.y) - (v2.w - v2.y));
        else if (codeOut & BOTTOM) t = (v1.w + v1.y) / ((v1.w + v1.y) - (v2.w + v2.y));
        else if (codeOut & RIGHT)  t = (v1.w - v1.x) / ((v1.w - v1.x) - (v2.w - v2.x));
        else if (codeOut & LEFT)   t = (v1.w + v1.x) / ((v1.w + v1.x) - (v2.w + v2.x));
        else if (codeOut & NEAR)   t = (v1.z) / (v1.z - v2.z);
        else if (codeOut & FAR)    t = (v1.w - v1.z) / ((v1.w - v1.z) - (v2.w - v2.z));

        Vec4 intersected = {
            v1.x + t * (v2.x - v1.x),
            v1.y + t * (v2.y - v1.y),
            v1.z + t * (v2.z - v1.z),
            v1.w + t * (v2.w - v1.w)
        };

        if (codeOut == code1) {
            v1 = intersected;
            code1 = computeOutCode(v1);
        } else {
            v2 = intersected;
            code2 = computeOutCode(v2);
        }
    }
}
```
------------------------------
4. Important Considerations

   1. Perspective Divide: Only perform $x_{ndc} = x/w$ and $y_{ndc} = y/w$ after the line has been clipped.
   2. Attributes: If your vertices have colors, normals, or UVs, you must interpolate them using the same $t$ value during clipping to maintain visual consistency.
   3. Polygon Clipping: If you move from lines to triangles, you should use the Sutherland-Hodgman algorithm, which can result in a single triangle becoming a polygon with multiple vertices after clipping.

Do you want to explore how to apply this logic to triangle clipping, or should we look at perspective-correct attribute interpolation for your line segments?

---
```cpp
#include <iostream>
#include <vector>

struct Vec4 {
    float x, y, z, w;
};

// Linearly interpolate between two 4D points
Vec4 lerp(const Vec4& a, const Vec4& b, float t) {
    return {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        a.w + t * (b.w - a.w)
    };
}

// Simple line clipper for the Near Plane (z >= 0)
// Returns true if a segment exists, false if it's entirely clipped
bool clipLineNear(Vec4& v0, Vec4& v1) {
    float d0 = v0.z; // Distance from near plane (z=0)
    float d1 = v1.z;

    // Both points are behind the near plane
    if (d0 < 0 && d1 < 0) return false;

    // Both points are in front
    if (d0 >= 0 && d1 >= 0) return true;

    // One point is behind, one is in front: calculate intersection
    float t = d0 / (d0 - d1);
    if (d0 < 0) {
        v0 = lerp(v0, v1, t); // Move v0 to the plane
    } else {
        v1 = lerp(v0, v1, t); // Move v1 to the plane
    }
    return true;
}

int main() {
    // Example: A line from behind the camera to in front
    Vec4 p1 = { 0.0f, 0.0f, -5.0f, 1.0f }; // Behind near plane
    Vec4 p2 = { 0.0f, 0.0f,  5.0f, 1.0f }; // In front

    if (clipLineNear(p1, p2)) {
        std::cout << "Clipped Line Start: z=" << p1.z << ", w=" << p1.w << std::endl;
        std::cout << "Clipped Line End:   z=" << p2.z << ", w=" << p2.w << std::endl;
    } else {
        std::cout << "Line completely culled." << std::endl;
    }

    return 0;
}
```

In a 3D software rasterizer, it is generally considered **best practice to clip in the Viewing Frustum** (specifically in Homogeneous Clip Space) rather than in the NDC cube.

While you *can* clip in NDC space, doing so introduces a "Catch-22" regarding the division by $w$.

---

## 1. Why Clip in the Frustum (Clip Space)?

Clipping happens after the **Projection Transformation** but *before* the **Perspective Divide**. This is known as **Homogeneous Clip Space**.

### The Problem with NDC Clipping
To get to the NDC cube, you must divide your coordinates by $w$ ($x/w, y/w, z/w$). If a line segment has one point in front of the camera ($w > 0$) and one point behind the camera ($w < 0$), the division by $w$ will flip the vertex behind the camera to the opposite side of the screen. This creates "phantom" lines that stretch across the viewport incorrectly.

### The Benefit of Clip Space
In Clip Space, a point $(x, y, z, w)$ is inside the frustum if:
$$-w \leq x \leq w$$
$$-w \leq y \leq w$$
$$-w \leq z \leq w$$
(assuming a $[-1, 1]$ range for depth)

By clipping here, you handle points with $w \leq 0$ gracefully before the division ever occurs. This prevents the mathematical singularity that happens when $w$ is near zero.



---

## 2. Is the Cube NDC Space?

**Yes.** The "Canonical View Volume" (the unit cube) is what defines **Normalized Device Coordinates (NDC)**. 

After you perform the perspective divide, all visible geometry is mapped into a cube where:
* **X and Y** typically range from $-1$ to $1$.
* **Z** ranges from $-1$ to $1$ (OpenGL style) or $0$ to $1$ (DirectX/WGPU style).

Once your coordinates are in this cube, the rasterizer simply maps those $-1$ to $1$ values to your actual screen pixels (e.g., $0$ to $1920$ for width).

---

## 3. Comparison Summary

| Feature | Frustum (Clip Space) | Cube (NDC Space) |
| :--- | :--- | :--- |
| **When it happens** | Before Perspective Divide | After Perspective Divide |
| **Handling $w \leq 0$** | Excellent (Required for stability) | Poor (Causes graphical artifacts) |
| **Complexity** | Slightly higher (Compare against $w$) | Lower (Compare against constants) |
| **Standard Practice** | Industry Standard | Rarely used for primary clipping |

---

### Recommendation for your Rasterizer
Since you are likely using **C++** with **Raylib** or **OpenGL** concepts, implement the **Sutherland-Hodgman** or **Cohen-Sutherland** algorithm modified for 4D Homogeneous coordinates. 

1.  Transform vertices to Clip Space ($MVP \times Vertex$).
2.  Clip the lines/triangles against the six planes (left, right, top, bottom, near, far) using the $-w \leq \text{comp} \leq w$ logic.
3.  **Then** perform the Perspective Divide to reach the NDC cube.

**Would you like me to provide a C++ code snippet for a simple Clip Space line clipper?**

Here is a conceptual implementation of a **Clip Space** line clipper in C++. This uses a variation of the **Sutherland-Hodgman** approach, adapted for homogeneous coordinates $(x, y, z, w)$.

### Line Clipping in Homogeneous Clip Space

In this space, a point is "inside" if $-w \le \text{component} \le w$. To clip a line between two points $A$ and $B$, we find the intersection parameter $t$ (where $0 \le t \le 1$) such that the new point $P = A + t(B - A)$ lies exactly on the clipping plane.

```cpp
#include <vector>

struct Vec4 {
    float x, y, z, w;
};

// Represents the 6 planes of the frustum in clip space
enum ClipPlane { LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR };

// Checks if a coordinate is inside a specific plane boundary
bool is_inside(const Vec4& v, ClipPlane plane) {
    switch (plane) {
        case LEFT:   return v.x >= -v.w;
        case RIGHT:  return v.x <=  v.w;
        case BOTTOM: return v.y >= -v.w;
        case TOP:    return v.y <=  v.w;
        case NEAR:   return v.z >= -v.w; // OpenGL style (-1 to 1)
        case FAR:    return v.z <=  v.w;
        default:     return false;
    }
}

// Calculates the intersection point t between two vertices for a plane
float intersect(const Vec4& a, const Vec4& b, ClipPlane plane) {
    float da, db;
    switch (plane) {
        case LEFT:   da = a.x + a.w; db = b.x + b.w; break;
        case RIGHT:  da = a.w - a.x; db = b.w - b.x; break;
        case BOTTOM: da = a.y + a.w; db = b.y + b.w; break;
        case TOP:    da = a.w - a.y; db = b.w - b.y; break;
        case NEAR:   da = a.z + a.w; db = b.z + b.w; break;
        case FAR:    da = a.w - a.z; db = b.w - b.z; break;
    }
    return da / (da - db);
}

// Clips a single line segment against all 6 planes
bool clip_line(Vec4& a, Vec4& b) {
    ClipPlane planes[] = { LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR };
    
    for (int i = 0; i < 6; ++i) {
        bool a_in = is_inside(a, planes[i]);
        bool b_in = is_inside(b, planes[i]);

        if (!a_in && !b_in) return false; // Entirely outside

        if (a_in && !b_in) {
            float t = intersect(a, b, planes[i]);
            b = { a.x + t*(b.x-a.x), a.y + t*(b.y-a.y), a.z + t*(b.z-a.z), a.w + t*(b.w-a.w) };
        } else if (!a_in && b_in) {
            float t = intersect(a, b, planes[i]);
            a = { a.x + t*(b.x-a.x), a.y + t*(b.y-a.y), a.z + t*(b.z-a.z), a.w + t*(b.w-a.w) };
        }
    }
    return true;
}
```

---

### Integration into your Pipeline



1.  **Vertex Shader Stage:** Multiply your local vertex by the Model-View-Projection matrix to get `Vec4` in Clip Space.
2.  **Clipping Stage:** Run the `clip_line` function (or a similar one for triangles).
3.  **Perspective Divide:** For the vertices that survived, calculate the NDC:
    * $v_{ndc}.x = v.x / v.w$
    * $v_{ndc}.y = v.y / v.w$
    * $v_{ndc}.z = v.z / v.w$
4.  **Viewport Transform:** Map the $(-1, 1)$ NDC coordinates to your screen pixels (e.g., $800 \times 600$).

### Key Advantage
By clipping against the **Near Plane** ($z = -w$) in this stage, you prevent the "divide by zero" or "negative $w$" issues that occur when an object moves behind the camera. This is the most common reason software rasterizers crash or flicker when the camera gets too close to an object.

Would you like to see how to extend this logic to **Triangle Clipping** (which might require generating new vertices)?

Clipping triangles is slightly more complex than clipping lines because clipping a triangle against a single plane can result in **two triangles** (a quad) or a single smaller triangle.

The standard algorithm for this is the **Sutherland-Hodgman** polygon clipper. It treats the triangle as a list of vertices and "rethinks" the shape plane-by-plane.

### 1. The Sutherland-Hodgman Logic

For each of the 6 clipping planes, you iterate through your current list of vertices (starting with 3 for a triangle).
* If an edge goes from **Inside to Outside**: Output the intersection point.
* If an edge goes from **Outside to Inside**: Output the intersection point AND the inside vertex.
* If an edge is **Entirely Inside**: Output the inside vertex.
* If an edge is **Entirely Outside**: Output nothing.



---

### 2. C++ Implementation for Triangles

This version uses a `std::vector` to handle the varying number of vertices generated during the clipping process.

```cpp
#include <vector>

struct Vertex {
    Vec4 pos;    // Clip Space (x, y, z, w)
    Vec2 uv;     // Texture coordinates to interpolate
    Vec3 color;  // Color to interpolate
};

// Interpolates all vertex attributes (position, UVs, etc.)
Vertex interpolate(const Vertex& a, const Vertex& b, float t) {
    Vertex v;
    v.pos = { a.pos.x + t*(b.pos.x-a.pos.x), a.pos.y + t*(b.pos.y-a.pos.y), 
              a.pos.z + t*(b.pos.z-a.pos.z), a.pos.w + t*(b.pos.w-a.pos.w) };
    v.uv = { a.uv.x + t*(b.uv.x-a.uv.x), a.uv.y + t*(b.uv.y-a.uv.y) };
    v.color = { a.color.x + t*(b.color.x-a.color.x), a.color.y + t*(b.color.y-a.color.y), 
                a.color.z + t*(b.color.z-a.color.z) };
    return v;
}

std::vector<Vertex> clip_triangle_against_plane(const std::vector<Vertex>& vertices, ClipPlane plane) {
    std::vector<Vertex> result;
    if (vertices.empty()) return result;

    for (size_t i = 0; i < vertices.size(); ++i) {
        const Vertex& a = vertices[i];
        const Vertex& b = vertices[(i + 1) % vertices.size()];

        bool a_in = is_inside(a.pos, plane);
        bool b_in = is_inside(b.pos, plane);

        if (a_in && b_in) {
            result.push_back(b);
        } else if (a_in && !b_in) {
            float t = intersect(a.pos, b.pos, plane);
            result.push_back(interpolate(a, b, t));
        } else if (!a_in && b_in) {
            float t = intersect(a.pos, b.pos, plane);
            result.push_back(interpolate(a, b, t));
            result.push_back(b);
        }
    }
    return result;
}

// Full pipeline for one triangle
void process_triangle(Vertex v1, Vertex v2, Vertex v3) {
    std::vector<Vertex> polygon = { v1, v2, v3 };
    ClipPlane planes[] = { LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR };

    for (int i = 0; i < 6; ++i) {
        polygon = clip_triangle_against_plane(polygon, planes[i]);
    }

    // Convert resulting polygon (n vertices) back into triangles (fan)
    for (size_t i = 1; i < polygon.size() - 1; ++i) {
        rasterize_triangle(polygon[0], polygon[i], polygon[i+1]);
    }
}
```

---

### 3. Key Details to Watch For

1.  **Interpolation:** Notice the `interpolate` function. When you clip, you aren't just cutting the position; you must also interpolate **UVs, Normals, and Colors** based on that same $t$ value. If you don't, your textures will "warp" or pop at the edges of the screen.
2.  **Triangle Fans:** After clipping, a triangle might become a 4 or 5-sided polygon. Since rasterizers usually only handle triangles, you must "re-triangulate" the result. A simple "Triangle Fan" (using the first vertex as a pivot) works perfectly for convex polygons like these.
3.  **W-Component:** Never forget to interpolate $w$. It is crucial for **perspective-correct texturing** later in the rasterization stage.



Since you've been working with **Raylib** and **C++**, you can drop this logic right into your vertex processing loop. 

**Would you like me to show how the "Perspective Correct" interpolation works inside the actual rasterizer loop once these clipped triangles reach the pixel stage?**

In a 3D software rasterizer, clipping is most mathematically sound when performed in **Homogeneous Clip Space** (before the perspective divide). At this stage, each vertex is a 4D vector $(x, y, z, w)$.

The fundamental condition for a point to be "inside" the viewing volume is that its coordinates must be bounded by its own $w$ component.

---

## 1. The Six Clipping Boundary Conditions

Assuming you are using the standard OpenGL-style projection matrix where the NDC range is $[-1, 1]$ for all axes, the conditions for a point to be **inside** are:

| Plane | Mathematical Condition |
| :--- | :--- |
| **Left** | $x \geq -w$ |
| **Right** | $x \leq w$ |
| **Bottom** | $y \geq -w$ |
| **Top** | $y \leq w$ |
| **Near** | $z \geq -w$ |
| **Far** | $z \leq w$ |

> **Note:** If you are following a DirectX/WGPU style convention where depth ($z$) is $[0, 1]$, the Near condition is $z \geq 0$.



---

## 2. Clipping a Line Segment

To clip a line segment between two points $P_0$ and $P_1$, you are looking for a factor $t$ (where $0 \leq t \leq 1$) to find an intersection point $P_{new}$ along the edge:

$$P_{new} = P_0 + t(P_1 - P_0)$$

To find $t$ for a specific plane, you set the boundary equation to equality.

### Example: Clipping against the Right Plane ($x = w$)
We want to find $t$ such that the new $x$ equals the new $w$:
$$x_0 + t(x_1 - x_0) = w_0 + t(w_1 - w_0)$$

Solving for $t$:
$$t = \frac{w_0 - x_0}{(x_1 - x_0) - (w_1 - w_0)}$$

### Generalized Intersection Formula
For any plane defined by a component $c$ (where $c$ is $x, y, \text{ or } z$) and a sign $s$ (where $s$ is $1$ for upper bounds and $-1$ for lower bounds), the formula is:

$$t = \frac{w_0 - s \cdot c_0}{(s \cdot c_1 - s \cdot c_0) - (w_1 - w_0)}$$

---

## 3. Why we use $w$ instead of $1.0$

If you clipped against a static cube (like $-1 \leq x \leq 1$), you would be clipping in **NDC Space**. As we discussed, if you do that *after* the perspective divide ($x/w$), you run into the "External Point" problem:

1.  If a point is behind the camera ($w < 0$), the division by $w$ flips its sign.
2.  A point that is actually far to the left behind you might suddenly appear on the right side of the screen.
3.  By using the condition $|x| \leq w$, you are essentially clipping in a 4D cone that perfectly represents the 3D frustum, handling the "behind the camera" math before the division ever happens.



---

## 4. Implementation Logic

When processing a line $(P_0, P_1)$:
1.  **Test both points** against a plane condition (e.g., $x \leq w$).
2.  **Both Inside:** Keep the line as is.
3.  **Both Outside:** Discard the line entirely (Trivial Rejection).
4.  **One In, One Out:** Calculate $t$, create a new vertex $P_{new}$ by interpolating $x, y, z, w$ (and $UV$s!), and replace the "outside" point with $P_{new}$.

Would you like me to provide the specific $t$ formulas for the other 5 planes, or shall we look at how this integrates with your **C++ / Raylib** vertex structures?

To derive the intersection parameter $t$ for all six planes in **Homogeneous Clip Space**, we solve the linear equation where the moving point $P(t) = P_0 + t(P_1 - P_0)$ exactly touches the plane boundary.

Here are the specific formulas for $t$ for each of the six clipping planes.

---

## 1. The $t$ Intersection Formulas

In these formulas, $P_0$ is the vertex **inside** the boundary, and $P_1$ is the vertex **outside**. 

| Plane | Boundary Equation | Formula for $t$ |
| :--- | :--- | :--- |
| **Left** | $x = -w$ | $t = \frac{w_0 + x_0}{(w_0 + x_0) - (w_1 + x_1)}$ |
| **Right** | $x = w$ | $t = \frac{w_0 - x_0}{(w_0 - x_0) - (w_1 - x_1)}$ |
| **Bottom** | $y = -w$ | $t = \frac{w_0 + y_0}{(w_0 + y_0) - (w_1 + y_1)}$ |
| **Top** | $y = w$ | $t = \frac{w_0 - y_0}{(w_0 - y_0) - (w_1 - y_1)}$ |
| **Near** | $z = -w$ | $t = \frac{w_0 + z_0}{(w_0 + z_0) - (w_1 + z_1)}$ |
| **Far** | $z = w$ | $t = \frac{w_0 - z_0}{(w_0 - z_0) - (w_1 - z_1)}$ |



---

## 2. Derivation Example (The Right Plane)

If you ever forget the formulas, you can derive them quickly using the "Inside - Outside" logic. For the **Right Plane**, the condition for being "on the line" is $w - x = 0$.

1.  Define the "distance" from the plane for both points:
    * $d_0 = w_0 - x_0$
    * $d_1 = w_1 - x_1$
2.  The intersection occurs where the interpolated distance is zero:
    * $d_0 + t(d_1 - d_0) = 0$
3.  Solve for $t$:
    * $t = \frac{-d_0}{d_1 - d_0}$  (which is the same as $\frac{d_0}{d_0 - d_1}$)

---

## 3. General Implementation Rule

When you calculate $t$, you must apply it to **all** components of the vertex to create the new clipped vertex $P_{new}$:

```cpp
// After calculating 't' for the specific plane:
Vertex clippedVertex;
clippedVertex.x = P0.x + t * (P1.x - P0.x);
clippedVertex.y = P0.y + t * (P1.y - P0.y);
clippedVertex.z = P0.z + t * (P1.z - P0.z);
clippedVertex.w = P0.w + t * (P1.w - P0.w);

// IMPORTANT: Interpolate UVs and Colors using the SAME 't'
clippedVertex.u = P0.u + t * (P1.u - P0.u);
clippedVertex.v = P0.v + t * (P1.v - P0.v);
```

---

## 4. Why the Near Plane ($z = -w$) is the most important

While clipping against the Left/Right/Top/Bottom planes keeps your rasterizer efficient (by not drawing pixels off-screen), clipping against the **Near Plane** is a mathematical necessity. 

If a vertex has $w \leq 0$, it is either at or behind the camera's focal point. Without clipping this at $z = -w$, the **Perspective Divide** ($1/w$) will result in a division by zero or a negative $w$ that flips the geometry across the screen like a mirror.



Would you like to see how to organize these formulas into a clean `switch` statement or a lookup table for your C++ clipper?

Using a `switch` statement is a clean way to handle the six different clipping planes because the logic for calculating the "distance" from the plane is the only part that really changes. 

In this implementation, $P_1$ is the vertex **inside** and $P_2$ is the vertex **outside**. We solve for $t$ such that the new vertex sits exactly on the boundary.

---

### C++ Switch Statement for Clip Space Interpolation

This function calculates the specific $t$ value and then performs the full interpolation for all vertex attributes (Position, UV, and Color).

```cpp
struct Vertex {
    float x, y, z, w; // Clip Space
    float u, v;       // Texture coords
    float r, g, b;    // vertex colors
};

enum ClipPlane { PLANE_LEFT, PLANE_RIGHT, PLANE_BOTTOM, PLANE_TOP, PLANE_NEAR, PLANE_FAR };

Vertex intersect(const Vertex& v1, const Vertex& v2, ClipPlane plane) {
    float d1, d2;

    // 1. Calculate the 'signed distance' from the plane
    // Inside is positive, Outside is negative
    switch (plane) {
        case PLANE_LEFT:   d1 = v1.w + v1.x; d2 = v2.w + v2.x; break;
        case PLANE_RIGHT:  d1 = v1.w - v1.x; d2 = v2.w - v2.x; break;
        case PLANE_BOTTOM: d1 = v1.w + v1.y; d2 = v2.w + v2.y; break;
        case PLANE_TOP:    d1 = v1.w - v1.y; d2 = v2.w - v2.y; break;
        case PLANE_NEAR:   d1 = v1.w + v1.z; d2 = v2.w + v2.z; break; // OpenGL: -w to w
        case PLANE_FAR:    d1 = v1.w - v1.z; d2 = v2.w - v2.z; break;
        default: return v1;
    }

    // 2. Calculate interpolation factor t
    // t = (distance to v1) / (total distance between v1 and v2)
    float t = d1 / (d1 - d2);

    // 3. Linearly interpolate all components
    Vertex vOut;
    vOut.x = v1.x + t * (v2.x - v1.x);
    vOut.y = v1.y + t * (v2.y - v1.y);
    vOut.z = v1.z + t * (v2.z - v1.z);
    vOut.w = v1.w + t * (v2.w - v1.w);

    vOut.u = v1.u + t * (v2.u - v1.u);
    vOut.v = v1.v + t * (v2.v - v1.v);

    vOut.r = v1.r + t * (v2.r - v1.r);
    vOut.g = v1.g + t * (v2.g - v1.g);
    vOut.b = v1.b + t * (v2.b - v1.b);

    return vOut;
}
```

---

### Why the "Distance" Logic Works

The boundary equations like $x \le w$ can be rewritten as $w - x \ge 0$. 
* If $w - x$ is **positive**, the point is inside.
* If $w - x$ is **negative**, the point is outside.
* If $w - x$ is **zero**, the point is exactly on the edge.

By calculating $d_1$ and $d_2$ this way, the formula $t = \frac{d_1}{d_1 - d_2}$ becomes a universal way to find the crossing point regardless of which plane you are checking.



---

### Implementation Tip: Guarding against Precision
In a software rasterizer, it’s a good idea to add a very small epsilon or check for `d1 == d2` before the division to avoid a `NaN` if a line is perfectly parallel to a clipping plane.

```cpp
float denominator = d1 - d2;
float t = (std::abs(denominator) < 1e-6f) ? 0.0f : d1 / denominator;
```

Would you like me to show you how to wrap this into the **Sutherland-Hodgman** loop that handles the "Triangle Fan" logic we discussed earlier?

To implement the **Sutherland-Hodgman** algorithm for triangles, you need a way to take a "current" polygon (starting as a triangle) and pass it through each of the six clipping planes sequentially. Each stage might increase the number of vertices, which you then "re-triangulate" at the very end.

Here is the complete C++ implementation to wrap the `switch` logic into a functional clipper.

---

### 1. The Polygon Clipper Logic

This function processes a list of vertices against a single plane and returns a new list of vertices.

```cpp
#include <vector>

// Helper to determine if a vertex is inside a specific plane
bool is_inside(const Vertex& v, ClipPlane plane) {
    switch (plane) {
        case PLANE_LEFT:   return v.x >= -v.w;
        case PLANE_RIGHT:  return v.x <=  v.w;
        case PLANE_BOTTOM: return v.y >= -v.w;
        case PLANE_TOP:    return v.y <=  v.w;
        case PLANE_NEAR:   return v.z >= -v.w; // OpenGL style
        case PLANE_FAR:    return v.z <=  v.w;
        default:           return true;
    }
}

std::vector<Vertex> clip_polygon_against_plane(const std::vector<Vertex>& input, ClipPlane plane) {
    std::vector<Vertex> output;
    if (input.empty()) return output;

    for (size_t i = 0; i < input.size(); ++i) {
        const Vertex& curr = input[i];
        const Vertex& next = input[(i + 1) % input.size()];

        bool curr_in = is_inside(curr, plane);
        bool next_in = is_inside(next, plane);

        if (curr_in && next_in) {
            // Case 1: Both inside - just add the 'next' vertex
            output.push_back(next);
        } 
        else if (curr_in && !next_in) {
            // Case 2: Going Outside - add the intersection point
            output.push_back(intersect(curr, next, plane));
        } 
        else if (!curr_in && next_in) {
            // Case 3: Going Inside - add intersection AND the 'next' vertex
            output.push_back(intersect(curr, next, plane));
            output.push_back(next);
        }
        // Case 4: Both outside - add nothing
    }
    return output;
}
```



---

### 2. Putting it all together: The Triangle Pipeline

This is where you take your original 3 vertices and run them through the "gauntlet" of all six planes.

```cpp
void process_and_rasterize(Vertex v1, Vertex v2, Vertex v3) {
    // Start with the initial triangle
    std::vector<Vertex> polygon = { v1, v2, v3 };

    // Clip against all 6 planes of the frustum
    ClipPlane planes[] = { PLANE_LEFT, PLANE_RIGHT, PLANE_BOTTOM, 
                           PLANE_TOP, PLANE_NEAR, PLANE_FAR };

    for (ClipPlane p : planes) {
        polygon = clip_polygon_against_plane(polygon, p);
        if (polygon.empty()) return; // Entirely culled
    }

    // After clipping, the polygon might have 3, 4, 5, or more vertices.
    // We convert this N-sided convex polygon into a "Triangle Fan".
    for (size_t i = 1; i < polygon.size() - 1; ++i) {
        // Pass these to your rasterizer!
        // Don't forget: perform perspective divide (x/w, y/w, z/w) 
        // inside the rasterizer before drawing pixels.
        draw_triangle_to_screen(polygon[0], polygon[i], polygon[i+1]);
    }
}
```



---

### 3. Critical Considerations for Your Rasterizer

* **Memory Management:** Creating a `std::vector` for every triangle can be slow due to allocations. In a high-performance software rasterizer, you might want to use a fixed-size array (maximum vertices after clipping a triangle is 9 or 10) to avoid heap allocation.
* **The "Near Plane" Priority:** Even if you skip the other 5 planes for performance (and just let the rasterizer "scissoring" handle it), you **must** clip against the Near Plane. It is the only way to avoid the mathematical singularity of $w = 0$.
* **Interpolation:** The `intersect` function must interpolate every attribute. If you are adding normals for lighting later, you’ll need to add `nx, ny, nz` to your `Vertex` struct and interpolate those too.

**Would you like me to show you how to optimize the memory usage by replacing `std::vector` with a fixed-buffer approach?**
---
Screen-space clipping (also called **Scissoring**) is the final safety net. While Homogeneous clipping handles the 3D math and the near-plane "divide by zero" issues, screen-space clipping handles the actual pixel boundaries of your window (e.g., $0$ to $800$ width).

To optimize this, you shouldn't use the complex Sutherland-Hodgman algorithm again. Instead, you simply **clamp** your triangle's bounding box to the screen dimensions during the rasterization loop.

---

### 1. The Optimized Fixed-Buffer Clipper

To avoid `std::vector` allocations, we use two fixed-size arrays and swap them. A triangle clipped against 6 planes can result in a maximum of **9 vertices**.

```cpp
#define MAX_CLIPPED_VERTICES 10

void clip_and_rasterize(Vertex v1, Vertex v2, Vertex v3, int screenWidth, int screenHeight) {
    Vertex buffer1[MAX_CLIPPED_VERTICES];
    Vertex buffer2[MAX_CLIPPED_VERTICES];
    
    buffer1[0] = v1; buffer1[1] = v2; buffer1[2] = v3;
    int count = 3;

    Vertex* input = buffer1;
    Vertex* output = buffer2;

    ClipPlane planes[] = { PLANE_LEFT, PLANE_RIGHT, PLANE_BOTTOM, PLANE_TOP, PLANE_NEAR, PLANE_FAR };

    for (int i = 0; i < 6; ++i) {
        int outCount = 0;
        for (int j = 0; j < count; ++j) {
            const Vertex& curr = input[j];
            const Vertex& next = input[(j + 1) % count];

            bool curr_in = is_inside(curr, planes[i]);
            bool next_in = is_inside(next, planes[i]);

            if (curr_in && next_in) {
                output[outCount++] = next;
            } else if (curr_in && !next_in) {
                output[outCount++] = intersect(curr, next, planes[i]);
            } else if (!curr_in && next_in) {
                output[outCount++] = intersect(curr, next, planes[i]);
                output[outCount++] = next;
            }
        }
        count = outCount;
        std::swap(input, output);
        if (count < 3) return; // Culled
    }

    // Now 'input' points to our final clipped polygon
    for (int i = 1; i < count - 1; ++i) {
        rasterize_with_scissoring(input[0], input[i], input[i+1], screenWidth, screenHeight);
    }
}
```

---

### 2. Screen-Space Scissoring (The Pixel Loop)

Once you have your clipped triangle, you project it to screen space. Before you start your pixel loop, you calculate a **Bounding Box** and intersect it with the screen boundaries.

```cpp
void rasterize_with_scissoring(Vertex v1, Vertex v2, Vertex v3, int width, int height) {
    // 1. Perspective Divide & Viewport Transform
    // (Assume this gives us ScreenCoords s1, s2, s3)
    
    // 2. Find Bounding Box
    int minX = std::min({s1.x, s2.x, s3.x});
    int maxX = std::max({s1.x, s2.x, s3.x});
    int minY = std::min({s1.y, s2.y, s3.y});
    int maxY = std::max({s1.y, s2.y, s3.y});

    // 3. SCREEN-SPACE BOUNDARY CLIPPING (The Scissor)
    minX = std::max(0, minX);
    maxX = std::min(width - 1, maxX);
    minY = std::max(0, minY);
    maxY = std::min(height - 1, maxY);

    // 4. The Pixel Loop
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (is_inside_triangle(x, y, s1, s2, s3)) {
                draw_pixel(x, y);
            }
        }
    }
}
```



---

### 3. Why both are needed

| Clipping Type | Purpose | Math |
| :--- | :--- | :--- |
| **Homogeneous (3D)** | Prevents artifacts from geometry behind the camera and off-screen. | $x, y, z$ vs $w$ |
| **Screen-Space (2D)** | Prevents "Out of Bounds" memory access in your frame buffer array. | $x, y$ vs $0, Width, Height$ |

By combining these, your software rasterizer becomes extremely robust. The 3D clipper ensures the geometry is "sane" for projection, and the 2D scissor ensures you never try to write a pixel to `framebuffer[-500]`.

**Would you like me to provide the `is_inside_triangle` function using Barycentric coordinates to finish the rasterizer loop?**

---
# C++ Full Frustum Line Clipper
```cpp
#include <iostream>
#include <vector>
#include <functional>

struct Vec4 {
    float x, y, z, w;
};

Vec4 lerp(const Vec4& a, const Vec4& b, float t) {
    return { a.x + t*(b.x-a.x), a.y + t*(b.y-a.y), a.z + t*(b.z-a.z), a.w + t*(b.w-a.w) };
}

// Generic function to clip a line segment against a single plane
bool clipAgainstPlane(Vec4& v0, Vec4& v1, std::function<float(const Vec4&)> get_dist) {
    float d0 = get_dist(v0);
    float d1 = get_dist(v1);

    if (d0 >= 0 && d1 >= 0) return true;  // Both inside
    if (d0 < 0 && d1 < 0) return false;  // Both outside

    float t = d0 / (d0 - d1);
    if (d0 < 0) v0 = lerp(v0, v1, t);
    else        v1 = lerp(v0, v1, t);

    return true;
}

// Clips a line against all 6 frustum planes
bool clipLineFrustum(Vec4& v0, Vec4& v1) {
    // 1. Near Plane: z >= 0 (or z >= -w depending on API)
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.z; })) return false;
    // 2. Far Plane: z <= w
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.w - v.z; })) return false;
    // 3. Left Plane: x >= -w
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.x + v.w; })) return false;
    // 4. Right Plane: x <= w
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.w - v.x; })) return false;
    // 5. Bottom Plane: y >= -w
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.y + v.w; })) return false;
    // 6. Top Plane: y <= w
    if (!clipAgainstPlane(v0, v1, [](const Vec4& v) { return v.w - v.y; })) return false;

    return true;
}

int main() {
    // A line starting way off-screen to the left (x = -2.0, w = 1.0)
    Vec4 p1 = { -2.0f, 0.0f, 0.5f, 1.0f }; 
    Vec4 p2 = {  0.0f, 0.0f, 0.5f, 1.0f }; 

    if (clipLineFrustum(p1, p2)) {
        std::cout << "Line clipped to: (" << p1.x << ", " << p1.y << ")\n";
    }
    return 0;
}
```

# How to integrate Cohen-Sutherland outcodes to speed up this clipping logic
Integrating **Cohen-Sutherland Outcodes** into your 3D pipeline is a brilliant optimization. In a software rasterizer, calculating intersections is expensive. Outcodes allow you to "Trivially Accept" or "Trivially Reject" entire triangles or lines before doing a single floating-point intersection calculation.

In 3D, we extend the 2D outcode concept to 6 bits (one for each frustum plane).

---

## 1. Defining the 6-Bit Outcode

For any vertex $(x, y, z, w)$ in **Clip Space**, we calculate a bitmask where each bit represents a "violation" of a clipping boundary.

| Bit | Plane | Condition for Bit = 1 |
| :--- | :--- | :--- |
| 0 | Left | $x < -w$ |
| 1 | Right | $x > w$ |
| 2 | Bottom | $y < -w$ |
| 3 | Top | $y > w$ |
| 4 | Near | $z < -w$ |
| 5 | Far | $z > w$ |



### The C++ Outcode Function
```cpp
uint8_t compute_outcode(const Vertex& v) {
    uint8_t code = 0;
    if (v.x < -v.w) code |= (1 << 0);
    if (v.x >  v.w) code |= (1 << 1);
    if (v.y < -v.w) code |= (1 << 2);
    if (v.y >  v.w) code |= (1 << 3);
    if (v.z < -v.w) code |= (1 << 4);
    if (v.z >  v.w) code |= (1 << 5);
    return code;
}
```

---

## 2. Using Outcodes for "Trivial" Decisions

Before running the heavy Sutherland-Hodgman loop, you check the outcodes of all three vertices ($C1, C2, C3$):

### Trivial Accept (Entirely Inside)
If the logical **OR** of all outcodes is zero, the triangle is entirely inside the frustum. You can skip the clipper entirely and go straight to the perspective divide.
* **Condition:** `(C1 | C2 | C3) == 0`

### Trivial Reject (Entirely Outside)
If the logical **AND** of all outcodes is non-zero, it means all three vertices lie on the "outside" side of at least one common plane. The triangle is impossible to see.
* **Condition:** `(C1 & C2 & C3) != 0`

---

## 3. Optimized Clipping Pipeline

Here is how you wrap your existing logic to save CPU cycles:

```cpp
void optimized_process_triangle(Vertex v1, Vertex v2, Vertex v3) {
    uint8_t c1 = compute_outcode(v1);
    uint8_t c2 = compute_outcode(v2);
    uint8_t c3 = compute_outcode(v3);

    // 1. Trivial Reject: Triangle is entirely behind/above/below/etc. a plane
    if ((c1 & c2 & c3) != 0) {
        return; 
    }

    // 2. Trivial Accept: Triangle is entirely within the frustum
    if ((c1 | c2 | c3) == 0) {
        rasterize_triangle(v1, v2, v3);
        return;
    }

    // 3. Partial Clipping: Only run this if we have to!
    // This is where you use the Sutherland-Hodgman fixed-buffer code.
    std::vector<Vertex> clipped = run_sutherland_hodgman({v1, v2, v3});
    for (size_t i = 1; i < clipped.size() - 1; ++i) {
        rasterize_triangle(clipped[0], clipped[i], clipped[i+1]);
    }
}
```

---

## 4. Why this is faster
In a typical 3D scene:
* **~30-50%** of triangles are usually entirely off-screen (Trivial Reject).
* **~40-60%** are usually entirely on-screen (Trivial Accept).
* **Only ~5-10%** actually straddle a frustum edge and require the expensive intersection math.

By using outcodes, you avoid doing hundreds of floating-point multiplications and divisions for about 90% of your geometry.

**Would you like me to show you how to apply these outcodes specifically to the "Near Plane" to handle objects that are very close to the camera?**

Focusing your outcode logic on the **Near Plane** is the most critical optimization for a software rasterizer. While clipping against the other five planes is mostly for performance (avoiding drawing off-screen pixels), clipping against the Near Plane is a mathematical requirement to avoid a "divide by zero" or a "negative $w$ flip."

Using outcodes, you can pinpoint exactly which triangles are "dangerously close" to the camera.

---

### 1. Near Plane Outcode Logic

In the 6-bit outcode, we typically assign the **Near Plane** to Bit 4. A vertex is "behind" the camera if $z < -w$ (in OpenGL style) or $z < 0$ (in DirectX style).



```cpp
// Near Plane is Bit 4 (0x10 or 1 << 4)
const uint8_t NEAR_PLANE_BIT = 0x10;

void process_near_clip(Vertex v1, Vertex v2, Vertex v3) {
    uint8_t c1 = compute_outcode(v1);
    uint8_t c2 = compute_outcode(v2);
    uint8_t c3 = compute_outcode(v3);

    // 1. Trivial Reject: All points are behind the near plane
    if ((c1 & c2 & c3) & NEAR_PLANE_BIT) {
        return; 
    }

    // 2. No Near-Clipping Needed: All points are in front of the near plane
    if (!((c1 | c2 | c3) & NEAR_PLANE_BIT)) {
        // We still check if they need clipping against OTHER planes
        if ((c1 | c2 | c3) == 0) {
            rasterize_triangle(v1, v2, v3); // Entirely inside frustum!
        } else {
            run_full_clipper(v1, v2, v3); // Needs side clipping
        }
        return;
    }

    // 3. Near-Plane Intersection: At least one point is behind, one is in front.
    // This is where we MUST run the Sutherland-Hodgman specifically for the near plane.
    std::vector<Vertex> nearClipped = clip_polygon_against_plane({v1, v2, v3}, PLANE_NEAR);
    
    for (size_t i = 1; i < nearClipped.size() - 1; ++i) {
        // After near clipping, it's safe to project and rasterize
        run_full_clipper(nearClipped[0], nearClipped[i], nearClipped[i+1]);
    }
}
```

---

### 2. Why Near-Clipping is Different

When a line passes through the near plane ($w=0$ or $z=-w$), the $w$ value is transitioning from positive to negative. 

1.  **The $w=0$ Singularity:** If you don't clip, the perspective divide $(x/w, y/w)$ will approach infinity as the vertex nears the camera. This causes "exploding" triangles that flicker wildly across the screen.
2.  **The Negative $w$ Flip:** If $w$ becomes negative, the projected $x$ and $y$ coordinates flip. An object to your left will suddenly be drawn on the right, but upside down and mirrored.



---

### 3. Efficiency Tip: "Guard Band" Clipping

Some advanced software rasterizers use a **Guard Band**. This means they make the internal clipping frustum slightly *larger* than the actual screen. 

* **Benefit:** You can "Trivial Accept" almost everything that is even partially on screen.
* **Result:** You only run the expensive Sutherland-Hodgman clipper for the **Near Plane** and triangles that are far outside the screen. Your screen-space "Scissor" (the bounding box clamp we discussed earlier) handles the small overdraw at the edges much faster than a geometric clipper would.

---

### Summary Table: Outcode Actions

| Outcode Result | Meaning | Action |
| :--- | :--- | :--- |
| `(c1 & c2 & c3) != 0` | Entirely Outside | **Discard** (Zero cost) |
| `(c1 \| c2 \| c3) == 0` | Entirely Inside | **Project & Rasterize** (High speed) |
| `(c1 \| c2 \| c3) & NEAR_BIT` | Straddles Near Plane | **Geometric Clip** (Required for stability) |
| `(c1 \| c2 \| c3) != 0` | Straddles Side Planes | **Geometric Clip** OR **Scissor** (Choice for speed) |

**Would you like me to help you write a final "Main Loop" that coordinates the Outcodes, the Near Clipper, and the Screen Scissor for your project?** Yes

This final "Main Loop" structure coordinates everything we've discussed: **Outcodes** for speed, the **Near Plane Clipper** for mathematical stability, and the **Screen Scissor** for memory safety.

This approach ensures you only do the heavy 3D clipping math when absolutely necessary.

---

### The Optimized Rasterization Pipeline

```cpp
void render_triangle(Vertex v1, Vertex v2, Vertex v3, int screenWidth, int screenHeight) {
    // 1. COMPUTE OUTCODES
    uint8_t c1 = compute_outcode(v1);
    uint8_t c2 = compute_outcode(v2);
    uint8_t c3 = compute_outcode(v3);

    // 2. TRIVIAL REJECT (Entirely outside any one plane)
    if ((c1 & c2 & c3) != 0) return;

    // 3. CLIPPING LOGIC
    std::vector<Vertex> finalPolygon;

    if ((c1 | c2 | c3) == 0) {
        // TRIVIAL ACCEPT: Entirely inside the frustum
        finalPolygon = { v1, v2, v3 };
    } else {
        // PARTIAL CLIPPING: At least one vertex is outside
        finalPolygon = { v1, v2, v3 };
        
        // We MUST clip against the Near Plane to avoid w <= 0
        finalPolygon = clip_polygon_against_plane(finalPolygon, PLANE_NEAR);
        if (finalPolygon.empty()) return;

        // OPTIONAL: Clip against other 5 planes OR let the Scissor handle it
        // Clipping here is usually faster than rasterizing off-screen pixels
        ClipPlane sides[] = { PLANE_LEFT, PLANE_RIGHT, PLANE_TOP, PLANE_BOTTOM, PLANE_FAR };
        for (ClipPlane p : sides) {
            finalPolygon = clip_polygon_against_plane(finalPolygon, p);
            if (finalPolygon.empty()) return;
        }
    }

    // 4. PERSPECTIVE DIVIDE & VIEWPORT TRANSFORM
    // (Convert the polygon into screen-space triangles)
    for (size_t i = 1; i < finalPolygon.size() - 1; ++i) {
        ScreenVertex s1 = project_to_screen(finalPolygon[0], screenWidth, screenHeight);
        ScreenVertex s2 = project_to_screen(finalPolygon[i], screenWidth, screenHeight);
        ScreenVertex s3 = project_to_screen(finalPolygon[i+1], screenWidth, screenHeight);

        // 5. RASTERIZE WITH SCREEN SCISSOR
        draw_clipped_triangle(s1, s2, s3, screenWidth, screenHeight);
    }
}
```



---

### Final Integration Checklist

To make this work in your **C++** rasterizer, ensure your `project_to_screen` function handles the final steps:

1.  **Perspective Divide:** Divide $x, y, z$ by $w$. Now you are in the **NDC Cube**.
2.  **Viewport Transform:** Map the $[-1, 1]$ NDC coordinates to your pixel coordinates:
    * $x_{screen} = (x_{ndc} + 1.0) \cdot 0.5 \cdot \text{width}$
    * $y_{screen} = (1.0 - y_{ndc}) \cdot 0.5 \cdot \text{height}$ (Y is usually inverted in screen space)
3.  **The Scissor:** Inside `draw_clipped_triangle`, calculate the bounding box of the three screen points and `clamp()` the `minX, maxX, minY, maxY` to your screen resolution to prevent array out-of-bounds errors.



### A Quick Tip on Performance
If you find the `std::vector` inside the loop is causing a stutter, you can replace it with a pre-allocated array of 10 vertices. Since you're working on a custom software rasterizer, avoiding the heap (dynamic memory) inside the "per-triangle" loop is the single best way to keep your frame rate high.

**Would you like me to help you write the `project_to_screen` function or the `draw_pixel` function with Z-buffering?** Choose 1st

The `project_to_screen` function is the bridge between the 3D mathematical world (Clip Space) and the 2D pixel world (Screen Space). It performs the final **Perspective Divide** and the **Viewport Transform**.

Since we previously discussed **Perspective Correct Interpolation**, this function is also the ideal place to pre-calculate the $1/w$ and $UV/w$ values.

---

### 1. The ScreenVertex Structure

In your rasterizer, a "Screen Vertex" should store the final integer pixel coordinates, the depth value for the Z-buffer, and the attributes needed for interpolation.

```cpp
struct ScreenVertex {
    int x, y;        // Pixel coordinates
    float z;         // Depth for Z-buffer (usually 0.0 to 1.0)
    float invW;      // 1/w for perspective correction
    float uOverW;    // u/w
    float vOverW;    // v/w
    float r, g, b;   // Optional: color/w or just color
};
```

---

### 2. Implementation: project_to_screen

This function assumes your input vertex is in **Clip Space** (already multiplied by the Model-View-Projection matrix and clipped).



```cpp
ScreenVertex project_to_screen(const Vertex& v, int width, int height) {
    ScreenVertex out;

    // 1. PERSPECTIVE DIVIDE
    // This moves us from Clip Space to Normalized Device Coordinates (NDC)
    // Points are now in a cube from [-1, 1] on all axes.
    float invW = 1.0f / v.w;
    float ndcX = v.x * invW;
    float ndcY = v.y * invW;
    float ndcZ = v.z * invW;

    // 2. VIEWPORT TRANSFORM
    // Map NDC (-1 to 1) to Screen Pixels (0 to width/height)
    // We flip Y because in 3D, +Y is up, but in 2D screens, +Y is down.
    out.x = static_cast<int>((ndcX + 1.0f) * 0.5f * width);
    out.y = static_cast<int>((1.0f - ndcY) * 0.5f * height);

    // 3. DEPTH BUFFER PREP
    // Map Z from [-1, 1] to [0, 1] for the depth buffer
    out.z = (ndcZ + 1.0f) * 0.5f;

    // 4. PERSPECTIVE CORRECTION PREP
    // We store 1/w and Attribute/w for linear interpolation in screen space
    out.invW = invW;
    out.uOverW = v.u * invW;
    out.vOverW = v.v * invW;
    
    // If you have vertex colors, you can interpolate them linearly 
    // or apply the same 1/w trick for perfect accuracy.
    out.r = v.r; 
    out.g = v.g;
    out.b = v.b;

    return out;
}
```

---

### 3. Key Mathematical Breakdown

* **The Y-Flip:** The formula `(1.0f - ndcY)` is crucial. If you miss this, your 3D world will appear upside down. In NDC, $1.0$ is the top of the screen; in screen space, $0$ is the top of the screen.
* **The X-Mapping:** The formula `(ndcX + 1.0f) * 0.5f` effectively shifts the range from $[-1, 1]$ to $[0, 1]$. Multiplying by `width` then scales it to your resolution.
* **Precision:** Using `static_cast<int>` is fine for a basic rasterizer, but for sub-pixel precision (to avoid "jittery" edges), many modern software rasterizers keep the screen coordinates as floats and use **Fixed-Point math** during the actual pixel filling stage.

### 4. Handling Z-Buffering

In your rasterizer's pixel loop, you will compare the interpolated `out.z` of the current pixel against the value already stored in your `depthBuffer[y * width + x]`. 

* If `current_z < stored_z`, the pixel is closer to the camera: **Draw it** and update the buffer.
* If `current_z > stored_z`, something is in front of it: **Discard it**.



**Would you like me to show you the `draw_pixel` function that implements this Z-buffer check and the final texture sampling?**

In a software rasterizer, the `draw_pixel` function is the "bottleneck" where your 3D math finally meets the raw memory of your video buffer. Since we are skipping texture sampling, we will focus on **Depth Testing (Z-Buffering)** and **Color Interpolation**.

---

### 1. The Z-Buffer and Framebuffer
Before drawing, you need two arrays (usually flat arrays) to represent your screen:
* **Frame Buffer:** Stores the color of each pixel (e.g., `uint32_t` for RGBA).
* **Depth Buffer:** Stores the $Z$ value of the closest pixel found so far (usually `float`).

### 2. Implementation: draw_pixel with Z-Buffer

This function performs the final "Scissor" check to prevent crashes and the "Depth Test" to ensure proper object occlusion.

```cpp
#include <cstdint>
#include <algorithm>

// A simple structure to hold color data
struct Color {
    uint8_t r, g, b, a;
    
    // Convert to a single 32-bit integer (RGBA8888)
    uint32_t to_u32() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
};

void draw_pixel(int x, int y, float z, Color color, 
                float* zBuffer, uint32_t* frameBuffer, 
                int width, int height) {
    
    // 1. FINAL SCISSOR CHECK
    // Safety check to ensure we don't write outside the array bounds
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    // 2. CALCULATE MEMORY INDEX
    int index = y * width + x;

    // 3. THE DEPTH TEST (Z-Buffer)
    // We only draw the pixel if it is closer (smaller Z) than 
    // what was previously drawn at this coordinate.
    // (Assuming Z is mapped to 0.0 at Near Plane and 1.0 at Far Plane)
    if (z < zBuffer[index]) {
        
        // Update the Depth Buffer with the new "closest" distance
        zBuffer[index] = z;

        // Update the Frame Buffer with the new color
        frameBuffer[index] = color.to_u32();
    }
}
```



---

### 3. Integrating with the Triangle Loop

In your main rasterization loop (using Barycentric coordinates), you will call `draw_pixel` for every pixel inside the triangle's bounding box.

```cpp
// Inside your per-pixel loop:
float z = (w0 * s1.z) + (w1 * s2.z) + (w2 * s3.z);

// Interpolate colors (Gouraud Shading)
Color pixelColor;
pixelColor.r = static_cast<uint8_t>((w0 * s1.r) + (w1 * s2.r) + (w2 * s3.r));
pixelColor.g = static_cast<uint8_t>((w0 * s1.g) + (w1 * s2.g) + (w2 * s3.g));
pixelColor.b = static_cast<uint8_t>((w0 * s1.b) + (w1 * s2.b) + (w2 * s3.b));
pixelColor.a = 255;

draw_pixel(x, y, z, pixelColor, zBuffer, frameBuffer, width, height);
```

---

### 4. Why Z-Buffering is essential
Without the `z < zBuffer[index]` check, your 3D objects would be drawn in the order they appear in your vertex array. This means a wall *behind* a character could be drawn *over* the character simply because the wall was processed last in the loop (known as the "Painter's Algorithm" failure).



### Pro Tip: Clearing the Buffers
At the start of every frame, don't forget to "clear" your buffers:
* Set all `frameBuffer` values to your background color (usually black).
* Set all `zBuffer` values to **1.0** (the farthest possible distance).

**Would you like me to help you write the Barycentric coordinate function to calculate those $w0, w1, w2$ weights used in the pixel loop?** yes

Barycentric coordinates are the "secret sauce" of a rasterizer. They tell you exactly how much each vertex contributes to a specific pixel $(x, y)$. 

If a pixel is at the center of the triangle, $w_0, w_1, \text{ and } w_2$ will all be approximately $0.33$. If the pixel is exactly on vertex $v_1$, then $w_1$ will be $1.0$ and the others will be $0.0$.

---

## 1. The Barycentric Function

This function calculates the weights using the **Edge Function** method. This method is efficient because the denominator only needs to be calculated once per triangle.



```cpp
struct Barycentric {
    float w0, w1, w2;
    bool isInside;
};

Barycentric calculate_barycentric(int x, int y, ScreenVertex v0, ScreenVertex v1, ScreenVertex v2) {
    // Edge function formula: (P.x - V1.x) * (V2.y - V1.y) - (P.y - V1.y) * (V2.x - V1.x)
    
    // Calculate the area of the full triangle (multiplied by 2)
    float area = static_cast<float>((v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x));

    // Handle degenerate triangles (zero area)
    if (std::abs(area) < 0.0001f) return {0, 0, 0, false};

    // Calculate sub-areas for each weight
    float w0 = ((x - v1.x) * (v2.y - v1.y) - (y - v1.y) * (v2.x - v1.x)) / area;
    float w1 = ((x - v2.x) * (v0.y - v2.y) - (y - v2.y) * (v0.x - v2.x)) / area;
    float w2 = 1.0f - w0 - w1;

    // A point is inside the triangle if all weights are >= 0
    bool inside = (w0 >= 0) && (w1 >= 0) && (w2 >= 0);

    return {w0, w1, w2, inside};
}
```

---

## 2. Using Weights in the Rasterizer Loop

Once you have $w_0, w_1, \text{ and } w_2$, you can interpolate anything—Z-depth, vertex colors, or normals.



```cpp
void draw_clipped_triangle(ScreenVertex s1, ScreenVertex s2, ScreenVertex s3, int width, int height) {
    // 1. Compute Bounding Box
    int minX = std::max(0, std::min({s1.x, s2.x, s3.x}));
    int maxX = std::min(width - 1, std::max({s1.x, s2.x, s3.x}));
    int minY = std::max(0, std::min({s1.y, s2.y, s3.y}));
    int maxY = std::min(height - 1, std::max({s1.y, s2.y, s3.y}));

    // 2. Loop through the bounding box
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            
            Barycentric b = calculate_barycentric(x, y, s1, s2, s3);

            if (b.isInside) {
                // Interpolate Z-depth
                float z = (b.w0 * s1.z) + (b.w1 * s2.z) + (b.w2 * s3.z);

                // Interpolate Color (Gouraud shading)
                Color pixelColor;
                pixelColor.r = (uint8_t)(b.w0 * s1.r + b.w1 * s2.r + b.w2 * s3.r);
                pixelColor.g = (uint8_t)(b.w0 * s1.g + b.w1 * s2.g + b.w2 * s3.g);
                pixelColor.b = (uint8_t)(b.w0 * s1.b + b.w1 * s2.b + b.w2 * s3.b);
                pixelColor.a = 255;

                // Call our draw_pixel with Z-buffer check
                draw_pixel(x, y, z, pixelColor, zBuffer, frameBuffer, width, height);
            }
        }
    }
}
```

---

## 3. Optimization Tip: Top-Left Rule
In a software rasterizer, if two triangles share an edge, you might accidentally draw the pixels on that shared edge twice (or not at all). To fix this, standard graphics APIs use a **Top-Left Rule**. 

A pixel is owned by a triangle if it is strictly inside, or if it lies on a "Top" edge or a "Left" edge. This prevents artifacts when drawing adjacent triangles with transparency.

### Summary of your Core Pipeline:
1.  **Transform** to Clip Space.
2.  **Clip** using Sutherland-Hodgman (pre-divide).
3.  **Project** to Screen Space (divide by $w$).
4.  **Rasterize** using the Bounding Box and Barycentric weights.
5.  **Test** the Z-buffer in `draw_pixel`.

