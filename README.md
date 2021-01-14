# What?

**3DSR** (short for 3D Software Rasterizer) is intended to be, as the title implies, a renderer that uses the rasterization technique, completely implemented in C++.

## Libraries

- **tinyobjloader**
- **SDL**
To set up a window and view the results of the rendering in real-time. None of the visuals drawn to the screen will use SDL's functions for drawing to the screen.


## Building and Running (Linux)

In the cloned repo:

- `mkdir build && cd build`
- `cmake ..`
- `make`

Finally, use `./3DSR` in the `build` folder to run it.

## Wishlist

- Frustum culling
- Frustum clipping and primitive reconstruction
- Mipmaps
- Shadow mapping
- FPS

## Lessons

- Be careful of using `std::numeric_limits<float>::min()` since this will give you the lowest possible POSITIVE value. Use `std::numeric_limits<float>::lowest()` instead.
- Be careful with loading data. For example, with `tinyobjloader`: make sure you're indexing into `attrib.normals` with the `normal_index` to get the normals.

## References

- https://github.com/JamesGriffin/3D-Rasterizer
- https://github.com/ssloy/tinyrenderer
- https://github.com/NotCamelCase/SoftLit
- https://github.com/karltechno/SoftRast

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
