
# Software Rasterizer Pipeline with Raylib

This document outlines the architecture and implementation of a CPU-based 3D rendering pipeline, from basic line drawing to an indexed, programmable-style vertex/fragment architecture.

---

## 1. The Core Raylib "Shell"

To use Raylib as a software renderer, we maintain a pixel buffer on the CPU, modify it, and upload it to a GPU texture for display.

```cpp
#include "raylib.h"
#include <stdint.h>
#include <float.h>
#include <math.h>

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

```

---

## 2. Vertex and Fragment Structures

Defining the data containers that flow through the pipeline.

```cpp
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
    float depth;
    Color color;
} Fragment;

```

---

## 3. The Math Engine

### Edge Function

Used for determining if a point is inside a triangle and calculating Barycentric weights.

$$Area(A, B, P) = (P.x - A.x) * (B.y - A.y) - (P.y - A.y) * (B.x - A.x)$$

```cpp
inline float EdgeFunction(float ax, float ay, float bx, float by, float px, float py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

```

### Perspective Projection

Converts 3D coordinates into a 2D view with foreshortening.

```cpp
Matrix MyComputeProjection(float fov, float aspect, float near, float far) {
    float top = near * tanf(fov * 0.5f * (PI/180.0f));
    float right = top * aspect;
    return (Matrix){
        near/right, 0, 0, 0,
        0, near/top, 0, 0,
        0, 0, -(far + near)/(far - near), -1,
        0, 0, -(2*far*near)/(far - near), 0
    };
}

```

---

## 4. Programmable Stages

### Vertex Shader

Transforms a single vertex from Local Space to Clip Space.

```cpp
Vector4 VertexShader(Vertex v, Matrix mvp) {
    return Vector4Transform((Vector4){v.position.x, v.position.y, v.position.z, 1.0f}, mvp);
}

```

## Vector4Transform
To transform a `Vector4` by a `Matrix4f` in a **Column-Major** system, you are essentially calculating the linear combination of the matrix's columns, weighted by the components of the vector.

Since your matrix is stored as `m[Column * 4 + Row]`, the mathematical operation for $V_{out} = M \times V_{in}$ looks like this:

### 1. The Mathematical Logic
Each component of the output vector is the **Dot Product** of a **Row** of the matrix and the input **Vector**.

$$
\begin{bmatrix} x' \\ y' \\ z' \\ w' \end{bmatrix} = 
\begin{bmatrix} 
m_{00} & m_{01} & m_{02} & m_{03} \\
m_{10} & m_{11} & m_{12} & m_{13} \\
m_{20} & m_{21} & m_{22} & m_{23} \\
m_{30} & m_{31} & m_{32} & m_{33} 
\end{bmatrix}
\times
\begin{bmatrix} x \\ y \\ z \\ w \end{bmatrix}
$$



---

### 2. C++ Implementation (Column-Major)
Given your storage logic `m[Col * 4 + Row]`, here is how you implement the transformation. Note how the first index of the array (the column) increments in the inner summations of each row calculation.

```cpp
Vector4 Vector4Transform(Vector4 v, Matrix4f mat) {
    Vector4 result;
    float *m = mat.m; // Assuming m is the internal float[16] array

    // Row 0: (Col0,Row0)*x + (Col1,Row0)*y + (Col2,Row0)*z + (Col3,Row0)*w
    result.x = m[0*4+0]*v.x + m[1*4+0]*v.y + m[2*4+0]*v.z + m[3*4+0]*v.w;
    
    // Row 1: (Col0,Row1)*x + (Col1,Row1)*y + (Col2,Row1)*z + (Col3,Row1)*w
    result.y = m[0*4+1]*v.x + m[1*4+1]*v.y + m[2*4+1]*v.z + m[3*4+1]*v.w;
    
    // Row 2: (Col0,Row2)*x + (Col1,Row2)*y + (Col2,Row2)*z + (Col3,Row2)*w
    result.z = m[0*4+2]*v.x + m[1*4+2]*v.y + m[2*4+2]*v.z + m[3*4+2]*v.w;
    
    // Row 3: (Col0,Row3)*x + (Col1,Row3)*y + (Col2,Row3)*z + (Col3,Row3)*w
    result.w = m[0*4+3]*v.x + m[1*4+3]*v.y + m[2*4+3]*v.z + m[3*4+3]*v.w;

    return result;
}
```

---

### 3. Why this order is critical for your Rasterizer
In your software pipeline, this function is the **"Vertex Shader."** * **The X, Y, Z results:** These are your coordinates in Clip Space.
* **The W result:** This is the most important value for perspective. In a standard projection, $W$ usually ends up being $-Z_{view}$.
* **Perspective Divide:** Immediately after this transform, you will do `x/w` and `y/w` to get the 2D screen coordinates. If your `Vector4Transform` uses the wrong indices, your $W$ will be wrong, and the triangle will either look like a flat 2D shape or distort wildly as you move.

### Performance Tip:
If you are transforming thousands of vertices, this is where **SIMD** helps the most. Since each row calculation is a dot product, you can use `_mm_mul_ps` and `_mm_add_ps` to calculate all four components ($x, y, z, w$) in much fewer clock cycles.

Would you like me to update the **Final Markdown Export** to include this specific `Vector4Transform` logic alongside your `setFrustum` code?

### Fragment Shader

Calculates the final pixel color.

```cpp
Color FragmentShader(Fragment frag) {
    return frag.color; // Standard Gouraud output
}

```

---

## 5. The Indexed Rasterizer

This is the main "Draw" call, implementing vertex caching and triangle rasterization via a bounding box.

```cpp
void MyDrawIndexed(SoftRenderer *sr, Vertex *vertices, int vCount, int *indices, int iCount) {
    Color *pixels = (Color *)sr->colorBuffer.data;
    TransformedVertex *cache = (TransformedVertex *)malloc(vCount * sizeof(TransformedVertex));

    // STAGE 1: Vertex Processing
    for (int i = 0; i < vCount; i++) {
        Vector4 clip = VertexShader(vertices[i], sr->mvp);
        if (clip.w < 0.1f) { cache[i].clipped = true; continue; }
        
        cache[i].clipped = false;
        cache[i].screenPos = (Vector3){
            (clip.x/clip.w + 1) * sr->width / 2.0f,
            (1 - clip.y/clip.w) * sr->height / 2.0f,
            clip.z / clip.w
        };
        cache[i].color = vertices[i].color;
    }

    // STAGE 2: Rasterization
    for (int i = 0; i < iCount; i += 3) {
        TransformedVertex v0 = cache[indices[i]], v1 = cache[indices[i+1]], v2 = cache[indices[i+2]];
        if (v0.clipped || v1.clipped || v2.clipped) continue;

        // Bounding Box
        int minX = (int)fmaxf(0, fminf(v0.screenPos.x, fminf(v1.screenPos.x, v2.screenPos.x)));
        int maxX = (int)fminf(sr->width - 1, fmaxf(v0.screenPos.x, fmaxf(v1.screenPos.x, v2.screenPos.x)));
        int minY = (int)fmaxf(0, fminf(v0.screenPos.y, fminf(v1.screenPos.y, v2.screenPos.y)));
        int maxY = (int)fminf(sr->height - 1, fmaxf(v0.screenPos.y, fmaxf(v1.screenPos.y, v2.screenPos.y)));

        float area = EdgeFunction(v0.screenPos.x, v0.screenPos.y, v1.screenPos.x, v1.screenPos.y, v2.screenPos.x, v2.screenPos.y);
        if (area <= 0) continue; // Backface culling

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                float w0 = EdgeFunction(v1.screenPos.x, v1.screenPos.y, v2.screenPos.x, v2.screenPos.y, x, y) / area;
                float w1 = EdgeFunction(v2.screenPos.x, v2.screenPos.y, v0.screenPos.x, v0.screenPos.y, x, y) / area;
                float w2 = EdgeFunction(v0.screenPos.x, v0.screenPos.y, v1.screenPos.x, v1.screenPos.y, x, y) / area;

                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    int idx = y * sr->width + x;
                    float z = w0 * v0.screenPos.z + w1 * v1.screenPos.z + w2 * v2.screenPos.z;

                    if (z < sr->depthBuffer[idx]) {
                        sr->depthBuffer[idx] = z;
                        Fragment f = {z, (Color){
                            (unsigned char)(v0.color.r*w0 + v1.color.r*w1 + v2.color.r*w2),
                            (unsigned char)(v0.color.g*w0 + v1.color.g*w1 + v2.color.g*w2),
                            (unsigned char)(v0.color.b*w0 + v1.color.b*w1 + v2.color.b*w2), 255
                        }};
                        pixels[idx] = FragmentShader(f);
                    }
                }
            }
        }
    }
    free(cache);
}

```

---

## 6. Optimization: Backface Culling

To prevent drawing the insides of meshes, we check the winding order relative to the camera.

```cpp
Vector3 edge1 = Vector3Subtract(v1.position, v0.position);
Vector3 edge2 = Vector3Subtract(v2.position, v0.position);
Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));
Vector3 viewDir = Vector3Subtract(v0.position, cameraPos);

if (Vector3DotProduct(normal, viewDir) < 0) {
    // Triangle is facing the camera - Render it!
}

```

---

Would you like me to help you set up a simple **Cube mesh** array to test this whole pipeline immediately?

To test your new software pipeline, we can define a standard 1-unit cube. This requires 8 unique vertices and 36 indices (12 triangles).

### 1. The Cube Mesh Data

Copy these arrays into your `main` function or a separate `Mesh` file.

```cpp
// 8 unique vertices of a cube
Vertex cubeVertices[] = {
    {{ -0.5f, -0.5f,  0.5f }, WHITE}, // 0: Bottom-front-left
    {{  0.5f, -0.5f,  0.5f }, RED},   // 1: Bottom-front-right
    {{  0.5f,  0.5f,  0.5f }, GREEN}, // 2: Top-front-right
    {{ -0.5f,  0.5f,  0.5f }, BLUE},  // 3: Top-front-left
    {{ -0.5f, -0.5f, -0.5f }, YELLOW},// 4: Bottom-back-left
    {{  0.5f, -0.5f, -0.5f }, PURPLE},// 5: Bottom-back-right
    {{  0.5f,  0.5f, -0.5f }, ORANGE},// 6: Top-back-right
    {{ -0.5f,  0.5f, -0.5f }, MAGENTA}// 7: Top-back-left
};

// 36 indices (6 faces * 2 triangles per face * 3 vertices per triangle)
int cubeIndices[] = {
    // Front face
    0, 1, 2,  0, 2, 3,
    // Back face
    5, 4, 7,  5, 7, 6,
    // Top face
    3, 2, 6,  3, 6, 7,
    // Bottom face
    4, 5, 1,  4, 1, 0,
    // Right face
    1, 5, 6,  1, 6, 2,
    // Left face
    4, 0, 3,  4, 3, 7
};

```

---

### 2. The Integrated Main Loop

Here is how you use the `SoftRenderer` we built to draw this cube while rotating it.

```cpp
int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Software Rasterizer - Cube Test");

    SoftRenderer sr = InitSoftRenderer(screenWidth, screenHeight);
    
    // Setup Projection Matrix (60 deg FOV)
    Matrix projection = MyComputeProjection(60.0f, (float)screenWidth/screenHeight, 0.1f, 100.0f);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // 1. Update Camera/Movement
        float time = (float)GetTime();
        Matrix model = MatrixRotateXYZ((Vector3){ time, time * 0.5f, 0 });
        Matrix view  = MatrixTranslate(0, 0, -3.0f); // Move camera back 3 units
        
        // MVP = P * V * M
        sr.mvp = MatrixMultiply(MatrixMultiply(model, view), projection);

        // 2. Rendering
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginSoftFrame(&sr);
                // Execute the software pipeline
                MyDrawIndexed(&sr, cubeVertices, 8, cubeIndices, 36);
            EndSoftFrame(&sr);

            DrawFPS(10, 10);
            DrawText("Software Rasterized Cube (Gouraud Shading)", 10, 30, 20, RAYWHITE);
        EndDrawing();
    }

    UnloadSoftRenderer(&sr);
    CloseWindow();
    return 0;
}

```

---

### Implementation Tips for the Cube

* **Winding Order:** If a face of the cube is missing, check your `EdgeFunction` check (`w >= 0`). You may need to swap the order of indices for that face or change the check to `w <= 0`.
* **Vertex Colors:** Since we implemented Gouraud shading in the previous step, the cube will look like a "rainbow" cube because each vertex has a different color assigned.
* **Clipping:** If you move the camera too close (inside the cube), triangles will start disappearing because of our simple `clip.w < 0.1f` safety check.

You now have a fully functional 3D engine running entirely on your CPU! Would you like to see how to implement a **Wireframe Mode** toggle so you can see the triangles being formed?

Adding a **Wireframe Mode** is a classic debugging tool. In an OpenGL-style pipeline, this usually means changing the primitive assembly stage to draw lines between the vertices instead of filling the triangles.

Since we already have a `MyDrawLine` function (Bresenham) and our `MyDrawIndexed` loop, we can implement this by simply adding a boolean to our state.

### 1. Update the State and Loop

Add a toggle to your `SoftRenderer` or just pass a flag to your draw function.

```cpp
void MyDrawIndexed(SoftRenderer *sr, Vertex *vertices, int vCount, int *indices, int iCount, bool wireframe) {
    // ... STAGE 1 (Vertex Shading) remains exactly the same ...

    // STAGE 2: Primitive Assembly
    for (int i = 0; i < iCount; i += 3) {
        TransformedVertex v0 = cache[indices[i]], v1 = cache[indices[i+1]], v2 = cache[indices[i+2]];
        if (v0.clipped || v1.clipped || v2.clipped) continue;

        if (wireframe) {
            // Draw three lines instead of filling
            // Note: Use the projected screenPos.x/y
            MyDrawLine(&sr->colorBuffer, v0.screenPos.x, v0.screenPos.y, v1.screenPos.x, v1.screenPos.y, GREEN);
            MyDrawLine(&sr->colorBuffer, v1.screenPos.x, v1.screenPos.y, v2.screenPos.x, v2.screenPos.y, GREEN);
            MyDrawLine(&sr->colorBuffer, v2.screenPos.x, v2.screenPos.y, v0.screenPos.x, v0.screenPos.y, GREEN);
        } else {
            // ... Your existing RasterizeTriangle (Bounding Box) logic ...
        }
    }
}

```

---

### 2. Interaction in the Main Loop

In your Raylib loop, you can now toggle the view with a keypress (like `SPACE` or `W`).

```cpp
bool wireframeMode = false;

while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_W)) wireframeMode = !wireframeMode;

    BeginSoftFrame(&sr);
        MyDrawIndexed(&sr, cubeVertices, 8, cubeIndices, 36, wireframeMode);
    EndSoftFrame(&sr);
}

```

---

### Why this is useful for your Rasterizer:

* **Visualizing the Bounding Box:** If your triangles look "cut off," seeing the wireframe helps you determine if the issue is in the projection math or the rasterizer's pixel-filling loop.
* **Performance Comparison:** You will notice that wireframe mode is often significantly faster than filled mode because it skips the nested `for` loops that check every pixel in a bounding box.
* **Depth Issues:** If you draw the wireframe *without* depth testing (just drawing lines to the color buffer), you'll see every edge of the cube at once, which is helpful for verifying that your index buffer is connecting vertices correctly.

### A Final "Pro" Optimization

For your software renderer, the **Bounding Box** method we used is great, but it checks a lot of "dead pixels" for thin, diagonal triangles. A common optimization is **Scissor Testing** or **Edge Walking**, but for a basic port to Raylib, the Barycentric approach is much easier to maintain.

**Would you like me to show you how to implement a simple Backface Culling check so you don't waste time rasterizing triangles facing away from the camera?**

Backface culling is a powerful optimization that allows you to discard roughly **50% of your triangles** before they ever reach the rasterization stage. If a triangle is facing away from the camera (like the back of a cube), there is no reason to calculate its pixels.

The most efficient way to do this in a software rasterizer is to check the **Winding Order** of the vertices once they are in 2D screen space.

---

## 1. The Winding Order Logic

In 3D graphics, we define which side of a triangle is the "front" based on the order of its vertices. 
* **CCW (Counter-Clockwise):** Usually considered the "Front" face.
* **CW (Clockwise):** Usually considered the "Back" face.

When you project a 3D triangle onto a 2D screen, the 2D "signed area" of that triangle will tell you its winding order. If the area is negative, the triangle has "flipped" and is now facing away from you.



---

## 2. Implementation: The Cross-Product Check

We can reuse the same math from our Barycentric `area` calculation. You perform this check **after** projection but **before** the heavy pixel loop.

```cpp
bool is_backface(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2) {
    // Calculate the signed area (2D cross product of two edges)
    // Edge 1: (v1 - v0), Edge 2: (v2 - v0)
    float area = static_cast<float>((v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x));

    // If area is > 0, it is Counter-Clockwise (Front-facing)
    // If area is < 0, it is Clockwise (Back-facing)
    // If area is 0, it's a "degenerate" triangle (a line or point)
    
    // Standard OpenGL default: Cull Clockwise faces
    return area <= 0.0f; 
}
```

---

## 3. Integrating into the Pipeline

You should place this check immediately after `project_to_screen`. If the triangle is a backface, you `return` immediately and skip the entire bounding box and pixel loop.

```cpp
void process_and_rasterize(Vertex v1, Vertex v2, Vertex v3) {
    // ... (Clipping happens here) ...

    for (size_t i = 1; i < polygon.size() - 1; ++i) {
        ScreenVertex s0 = project_to_screen(polygon[0]);
        ScreenVertex s1 = project_to_screen(polygon[i]);
        ScreenVertex s2 = project_to_screen(polygon[i+1]);

        // BACKFACE CULLING
        if (is_backface(s0, s1, s2)) {
            continue; // Skip this triangle!
        }

        // Only now do we do the expensive rasterization
        draw_clipped_triangle(s0, s1, s2);
    }
}
```



---

## 4. Why This Works

When an object is closed (like a sphere or a character model), the back-facing triangles are always occluded by the front-facing ones. By removing them geometrically:
1.  You save the CPU from running the **Barycentric** math for every pixel of that triangle.
2.  You reduce the number of **Z-buffer** writes and **Frame Buffer** updates significantly.

### Important Note on "Double-Sided" Geometry
If you are rendering something thin like a "leaf" or a "piece of paper" where you want to see both sides, you must disable backface culling for those specific triangles.

---

### Your Completed Software Rasterizer Architecture

1.  **Vertex Processing:** Transform vertices to Clip Space.
2.  **Outcode Check:** Trivially accept or reject.
3.  **Clipping:** Sutherland-Hodgman (Focus on the Near Plane).
4.  **Projection:** Perspective divide and viewport transform.
5.  **Culling:** Check 2D winding order to remove backfaces.
6.  **Rasterization:** * Find Bounding Box.
    * **Scissor** box to screen dimensions.
    * Iterate pixels using **Barycentric weights**.
    * Perform **Z-buffer** test and write to Frame Buffer.

