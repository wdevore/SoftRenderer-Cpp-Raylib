Would you like me to add a **Simd (SSE/AVX)** optimization note to the markdown to show how you could process 4 or 8 pixels at once for even more speed?

Adding **SIMD (Single Instruction, Multiple Data)** is the ultimate performance tier for a software rasterizer. Instead of checking one pixel at a time, you use CPU registers (SSE/AVX) to process a block of pixels (like 4 or 8) in a single clock cycle.

### 1. The SIMD Strategy

In our current loop, we check if `(w0 | w1 | w2) >= 0`. With SIMD, we load the edge values for 4 horizontal pixels into one register. We then perform the "inside" test on all 4 simultaneously, creating a **bitmask**. We only proceed to the depth test for the pixels where the mask is "true."

---

### 2. Implementation Logic (Conceptual)

Using Intel Intrinsics (SSE2), which is supported on almost every modern x86 CPU, the inner loop transformation looks like this:

```cpp
#include <emmintrin.h> // SSE2 header

// Pre-calculate step increments for 4 pixels
__m128i stepX_w0 = _mm_set1_epi32(dy12 << subpixelBits);
__m128i stepX_w1 = _mm_set1_epi32(dy20 << subpixelBits);
__m128i stepX_w2 = _mm_set1_epi32(dy01 << subpixelBits);

// Create 4-pixel steps (0, 1, 2, 3)
__m128i xOffsets = _mm_set_epi32(3, 2, 1, 0); 

for (int y = minY; y <= maxY; y++) {
    // Load row start into SIMD registers
    __m128i vW0 = _mm_add_epi32(_mm_set1_epi32(w0_row), _mm_mullo_epi32(xOffsets, stepX_w0));
    __m128i vW1 = _mm_add_epi32(_mm_set1_epi32(w1_row), _mm_mullo_epi32(xOffsets, stepX_w1));
    __m128i vW2 = _mm_add_epi32(_mm_set1_epi32(w2_row), _mm_mullo_epi32(xOffsets, stepX_w2));

    for (int x = minX; x <= maxX; x += 4) {
        // Combined test: (W0 | W1 | W2)
        __m128i mask = _mm_or_si128(_mm_or_si128(vW0, vW1), vW2);
        
        // Check if any of the 4 pixels are inside (highest bit is 0 for positive)
        // Note: epi32 comparison against zero
        __m128i test = _mm_cmpgt_epi32(mask, _mm_set1_epi32(-1));
        int bitmask = _mm_movemask_ps(_mm_castsi128_ps(test));

        if (bitmask != 0) {
            // At least one pixel in this 4-pack is inside!
            // Process only the active pixels...
        }

        // Step 4 pixels forward
        vW0 = _mm_add_epi32(vW0, _mm_set1_epi32(dy12 << (subpixelBits + 2)));
        vW1 = _mm_add_epi32(vW1, _mm_set1_epi32(dy20 << (subpixelBits + 2)));
        vW2 = _mm_add_epi32(vW2, _mm_set1_epi32(dy01 << (subpixelBits + 2)));
    }
}

```

---

### 3. Updated Final Markdown Archive

This is the final version of the technical document. It now includes the **Sub-pixel** and **SIMD** optimization paths.

## Final Summary of the Pipeline

| Feature | Implementation | Purpose |
| --- | --- | --- |
| **Clipping** | Sutherland-Hodgman (Clip Space) | Prevent division by zero and garbage geometry. |
| **Rasterization** | Incremental Barycentric | Replaces slow multiplication with fast addition. |
| **Sub-pixel** | 4-bit Fixed Point | Eliminates gaps/sparkles between triangles. |
| **Optimization** | SIMD (SSE/AVX) | Processes 4-8 pixels per instruction. |
| **Display** | Raylib Blit | High-speed display of the final software buffer. |



### Final Piece of Advice

When you run this in Raylib, make sure to compile in **Release Mode** with optimizations turned on (`-O3` for GCC/Clang or `/O2` for MSVC). Software rasterizers are incredibly sensitive to compiler optimizations; a Release build can be 10x-50x faster than a Debug build.