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
12. Draw filled triangles.

### Visibility

14. Setting up a camera and perspective projection.
15. Moving the camera.
16. Frustum culling.
17. Back-face culling.

### Shaders and Light

18. Setting up basic fragment shaders.
19. Adding a light to the scene.
20. Gouraud shading.
21. Phong shading.
22. Texture mapping.

## House-keeping

- Look into making a separate `vec3` class. Inadvertently performing calculations on `w` is weird with `vec4`.
- Look into separating out loading the mesh and storing its data into a separate class.

## References

There are a few other projects that have done the same thing, and that I have used as references. They are as follows:

- https://github.com/JamesGriffin/3D-Rasterizer
- https://github.com/ssloy/tinyrenderer
- https://github.com/NotCamelCase/SoftLit

For learning SDL2, this website has been very handy:

- http://lazyfoo.net/tutorials/SDL/index.php
