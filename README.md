# What?

**3DSR** (short for 3D Software Rasterizer) is intended to be as the title implies, a simple 3D software rasterizer.

## Why?

The idea behind rendering 3D graphics boils down to this: we have a virtual scene, with cameras, and lights, etc. along with some models, that are made up of primitives, namely, triangles. Somehow, we want to draw these triangles, which are represented in 3D space, onto our 2D screen. How can we do that?

One such technique for doing so is called **rasterization**. Long story short, we project each triangle onto the screen, by using the corners of the triangle to determine which pixels it takes up.

Although this technique has its pros and cons, it has been the technique of choice for a long time. Hence, it is good practice to understand how to implement it from scratch, including all the math!

## Libraries

- **SDL**
To set up a window and view the results of the rendering in real-time. None of the visuals drawn to the screen will use SDL's functions for drawing to the screen, which would be cheating, since those would be accelerated by the GPU, and therefore, done using hardware. 

We will be using pure code to calculate and draw everything to a buffer, which will then, in turn, be rendered by SDL.

- **tinyobjloader**
To load in `.obj` files conveniently. Why? Our models that we are going to render have to be described somehow. That's where the `.obj` file format comes in. It allows us to easily encode a 3D model with text.

## Building and Running

In the cloned repo:

- `mkdir build && cd build`
Creates the `build` folder, which is not committed to the repo, since each user can generate their own build files.

- `cmake ..`
Uses the `CMakeLists.txt` file, which specifies a bunch of `CMake` rules to create a `Makefile`.

- `make`
Runs the `Makefile` to build the project.

To run the project, all you need to do is type `./3DSR` in the `build` folder.

## To-Do

### Basic Set-up
1. ~~Set up `CMakeLists.txt` to build the executable and link in **SDL2**.~~
2. ~~Figure out **tinyobjloader** and include it in the project.~~
Fun fact: you don't need to specify the exact path of where the header lives, as long as the directory is included using `target_include_directories` in your `CMakeLists.txt`.

3. ~~Create a window in **SDL2**.~~
4. ~~Figure out how to write to a buffer.~~
5. ~~Figure out how to display said buffer using **SDL2**.~~
6. ~~Create a basic vector class.~~
7. ~~Create a basic matrix class.~~
8. ~~Load `.obj` files.~~

### Primitive Assembly
9.  ~~Run through `tinyobjloader` returned file format, and act upon vertices (Step 10 and onwards).~~
10. ~~Draw line segments into the buffer, using Bresenham's line algorithm.~~
12. ~~Draw filled triangles using a basic rasterization algorithm.~~

### Visibility

13. ~~Occlusion culling using a z-buffer.~~
14. ~~Perspective projection.~~
15. ~~Back-face culling.~~

### Shaders and Light

18. ~~Texture mapping.~~
19. ~~Perspective-correct interpolated barycentric coordinates and depth.~~
20. Perspective-correct interpolated normals.
21. ~~Gouraud shading.~~ 
22. Phong shading.

### Usability

23. Add key bindings for rotating the camera.
24. Add key bindings for switching to different shaders. (add abstraction for shaders, possibly)
25. Add text describing model name, FPS and triangles. (inspired by JamesGriffin repo)
26. Add checkboard texture. (inspired by JamesGirffin repo)

## House-keeping

- ~~Look into making a separate `vec3` class. Inadvertently performing calculations on `w` is weird with `vec4`.~~
- Make vector API nicer. Add more QoL functions to other vec2 and vec4 classes (i.e normalize, magnitude, etc.). Possibly add stuff like swizzling.
- Look into why using `vec2` with floats instead of integers causes gaps between triangles. (may have to do with weird rounding for bounding boxes)
- Possibly abstract out `SDL_MakeRGBA` into a color class.
- Possibly abstract out renderer (i.e `draw_line`, `draw_triangle`)
- Look into separating out loading the mesh and storing its data into a separate class.
- Look into creating an interface for shaders.
- Look into a better interface to load models with.
- Displaying FPS.
- Optimization.
- Clean up code. A lot.

## Maybe's

- Frustum culling. (not really necessary since we only have 1 model at a time)
- Frustum clipping. (i.e clipping before entering the rasterization algorithm)
- Mipmaps.
- Shadow mapping.

## Lessons

- Be wary of where your cross product vector points. Place your thumb in the direction of the first vector and your index finger in the direction of the second vector. Your middle finger should be perpendicular and show the direction of the cross product vector. Remember to do this with the appropriate handedness of your coordinate system (left-hand/right-hand).
- Be careful of using `std::numeric_limits<float>::min()` since this will give you the lowest possible POSITIVE value. Use `std::numeric_limits<float>::lowest()` instead.
- Be careful with loading data. For example, with `tinyobjloader`: make sure you're indexing into `attrib.normals` with the `normal_index` to get the normals.

## References

There are a few other projects that have done the same thing, and that I have used as references. They are as follows:

- https://github.com/JamesGriffin/3D-Rasterizer
- https://github.com/ssloy/tinyrenderer
- https://github.com/NotCamelCase/SoftLit

For learning SDL2, this website has been very handy:

- http://lazyfoo.net/tutorials/SDL/index.php

For a deeper overview into the basic rasterization algorithm:

- https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
- https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage

About barycentric coordinates:
- https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates

About homogeneous coordinates:
- http://www.songho.ca/math/homogeneous/homogeneous.html

About the coordinate transform pipeline:
- https://www.youtube.com/watch?v=jmRkYDFDIp0&list=PLbMVogVj5nJSyt80VRXYC-YrAvQuUb6dh&index=19 (awesome series of videos, but be warned, his math is possibly incorrect)
- http://www.songho.ca/opengl/gl_transform.html#projection (overview)
- http://math.hws.edu/graphicsbook/c3/s3.html (overview)
- https://computergraphics.stackexchange.com/questions/1769/world-coordinates-normalised-device-coordinates-and-device-coordinates (about all the different coordinates)
- http://www.songho.ca/opengl/gl_camera.html (construction of openGL view matrix)
- http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective (construction of openGL projection matrix)
- https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/projection-stage (about projection)
- https://paroj.github.io/gltut/Positioning/Tut04%20Perspective%20Projection.html (about projection)
- http://www.codinglabs.net/article_world_view_projection_matrix.aspx (constructing projection matrix at the bottom, using fov)
- https://computergraphics.stackexchange.com/questions/6271/what-is-the-use-of-homogenous-divide (very good question and answer)
