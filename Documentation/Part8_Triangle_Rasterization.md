

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