

Would you like to see how to implement **Simple Flat Shading** using that Normal calculation we discussed, to make the cube look like it has solid, lit faces rather than just rainbow gradients?

To implement **Flat Shading**, we shift from interpolating colors across the face to calculating a single color for the entire triangle based on its orientation relative to a light source.

This gives the cube a much more "solid" look, where each face has a consistent brightness based on how it catches the light.

### 1. Update the Vertex and Normal Logic

For Flat Shading, we need the **Face Normal**. You calculate this using the original 3D coordinates of the vertices (World Space) before they are projected to the screen.

```cpp
// Helper to calculate the face normal of a triangle
Vector3 GetTriangleNormal(Vector3 v0, Vector3 v1, Vector3 v2) {
    Vector3 edge1 = Vector3Subtract(v1, v0);
    Vector3 edge2 = Vector3Subtract(v2, v0);
    return Vector3Normalize(Vector3CrossProduct(edge1, edge2));
}

```

### 2. Modifying the Pipeline for Lighting

We will add a "Light Direction" to our logic. We calculate the shade once per triangle and pass that color into our rasterizer.

```cpp
void MyDrawIndexed(SoftRenderer *sr, Vertex *vertices, int vCount, int *indices, int iCount, bool wireframe) {
    // ... STAGE 1 (Vertex Shading) remains same ...

    Vector3 lightDir = Vector3Normalize((Vector3){ 0.5f, 1.0f, 0.5f }); // Light from top-right

    for (int i = 0; i < iCount; i += 3) {
        int i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];
        TransformedVertex tv0 = cache[i0], tv1 = cache[i1], tv2 = cache[i2];
        
        if (tv0.clipped || tv1.clipped || tv2.clipped) continue;

        // --- FLAT SHADING CALCULATION ---
        // Use original 3D positions for the normal
        Vector3 n = GetTriangleNormal(vertices[i0].position, vertices[i1].position, vertices[i2].position);
        
        // Lambertian factor: dot product of normal and light direction
        float dot = Vector3DotProduct(n, lightDir);
        float intensity = fmaxf(0.2f, dot); // 0.2 is ambient light (the "minimum" brightness)

        Color faceColor = {
            (unsigned char)(200 * intensity), // Base color Gray-ish
            (unsigned char)(200 * intensity),
            (unsigned char)(250 * intensity), // Slight blue tint
            255
        };

        if (wireframe) {
            MyDrawLine(&sr->colorBuffer, tv0.screenPos.x, tv0.screenPos.y, tv1.screenPos.x, tv1.screenPos.y, GREEN);
            // ... (rest of wireframe lines)
        } else {
            // Pass 'faceColor' into your rasterizer loop instead of interpolating vertex colors
            RasterizeTriangle(sr, tv0, tv1, tv2, faceColor); 
        }
    }
}

```

---

### 3. The Visual Result

* **Without Lighting:** The cube looks like a flat 2D silhouette if all faces are the same color.
* **With Flat Shading:** As the cube rotates, the faces will brighten and dim dynamically. This provides the visual depth needed to perceive the 3D shape.

### Key Implementation Note: Backface Culling

Since you are now calculating the **Normal** ($N$), you should also implement **Backface Culling**. If the normal is pointing away from the camera, you can `continue` and skip the triangle entirely.

```cpp
// In a simple orthographic/forward view:
if (n.z < 0) continue; // Skip triangles facing away from the screen

```

### Summary of the Software Pipeline

You now have a system that mimics the core stages of OpenGL:

1. **Buffer Management:** Raylib `Image` as your Framebuffer.
2. **Input Assembly:** Indexed vertex arrays.
3. **Vertex Processing:** MVP matrix transformation and caching.
4. **Primitive Assembly:** Face normal and lighting calculation.
5. **Rasterization:** Bounding box + Barycentric testing + Z-buffering.

Would you like me to update the **Markdown Export** to include these final Flat Shading and Wireframe sections?

Here is the updated, comprehensive Markdown export. This version includes the final architectural pieces: **Vertex Indexing**, **Wireframe Debugging**, and **Flat Shading/Lighting**.

---

# Complete Software Rasterizer Pipeline (Raylib Edition)

This document provides a full reference for a CPU-based 3D rendering pipeline integrated with Raylib. It covers everything from buffer management to indexed 3D drawing with lighting.

---

## 1. Pipeline State & Initialization

We use a "State" structure to manage the CPU color buffer, depth buffer, and the GPU texture used for blitting.

```cpp
#include "raylib.h"
#include <stdint.h>
#include <float.h>
#include <math.h>

typedef struct {
    Vector3 position;
    Color color;
} Vertex;

typedef struct {
    Vector3 screenPos; 
    Color color;
    bool clipped;
} TransformedVertex;

typedef struct {
    int width, height;
    Image colorBuffer;
    float *depthBuffer;
    Texture2D displayTex;
    Matrix mvp;
} SoftRenderer;

SoftRenderer InitSoftRenderer(int w, int h) {
    SoftRenderer sr;
    sr.width = w;
    sr.height = h;
    sr.colorBuffer = GenImageColor(w, h, BLACK);
    sr.depthBuffer = (float *)malloc(w * h * sizeof(float));
    sr.displayTex = LoadTextureFromImage(sr.colorBuffer);
    return sr;
}

void BeginSoftFrame(SoftRenderer *sr) {
    ImageClearBackground(&sr->colorBuffer, BLACK);
    for (int i = 0; i < sr->width * sr->height; i++) sr->depthBuffer[i] = FLT_MAX;
}

void EndSoftFrame(SoftRenderer *sr) {
    UpdateTexture(sr->displayTex, sr->colorBuffer.data);
    DrawTexture(sr->displayTex, 0, 0, WHITE);
}

```

---

## 2. Geometry & Lighting Logic

### Face Normals & Lambertian Shading

To give objects a "solid" look, we calculate how much light hits a face using the Dot Product.

```cpp
Vector3 GetTriangleNormal(Vector3 v0, Vector3 v1, Vector3 v2) {
    Vector3 edge1 = Vector3Subtract(v1, v0);
    Vector3 edge2 = Vector3Subtract(v2, v0);
    return Vector3Normalize(Vector3CrossProduct(edge1, edge2));
}

inline float EdgeFunction(float ax, float ay, float bx, float by, float px, float py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

```
**Would you like me to show you how to implement simple "Flat Shading" or "Directional Lighting" so your 3D objects don't just look like solid silhouettes?**

To give your 3D objects depth, you need to calculate how light hits each surface. The simplest way to do this is **Flat Shading**, where one lighting value is calculated per triangle. A step up is **Gouraud Shading**, where you calculate lighting at each vertex and interpolate the colors.

Both rely on the **Normal Vector** ($\vec{N}$)—a vector pointing directly "out" of the triangle's surface.

---

## 1. The Math: Lambert’s Cosine Law

The brightness of a surface depends on the angle between the surface normal ($\vec{N}$) and the direction of the light source ($\vec{L}$).

The intensity is calculated using the **Dot Product**:

$$Intensity = \text{max}(0, \vec{N} \cdot \vec{L})$$

* If the light is hitting the face directly, the dot product is **1.0** (Maximum brightness).
* If the light is grazing the side, the value drops toward **0.0**.
* The `max(0, ...)` ensures that if the light is behind the triangle, the intensity doesn't become negative (which would look weird).



---

## 2. Calculating the Face Normal

If your 3D model doesn't provide normals, you can calculate the "Face Normal" using the **Cross Product** of two edges of the triangle.

```cpp
Vec3 calculate_normal(Vec3 v0, Vec3 v1, Vec3 v2) {
    Vec3 edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
    Vec3 edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

    // Cross Product
    Vec3 normal;
    normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
    normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
    normal.z = edge1.x * edge2.y - edge1.y * edge2.x;

    // Normalize (make the length 1.0)
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length > 0) {
        normal.x /= length; normal.y /= length; normal.z /= length;
    }
    return normal;
}
```

---

## 3. Implementation: Flat Shading

In Flat Shading, you calculate the color **once** per triangle before it enters the rasterizer.

```cpp
void process_triangle_with_lighting(Vertex v1, Vertex v2, Vertex v3) {
    // 1. Calculate Normal (usually in World Space or View Space)
    Vec3 normal = calculate_normal(v1.worldPos, v2.worldPos, v3.worldPos);

    // 2. Define a directional light (e.g., coming from the top-right)
    Vec3 lightDir = {0.5f, -1.0f, 0.5f}; // Normalize this!
    
    // 3. Calculate Dot Product (Intensity)
    float dot = -(normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z);
    float intensity = std::max(0.1f, dot); // 0.1 is "Ambient" light so it's not pitch black

    // 4. Apply to color
    Color faceColor = { (uint8_t)(255 * intensity), (uint8_t)(255 * intensity), (uint8_t)(255 * intensity), 255 };

    // 5. Pass this single color to the whole triangle
    v1.color = v2.color = v3.color = faceColor;
    
    // Continue to clipping/rasterization...
}
```



---

## 4. Upgrading to Gouraud Shading

Flat shading makes objects look "blocky" because you can see the edges of the triangles. To make a smooth sphere:
1.  **Vertex Normals:** Instead of one normal per face, each vertex has its own normal (usually provided by the 3D model file like an `.obj`).
2.  **Per-Vertex Light:** Calculate the dot product at *each* vertex.
3.  **Interpolate:** Use your **Barycentric weights** in the pixel loop to blend those colors across the face.

### The Lighting Pipeline
* **Flat Shading:** 1 Dot Product per Triangle. Fast, retro look.
* **Gouraud Shading:** 3 Dot Products per Triangle + Color Interpolation. Smooth look.
* **Phong Shading:** 1 Dot Product **per Pixel**. Slowest, but allows for shiny "specular" highlights.

**Would you like me to show you how to implement 'Phong Shading' for shiny specular highlights?** No
