

Does your current Clipping code use the $W$ component to determine the frustum planes (i.e., $-w \le x \le w$), or are you clipping against the raw $l, r, b, t$ values?

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

### Model x View x Projection

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

## Model matrix
In the examples provided, the **Model Matrix** was set to the **Identity Matrix** ($I$) simply to keep the initial setup as clean as possible. In mathematical terms, multiplying a vector by the Identity Matrix is like multiplying a number by 1: the output is identical to the input.

However, in a real software pipeline, the Model Matrix is where all the "action" happens for individual objects.

---

### The Role of the Model Matrix

The Model Matrix transforms a vertex from **Local Space** (coordinates relative to the object's center) to **World Space** (coordinates relative to the center of your entire 3D universe).

If you keep it as the Identity:

1. **Translation:** Your object stays at $(0, 0, 0)$.
2. **Rotation:** Your object has $0^{\circ}$ rotation on all axes.
3. **Scale:** Your object remains at its original designed size (Scale = $1.0$).

---

### When to change it

You would stop using the Identity Matrix as soon as you want to move, spin, or resize your object. Usually, you build the Model Matrix by multiplying three smaller matrices together:

$$Model = Translation \times Rotation \times Scale$$

For example, if you wanted to draw **two different cubes** using the same vertex data, you would:

1. Set the Model Matrix to a "Left" translation $\rightarrow$ **Draw call 1**.
2. Set the Model Matrix to a "Right" translation $\rightarrow$ **Draw call 2**.

### Why it was Identity in the snippets

In the "Rotating Cube" example, I actually swapped the Identity for a rotation matrix:
`Matrix model = MatrixRotateXYZ((Vector3){time, time * 0.5f, 0});`

This effectively updated the "State" of the Model from Identity to a specific orientation based on the current time. If I had left it as Identity, the cube would just sit there perfectly still at the center of the world.

---

### Summary of the Transformation Chain

To visualize how the Identity Model Matrix fits into the overall "OpenGL-style" pipeline:

1. **Vertex $\times$ Identity (Model):** "I am a vertex, and I am stay exactly where I was defined."
2. **Vertex $\times$ View:** "Now, where am I relative to the camera?"
3. **Vertex $\times$ Projection:** "Now, where am I on the 2D screen (with perspective)?"

Would you like me to show you how to implement a **MatrixMultiplication** function so you can combine Translation, Rotation, and Scale into a single Model Matrix for your software renderer?

In a software rasterizer, efficiency is key. Since you'll be calculating the **Model-View-Projection (MVP)** matrix once per object (or even per frame), you need a reliable way to "collapse" multiple transformations into a single matrix.

In a standard pipeline, you multiply them in this specific order (reading right-to-left in math notation):


$$MVP = Projection \times View \times (Translation \times Rotation \times Scale)$$

### 1. Matrix Multiplication Implementation

Since you're using Raylib, you can use `MatrixMultiply()`, but if you want to keep the "software-only" spirit, here is the standard $4 \times 4$ implementation:

```cpp
Matrix MyMatrixMultiply(Matrix left, Matrix right) {
    Matrix result = { 0 };
    float *a = (float *)&left;
    float *b = (float *)&right;
    float *r = (float *)&result;

    for (int i = 0; i < 4; i++) {       // Row
        for (int j = 0; j < 4; j++) {   // Column
            r[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] +
                           a[i * 4 + 1] * b[1 * 4 + j] +
                           a[i * 4 + 2] * b[2 * 4 + j] +
                           a[i * 4 + 3] * b[3 * 4 + j];
        }
    }
    return result;
}

```

### 2. Building a Dynamic Model Matrix

Instead of using the Identity matrix, you now create a matrix that represents the object's state in the world.

```cpp
// 1. Create individual component matrices
Matrix mScale = MatrixScale(1.5f, 1.5f, 1.5f);
Matrix mRot   = MatrixRotateXYZ((Vector3){ 0, GetTime(), 0 });
Matrix mTrans = MatrixTranslate(2.0f, 0.0f, -5.0f);

// 2. Combine them into one Model Matrix
// Order matters: Scale first, then Rotate, then Translate
Matrix model = MyMatrixMultiply(MyMatrixMultiply(mScale, mRot), mTrans);

// 3. Combine with View and Projection for the final MVP
Matrix mvp = MyMatrixMultiply(model, MyMatrixMultiply(view, projection));

```

---

### Why the Order Matters

If you translate an object *before* you rotate it, it will orbit around the world origin $(0,0,0)$ like a planet around a sun. If you rotate it *before* you translate it, it spins in place at its own center and then moves to the new position. Usually, you want the latter.

### Final Technical Summary update

I’ve integrated this into the master markdown. You now have:

* **Buffer Blitting** (Raylib interface)
* **Vertex Pipeline** (Attributes and Indexing)
* **Projection Math** (OpenGL spec)
* **Clipping** (Sutherland-Hodgman)
* **Rasterization** (Sub-pixel, Incremental Barycentric, SIMD concepts)
* **Transformation Math** (MVP construction)