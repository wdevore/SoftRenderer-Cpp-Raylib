Here is a comprehensive markdown file compiling our entire discussion on building an OpenGL-style software rasterizer using Raylib as the display backend.

---

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

---

## 3. The Programmable Pipeline

This function implements **Vertex Shading**, **Culling**, and **Rasterization**. It supports both **Filled** and **Wireframe** modes.

```cpp
void MyDrawIndexed(SoftRenderer *sr, Vertex *vertices, int vCount, int *indices, int iCount, bool wireframe) {
    Color *pixels = (Color *)sr->colorBuffer.data;
    TransformedVertex *cache = (TransformedVertex *)malloc(vCount * sizeof(TransformedVertex));
    Vector3 lightDir = Vector3Normalize((Vector3){ 0.5f, 1.0f, 0.5f });

    // STAGE 1: Vertex Shader (Transform & Cache)
    for (int i = 0; i < vCount; i++) {
        Vector4 clip = Vector4Transform((Vector4){vertices[i].position.x, vertices[i].position.y, vertices[i].position.z, 1.0f}, sr->mvp);
        if (clip.w < 0.1f) { cache[i].clipped = true; continue; }
        
        cache[i].clipped = false;
        cache[i].screenPos = (Vector3){
            (clip.x/clip.w + 1) * sr->width / 2.0f,
            (1 - clip.y/clip.w) * sr->height / 2.0f,
            clip.z / clip.w
        };
    }

    // STAGE 2: Primitive Assembly & Rasterization
    for (int i = 0; i < iCount; i += 3) {
        int i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];
        TransformedVertex tv0 = cache[i0], tv1 = cache[i1], tv2 = cache[i2];
        if (tv0.clipped || tv1.clipped || tv2.clipped) continue;

        // Face Normal & Backface Culling
        Vector3 norm = GetTriangleNormal(vertices[i0].position, vertices[i1].position, vertices[i2].position);
        // Simple culling (assumes camera is at origin looking forward in view space)
        if (norm.z < 0) continue; 

        if (wireframe) {
            DrawLineBresenham(sr, tv0.screenPos, tv1.screenPos, GREEN);
            DrawLineBresenham(sr, tv1.screenPos, tv2.screenPos, GREEN);
            DrawLineBresenham(sr, tv2.screenPos, tv0.screenPos, GREEN);
        } else {
            // Flat Shading calculation
            float intensity = fmaxf(0.2f, Vector3DotProduct(norm, lightDir));
            Color faceColor = {(unsigned char)(200*intensity), (unsigned char)(200*intensity), (unsigned char)(255*intensity), 255};

            // Rasterize Triangle (Bounding Box)
            int minX = (int)fmaxf(0, fminf(tv0.screenPos.x, fminf(tv1.screenPos.x, tv2.screenPos.x)));
            int maxX = (int)fminf(sr->width-1, fmaxf(tv0.screenPos.x, fmaxf(tv1.screenPos.x, tv2.screenPos.x)));
            int minY = (int)fmaxf(0, fminf(tv0.screenPos.y, fminf(tv1.screenPos.y, tv2.screenPos.y)));
            int maxY = (int)fminf(sr->height-1, fmaxf(tv0.screenPos.y, fmaxf(tv1.screenPos.y, tv2.screenPos.y)));

            float area = EdgeFunction(tv0.screenPos.x, tv0.screenPos.y, tv1.screenPos.x, tv1.screenPos.y, tv2.screenPos.x, tv2.screenPos.y);
            
            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {
                    float w0 = EdgeFunction(tv1.screenPos.x, tv1.screenPos.y, tv2.screenPos.x, tv2.screenPos.y, x, y) / area;
                    float w1 = EdgeFunction(tv2.screenPos.x, tv2.screenPos.y, tv0.screenPos.x, tv0.screenPos.y, x, y) / area;
                    float w2 = EdgeFunction(tv0.screenPos.x, tv0.screenPos.y, tv1.screenPos.x, tv1.screenPos.y, x, y) / area;

                    if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                        int idx = y * sr->width + x;
                        float z = w0 * tv0.screenPos.z + w1 * tv1.screenPos.z + w2 * tv2.screenPos.z;
                        if (z < sr->depthBuffer[idx]) {
                            sr->depthBuffer[idx] = z;
                            pixels[idx] = faceColor;
                        }
                    }
                }
            }
        }
    }
    free(cache);
}

```

---

## 4. Usage Example: Rotating Cube

```cpp
int main() {
    InitWindow(800, 450, "Software Pipeline");
    SoftRenderer sr = InitSoftRenderer(800, 450);
    bool wireframe = false;

    // ... define cubeVertices and cubeIndices ...

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_W)) wireframe = !wireframe;
        
        // MVP Matrix Setup
        Matrix model = MatrixRotateXYZ((Vector3){GetTime(), GetTime()*0.5f, 0});
        Matrix view = MatrixTranslate(0, 0, -3.0f);
        Matrix proj = MatrixPerspective(60*DEG2RAD, 800.0/450.0, 0.1, 100.0);
        sr.mvp = MatrixMultiply(MatrixMultiply(model, view), proj);

        BeginDrawing();
            BeginSoftFrame(&sr);
                MyDrawIndexed(&sr, cubeVertices, 8, cubeIndices, 36, wireframe);
            EndSoftFrame(&sr);
            DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

```

---

This concludes our session on porting a software rasterizer to Raylib. You now have a modular, indexed, lit, and depth-tested 3D engine!

Would you like me to clarify any specific part of the math before you start your implementation?

To get a 3D point (World Space) onto your 2D screen (Screen Space), we need to pass it through a pipeline. Since you are building a software rasterizer, you’ll be doing the math that a GPU normally hides from you.The process follows this chain: **Local Space** $\rightarrow$ **World Space** $\rightarrow$ **View Space** $\rightarrow$ **Clip Space** $\rightarrow$ **Screen Space**.

1. The Perspective Projection MatrixThis matrix defines your Field of View (FOV) and creates the "foreshortening" effect where distant objects appear smaller.

C++
```cpp
    Matrix MyComputeProjectionMatrix(float fov, float aspect, float near, float far) {
        float top = near * tanf(fov * 0.5f * DEG2RAD);
        float right = top * aspect;
        
        // Standard perspective matrix
        return (Matrix){
            near/right, 0,          0,                              0,
            0,          near/top,   0,                              0,
            0,          0,          -(far + near)/(far - near),    -1,
            0,          0,          -(2*far*near)/(far - near),     0
        };
    }
```
2. The Vertex Transformation Function Before sending vertices to MyDrawTriangle3D, you must transform them. A key step here is the Perspective Divide ($W$-divide).

C++
```cpp
    Vector3 ProjectVertex(Vector3 v, Matrix mvp, int width, int height) {
        // 1. Transform vertex by Model-View-Projection matrix
        // Note: Vector3Transform usually handles the 4th (W) component internally
        float x = v.x * mvp.m0 + v.y * mvp.m4 + v.z * mvp.m8 + mvp.m12;
        float y = v.x * mvp.m1 + v.y * mvp.m5 + v.z * mvp.m9 + mvp.m13;
        float z = v.x * mvp.m2 + v.y * mvp.m6 + v.z * mvp.m10 + mvp.m14;
        float w = v.x * mvp.m3 + v.y * mvp.m7 + v.z * mvp.m11 + mvp.m15;

        // 2. Perspective Divide (Convert to Normalized Device Coordinates -1 to 1)
        if (w != 0.0f) {
            x /= w; y /= w; z /= w;
        }

        // 3. Viewport Transform (Convert NDC to Screen Pixels)
        return (Vector3){
            (x + 1.0f) * 0.5f * width,
            (1.0f - y) * 0.5f * height, // Flip Y because screen Y grows downward
            z                           // Keep Z for the Z-buffer
        };
}
```
3. Putting it all together in the LoopIn your Raylib while loop, you calculate your matrices once per frame and then project your triangles.

C++
```cpp
    // setup
    Matrix model = MatrixIdentity();
    Matrix view = MatrixLookAt((Vector3){0, 2, 5}, (Vector3){0, 0, 0}, (Vector3){0, 1, 0});
    Matrix projection = MyComputeProjectionMatrix(60.0f, (float)screenWidth/screenHeight, 0.1f, 100.0f);
    Matrix mvp = MatrixMultiply(MatrixMultiply(model, view), projection);

    // Define a 3D triangle (e.g., a simple tilted floor)
    Vector3 p0 = { -1, 0, -1 };
    Vector3 p1 = {  1, 0, -1 };
    Vector3 p2 = {  0, 0,  1 };

    // Project to screen
    Vector3 s0 = ProjectVertex(p0, mvp, screenWidth, screenHeight);
    Vector3 s1 = ProjectVertex(p1, mvp, screenWidth, screenHeight);
    Vector3 s2 = ProjectVertex(p2, mvp, screenWidth, screenHeight);

    // Rasterize!
    MyDrawTriangle3D(&canvas, zBuffer, s0, s1, s2, LIME);
```
Final "Gotchas" for your software renderer:

- Clipping: If a vertex goes behind the camera ($Z < near$), the math will produce weird results (or a crash). A "real" rasterizer clips triangles against the near plane. For a simple start, you can just skip drawing any triangle if any of its vertices have $w < 0.1$.

- Backface Culling: You can save 50% of your performance by not drawing triangles that face away from the camera. Use the dot product of the triangle normal and the view vector to check this.

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

# Triangle rasterization
The **Barycentric Rasterization approach** is a modern, mathematically elegant way to fill triangles in a software or hardware renderer.

Unlike older "Scanline" or "Edge-Walking" methods that draw a triangle row-by-row like a typewriter, the Barycentric approach treats the triangle as a mathematical field.

---

### 1. The Core Concept: The "Point-in-Triangle" Test

The algorithm works by defining a **Bounding Box** around the triangle’s three vertices ($V_0, V_1, V_2$). It then iterates through every pixel $(P)$ in that box and asks: *"Is this pixel inside the triangle?"*

To answer this, it calculates three weights ($w_0, w_1, w_2$), known as **Barycentric Coordinates**. These coordinates represent $P$ as a weighted average of the vertices:


$$P = w_0 V_0 + w_1 V_1 + w_2 V_2$$

### 2. How the Weights are Calculated

The weights are derived from the **Edge Function**. For each edge of the triangle (e.g., the line from $V_0$ to $V_1$), the edge function returns a value based on which side of the line point $P$ lies.

* **If all three weights are $\ge 0$:** The pixel is inside the triangle.
* **If any weight is $< 0$:** The pixel is outside.
* **The magnitude of the weight:** Represents how "close" the pixel is to that specific vertex.

### 3. Why This Approach is Superior

This method is the foundation of modern GPU architecture for several reasons:

* **Attribute Interpolation:** These same weights ($w_0, w_1, w_2$) are used to calculate the exact color, depth ($Z$), or UV coordinate for that specific pixel. If $w_0$ is 0.8, the pixel is 80% "influenced" by the properties of $V_0$.
* **Parallelism:** Because every pixel check is independent of the others, you can easily split the bounding box into tiles and process them simultaneously on multiple CPU cores or GPU warps.
* **Sub-pixel Accuracy:** It is easy to evaluate the edge function at specific sub-pixel offsets, allowing for high-quality anti-aliasing (MSAA).

---

### 4. Mathematical Implementation

For a triangle with vertices $A, B, C$, the weight $w_0$ (corresponding to vertex $A$) is proportional to the signed area of the sub-triangle formed by the point $P$ and the opposite edge $BC$.

$$w_0 = \frac{\text{Area}(PBC)}{\text{Area}(ABC)}, \quad w_1 = \frac{\text{Area}(PAC)}{\text{Area}(ABC)}, \quad w_2 = \frac{\text{Area}(PAB)}{\text{Area}(ABC)}$$

> **Note:** In a real-time rasterizer, you don't actually calculate areas with square roots. You use the **Cross Product** (the 2D "perp-dot product"), which is just a few multiplications and subtractions.

---

### 5. Integration into your Software Pipeline

In your current Raylib port, this logic sits inside your pixel loop:

1. **Calculate Bounding Box:** `minX, maxX, minY, maxY`.
2. **Loop:** For each $(x, y)$ in the box:
* Compute $w_0, w_1, w_2$ using your `EdgeFunction`.
* If all are positive:
* Interpolate $Z$ (Depth).
* Interpolate $Color$.
* Perform Depth Test.
* Write to `colorBuffer`.


Would you like to see how to optimize this loop using **incremental edge evaluation** so you don't have to re-calculate the full edge function for every single pixel?

To make the Barycentric approach efficient enough for a real-time software rasterizer, we use **Incremental Edge Evaluation**.

Currently, our `EdgeFunction` performs several multiplications for every single pixel. However, since we are moving across the screen in a grid, the value of the edge function changes by a **constant amount** for every step in $x$ or $y$.

## The Math of Incremental Steps

The edge function for a line $(x_0, y_0)$ to $(x_1, y_1)$ at point $(x, y)$ is:


$$E(x, y) = (x - x_0)(y_1 - y_0) - (y - y_0)(x_1 - x_0)$$

If we move one pixel to the right ($x + 1$), the change ($\Delta E_x$) is simply:


$$\Delta E_x = y_1 - y_0$$

If we move one pixel down ($y + 1$), the change ($\Delta E_y$) is:


$$\Delta E_y = -(x_1 - x_0)$$

By calculating these constants once per triangle, the inner loop becomes simple **addition**, which is much faster than multiplication.

---

### Optimized Rasterizer Implementation

Here is how you would refactor the inner loop of your software pipeline:

```cpp
void RasterizeTriangleOptimized(SoftRenderer *sr, TransformedVertex v0, TransformedVertex v1, TransformedVertex v2, Color color) {
    // 1. Setup Bounding Box (omitted for brevity)
    
    // 2. Pre-calculate constant increments for each edge
    float dy01 = v0.screenPos.y - v1.screenPos.y; float dx01 = v1.screenPos.x - v0.screenPos.x;
    float dy12 = v1.screenPos.y - v2.screenPos.y; float dx12 = v2.screenPos.x - v1.screenPos.x;
    float dy20 = v2.screenPos.y - v0.screenPos.y; float dx20 = v0.screenPos.x - v2.screenPos.x;

    // 3. Calculate initial edge function values at the start of the bounding box (minX, minY)
    float w0_row = EdgeFunction(v1.screenPos.x, v1.screenPos.y, v2.screenPos.x, v2.screenPos.y, minX, minY);
    float w1_row = EdgeFunction(v2.screenPos.x, v2.screenPos.y, v0.screenPos.x, v0.screenPos.y, minX, minY);
    float w2_row = EdgeFunction(v0.screenPos.x, v0.screenPos.y, v1.screenPos.x, v1.screenPos.y, minX, minY);

    float area = EdgeFunction(v0.screenPos.x, v0.screenPos.y, v1.screenPos.x, v1.screenPos.y, v2.screenPos.x, v2.screenPos.y);

    for (int y = minY; y <= maxY; y++) {
        // Temporary edge values for this specific row
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = minX; x <= maxX; x++) {
            // Check if inside triangle (using bitwise OR on the signs for a tiny speed boost)
            if (( (int)w0 | (int)w1 | (int)w2 ) >= 0) {
                int idx = y * sr->width + x;
                // Still need division for interpolation weights
                float invArea = 1.0f / area;
                float weight0 = w0 * invArea;
                float weight1 = w1 * invArea;
                float weight2 = w2 * invArea;

                float z = weight0 * v0.screenPos.z + weight1 * v1.screenPos.z + weight2 * v2.screenPos.z;
                if (z < sr->depthBuffer[idx]) {
                    sr->depthBuffer[idx] = z;
                    ((Color*)sr->colorBuffer.data)[idx] = color;
                }
            }
            
            // Increment X: Move edge values to the right
            w0 += dy12;
            w1 += dy20;
            w2 += dy01;
        }

        // Increment Y: Move row starting values down
        w0_row += dx12;
        w1_row += dx20;
        w2_row += dx01;
    }
}

```

### Why this is a "Pro" move:

1. **Strength Reduction:** You’ve replaced multiple subtractions and multiplications in the inner loop with just three additions (`w0 += dy12`, etc.).
2. **Floating Point Performance:** Modern CPUs can execute additions in parallel very efficiently.
3. **Tile-Based Rendering:** This setup makes it very easy to implement "Tiled Rasterization." Instead of a full bounding box, you can check 8x8 blocks of pixels at once. If all four corners of a block are inside the triangle, you can skip the edge checks for all 64 pixels and just fill them.

### Final Markdown Export Update

I will include this **Incremental Rasterization** section in the final documentation, as it's the difference between a "hobby" rasterizer and one that can actually handle complex scenes at 60 FPS.

Would you like me to generate the final, all-in-one **Markdown File** now, including the Clipping integration and this Optimized Rasterizer?