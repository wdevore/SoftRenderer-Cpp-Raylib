# Debugging

In order to see a variable in the *Debug Console* we need to explicitly call
```-exec printf "%s", worldMatrix.toString().c_str()```

# Tasks
- Simple particle system
- Add more objects, for example Tetrahedra
- Add Sphere
- Add Cylindar
- Add Zbuffered lines (**done**)
- Add Arcball camera
- Add turntable camera (**done**)
- Add WASD to lookAt camera
- Make further optimizations
- Add Line collection (**done**)
- Add X,Z grid plane (**done**)
- Add 3D axis marker (**done**)
- Add Wu lines

What would you like to tackle next? We can look into optimizing the clipping with Cohen-Sutherland outcodes, adding anti-aliasing to make those grid lines look smoother, or move on to something entirely new!

# Useful resources

- Foundations of Game Engine Development (Eric Lengyel)
- Graphics Programming Black Book (Michael Abrash)
- Texture Mapping Technical Articles (Chris Hecker)
- Rasterization Rules & the Edge Function (ScratchAPixel.com)
- Rasterization Rules (Microsoft Direct3D)