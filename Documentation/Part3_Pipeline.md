
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