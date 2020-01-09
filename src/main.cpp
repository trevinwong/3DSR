#include <iostream>
#include <limits>
#include <utility>
#include <cmath>
#include <iomanip>
#include <cstdint>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../lib/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "frame.h"

#define trace(var)  { std::cout << "Line " << __LINE__ << ": " << #var << "=" << var << "\n";}

#include "SDL.h"

// TODO: Refactor this later. Assumed to be RGBA
struct Texture
{
    int width, height, channels;
    unsigned char* data;
};

const inline int WINDOW_WIDTH = 800;
const inline int WINDOW_HEIGHT = 800;
const inline SDL_PixelFormat* pixel_format;

bool quit = false;


void draw_line(int x0, int y0, int x1, int y1, Frame& frame)
{
    int x = x0;
    int y = y0;
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    bool steep = false;

    if (dy > dx)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        std::swap(x, y);
        std::swap(dx, dy);
        steep = true;
    }

    int P = 2*dy - dx;

    bool decrement_x = (x0 > x1);
    bool decrement_y = (y0 > y1);

    while ((decrement_x) ? x >= x1 : x <= x1)
    {
        if (steep)
        {
            frame.set_pixel(y, x, 0xffff0000);
        } else
        {
            frame.set_pixel(x, y, 0xffff0000); }
        if (P < 0)
        {
            P = P + 2*dy;
        } else
        {
            P = P + 2*dy - 2*dx;
            if (decrement_y)
            {
                --y;
            } else
            {
                ++y;
            }
        }
        
        if (decrement_x)
        {
            --x;
        } else
        {
            ++x;
        }
    }
}

void draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame)
{
    draw_line(v0.x, v0.y, v1.x, v1.y, frame);
    draw_line(v1.x, v1.y, v2.x, v2.y, frame);
    draw_line(v2.x, v2.y, v0.x, v0.y, frame);
}

// We are given two vectors with z = 0, and want to calculate their cross product.
// This is equivalent to the 2x2 determinant formed by the vectors created by their x and y components.
// This is also equivalent to the area of the parallelogram formed by the two vectors.
// This is also equivalent to comparing b to the edge function of a.
float cross_2d(const vec2& a, const vec2& b)
{
    return (a.x * b.y) - (a.y * b.x);
}

int min3(int a, int b, int c)
{
    return std::min(std::min(a, b), c);
}

int max3(int a, int b, int c)
{
    return std::max(std::max(a, b), c);
}

void print_hex(uint32_t num)
{
    std::cout << std::setfill('0') << std::setw(8) << std::hex << num << '\n';
}

// Assume p0, p1 and p2 are listed in CCW order, and we're using a right-hand coordinate system.
// TODO: Clean up parameters.
void draw_triangle(vec3& p0, vec3& p1, vec3& p2, Frame& frame, float* z_buffer, vec2& t0, vec2& t1, vec2& t2, Texture& texture, std::vector<float> intensities, uint32_t color)
{
    /* p0.print(); */
    /* p1.print(); */
    /* p2.print(); */

    vec2 edge0 = (p1 - p0);
    vec2 edge1 = (p2 - p1);
    vec2 edge2 = (p0 - p2);

    // Get the bounding box of these points.
    int min_x = min3(p0.x, p1.x, p2.x);
    int max_x = max3(p0.x, p1.x, p2.x);
    int min_y = min3(p0.y, p1.y, p2.y);
    int max_y = max3(p0.y, p1.y, p2.y);

    // Clip against the screen.
    min_x = std::max(min_x, 0);
    max_x = std::min(max_x, frame.w - 1);
    min_y = std::max(min_y, 0);
    max_y = std::min(max_y, frame.h - 1);

    // Iterate through all pixels inside the bounding box.
    for (int i = min_x; i < max_x; i++)
    {
        for (int j = min_y; j < max_y; j++)
        {
            vec2 to_point_from_p0 = vec2(i, j) - vec2(p0);
            vec2 to_point_from_p1 = vec2(i, j) - vec2(p1);
            vec2 to_point_from_p2 = vec2(i, j) - vec2(p2);

            /*
            edge0.print();
            to_point_from_p0.print();

            edge1.print();
            to_point_from_p1.print();

            edge2.print();
            to_point_from_p2.print();
            */
            float v0v1p = cross_2d(edge0, to_point_from_p0); 
            float v1v2p = cross_2d(edge1, to_point_from_p1); 
            float v2v0p = cross_2d(edge2, to_point_from_p2); 
            // std::cout << " v0v1p: " << v0v1p << " v1v2p: " << v1v2p << " v2v0p: " <<  v2v0p << std::endl;

            if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
            {
                float v0v1v2 = cross_2d(p1 - p0, p2 - p0);

                if (v0v1v2 == 0) return;

                // Calculate barycentric coordinates. We can leave out the division by 2 since we're just getting the ratio between the sub-triangle and the triangle.
                // TODO: Be careful about dividing by 0.
                // aka check for degenerate triangles first?
                float w0 = v0v1p / v0v1v2; // corresponds to the weight of p2
                float w1 = v1v2p / v0v1v2; // corresponds to the weight of p0
                float w2 = v2v0p / v0v1v2; // corresponds to the weight of p1

                // Get z value by interpolating from each vertice using the barycentric coordinates.
                float z = 0;
                z += (w0 * p2.z);
                z += (w1 * p0.z);
                z += (w2 * p1.z);

                // Interpolate the value for u, v as well.
                // TODO: should we have a specific syntax for zero initializing vectors?
                vec2 uv;
                /* t2.print(); */
                /* t0.print(); */
                /* t1.print(); */
                /* std::cout << "w0: " << w0 << " w1: " << w1 << " w2: " << w2 << std::endl; */
                uv += (t2 * w0);
                uv += (t0 * w1);
                uv += (t1 * w2);

                // Interpolate the intensity of the color.
                float intensity = 0;
                intensity += (intensities[2] * w0);
                intensity += (intensities[0] * w1);
                intensity += (intensities[1] * w2);
                intensity = std::max(intensity, 0.0f);
                std::cout << "intensity: " << intensity << std::endl;

                // Pass in the texture.
                // The u,v coordinates HAVE to be floored before indexing the texture color with them!
                uv.x = std::floor(uv.x * texture.width);
                uv.y = std::floor(uv.y * texture.height);
 
                // Sample the color at uv.x and uv.y.
                // Nice way of sampling borrowed from NotCamelCase/SoftLit.
                // TODO: Abstract getting texture color (won't always be in RGB fashion)
                // TODO: Texture seems a little redder than it should be. Investigate
                int idx = ((uv.y * texture.width) + uv.x) * texture.channels;
                float r = (float) texture.data[idx++] * intensity;
                float b = (float) texture.data[idx++] * intensity;
                float g = (float) texture.data[idx++] * intensity;
                uint32_t texture_color = SDL_MapRGBA(pixel_format, r, g, b, 255);
                uint32_t gouraud_color = SDL_MapRGBA(pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);

                // If the z-value of the pixel we're on is greater than our current stored z-buffer's value...
                if (z < z_buffer[i + (j * frame.w)])
                {
                    // Replace it and color that pixel in.
                    z_buffer[i + (j * frame.w)] = z;
                    frame.set_pixel(i, j, texture_color);
                }
            }
        }
    }
}

// Constructs a view matrix given the position of the eye and the target.
// Use the unit vector pointing up as our temporary up vector, if none is specified.
mat4 lookAt(vec3 eye, vec3 target, vec3 up = vec3(0, 1, 0))
{
    // Translate eye position back to origin.
    // Our calculations assume that our eye is positioned at the origin and looks down the -Z axis.
    mat4 mT         (   1, 0, 0, -eye.x,
                        0, 1, 0, -eye.y,
                        0, 0, 1, -eye.z,
                        0, 0, 0, 1
                    );

    vec3 forward = eye - target;
    forward.normalize_inplace();

    // Be very careful with the order of these cross products.
    vec3 left = cross(up, forward);
    left.normalize_inplace();

    // Re-calculate our up vector using our forward and left vector.
    up = cross(forward, left);

    mat4 mR         (   left.x, up.x, forward.x, 0,
                        left.y, up.y, forward.y, 0,
                        left.z, up.z, forward.z, 0,
                        0, 0, 0, 1
                    );

    // Inverse of mR is equal to the transpose since all basis vectors are orthonormal.
    // Why do we want to take the inverse? This is the matrix that gives us the rotation of the camera. We want to unrotate it.
    // Since we've calculated the left vector, unrotating it will give us a camera facing down the -Z axis!!
    mat4 mR_transpose = transpose(mR); 
    mat4 mView = mR_transpose * mT;

    return mView;
}

int main() {
    // Initialize event var. to store events when unqueueing them.
    SDL_Event event;

    // Initialize SDL (we only care about the video subsystem)
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return 1;

    SDL_Window* window = SDL_CreateWindow("3DSR",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    WINDOW_WIDTH, WINDOW_HEIGHT,
                    SDL_WINDOW_SHOWN);

    // Attach an SDL_Renderer to the window.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Creates the "screen" as a texture which we will update with the frame buffer we will draw to using the CPU.
    // Each pixel will be a uint32. 
    // The most significant 8 bytes will be the alpha, then red, then green, then blue, as specified by the pixel format below.
    SDL_Texture* screen = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING,
                        WINDOW_WIDTH, WINDOW_HEIGHT);
    pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    
    // Create a Frame object, which allocates a buffer to draw to.
    Frame frame(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Load our object.
    // monkey_flat.obj only specifies vertices, normals and faces.
    std::string inputfile = "obj/african_head.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;  

    // LoadObj now only passes in 1 std::string for error handling for release v1.0.6.
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) {
      std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
      exit(1);
    }

    // Create a light.
    // TODO: The light distance should affect the intensity of the shading...
    vec3 light_dir(0.3, 0, 0.7);
    light_dir.normalize_inplace();

    // Create an eye.
    vec3 eye(0, 0, 2);
    
    float t = 1.0f;
    float b = -t;
    float r = 1.0f;
    float l = -r;
    float n = 1.8f;
    float f = 10.0f;

    // Here's how the pipeline goes:
    // Local -> world -> eye -> clip -> screen -> viewport
    // We don't need to transform our local coordinates into our world coordinates since we have not transformed them in any way.                 
   
    // Construct our model matrix.
    // (i.e should contain any of the transformations we make to the model in the world, which right now is none)
    mat4 model      (   1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1
                    );
    
    // Point our eye towards the origin and construct a view matrix from that to convert to eye coords from world coords.
    mat4 view = lookAt(eye, vec3(0, 0, 0)); 

    // The math is from: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
    // NOTE: this assumes n and f are both positive!!!
    // NOTE: this transforms the coordinates into NDCS coordinates, which FLIPS the direction of our z-axis, essentially
    // Our camera typically looks down the negative z-axis but after transforming the coordinates it looks down the positive z-axis.
    // This means your z-buffer implementation needs to change: closer coordinates should be SMALLER, not LARGER
    // Construct the perspective matrix.
    mat4 perspective(   2*n/(r-l), 0, (r+l)/(r-l), 0,
                        0, 2*n/(t-b), (t+b)/(t-b), 0,
                        0, 0, -(f + n)/(f - n), -2*(f * n)/(f - n),
                        0, 0, -1, 0
                    );

    // Construct the viewport matrix, which comprises of multiple steps.
    // Remember, our NDCS coords range from -1 to 1. We want them to range from 0 to 1.
    // So we do this: (coords + 1), so it ranges from 0 to 2, and then divide by 2, which gets us to our desired range.
    // Finally, we multiply by our width and height of our viewport!
    // It all looks like this, using x as an example: width * (x + 1)/2 -> width*x/2 + width/2, which is exactly what's happening here
    int d = 255;
    mat4 viewport   (   frame.w/2, 0, 0, (frame.w)/2,
                        0, frame.h/2, 0, (frame.h)/2,
                        0, 0, d/2, d/2,
                        0, 0, 0, 1
                    );

    // Model-view projection matrix. Note that we don't transform the object from local coords to world coords, so there is no model matrix needed!
    mat4 mvp = perspective * view * model;

    // Load our texture.
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("img/african_head_diffuse.tga", &width, &height, &channels, 0);
    if (data == nullptr)
    {
        // Just crash if we can't load the texture.
        // TODO: Refactor this.
        std::cout << "Unable to load texture." << std::endl;
        return 1;
    }
    Texture texture;
    texture.width = width;
    texture.height = height;
    texture.channels = channels;
    texture.data = data;

    // Time to draw the mesh. 
    // Let's allocate our z-buffer.
    float* z_buffer = new float[frame.w * frame.h];
    for (int i = 0; i < frame.w * frame.h; i++)
    {
        z_buffer[i] = std::numeric_limits<float>::max();
    }

    // For each shape...
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // For each face...
        size_t f_index_begin = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // Find the number of vertices that describes it. 
            int num_vertices = shapes[s].mesh.num_face_vertices[f];

            std::vector<vec3> world_coords;
            std::vector<vec3> viewport_coords;
            std::vector<vec2> texture_coords;
            std::vector<vec3> normals;
            
            // All vertices are stored in counter-clockwise order by default.
            for (size_t v = 0; v < num_vertices; v++)
            {
                // Fetch the index of the 1st/2nd/3rd... etc. vertex
                // This index is NOT the vertex index, but rather the index in mesh.indices.
                tinyobj::index_t idx = shapes[s].mesh.indices[f_index_begin + v]; 
                // We want to loop back to the first vertex if we are at the last vertex.
                tinyobj::index_t next_idx = shapes[s].mesh.indices[f_index_begin + (v+1)%num_vertices];

                // We can obtain the vertex index by calling idx.vertex_index.
                // All vertices are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index + 2];

                world_coords.push_back(vec3(vx, vy, vz));

                /* std::cout << std::endl; */
                /* std::cout << "original coords" << std::endl; */
                /* vec4(vx, vy, vz, 1).print(); */

                /* vec4 view_coords = view * vec4(vx, vy, vz, 1); */
                /* std::cout << "view coords" << std::endl; */
                /* view_coords.print(); */

                /* vec4 clip_coords = perspective * view_coords; */
                /* std::cout << "clip coords" << std::endl; */
                /* clip_coords.print(); */

                /* vec4 screen_coords = clip_coords / clip_coords.w; */
                /* std::cout << "screen coords" << std::endl; */
                /* screen_coords.print(); */

                vec4 clip = mvp * vec4(vx, vy, vz, 1);
                // TODO: Do some clipping here. Not sure how to reconstruct clipped triangles just yet.
                // CHECK IF -w < x < w and -w < y < w because afer dividing by w, -1 < x < 1, same with y
                // If we throw away the vertex what do we do? where do we reconstruct the vertex?
                vec4 screen = clip / clip.w; // Perspective divide.

                // TODO: Why is z not between -1 and 1??
                // Big think: maybe the z thats not between -1 and 1 is clipped out? or should be clipped out, but we don't do it?
                // Somehow offscreen vertices are being clipped out at a later process - likely at the bounds checking during rasterization
                
                vec3 viewport_coord = (viewport * screen);
                // Need to convert to int otherwise weird black gaps will appear between triangles
                viewport_coord.x = (int) viewport_coord.x;
                viewport_coord.y = (int) viewport_coord.y;
                viewport_coords.push_back(viewport_coord);

                // We can obtain the vertex index by calling idx.normal_index.
                // All normals are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t nx = attrib.vertices[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.vertices[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.vertices[3*idx.normal_index + 2];
                normals.push_back(vec3(nx, ny, nz));

                // Textures.
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                texture_coords.push_back(vec2(tx, ty));
            }

            // Calculate the normal of the face.
            vec3 normal = cross(world_coords[1] - world_coords[0], world_coords[2] - world_coords[0]);
            normal.normalize_inplace();

            // Create the viewing vector - using one of the vertices of the polygon, create a vector from the eye's position to it.
            // Note that our eye is in world space and we're using the world space version of the vertex.
            vec3 viewing = world_coords[0] - eye;

            float facing = dot(normal, viewing);
            // If facing is positive, that means the polygon is facing the same direction as the viewing vector, aka, AWAY
            if (facing < 0)
            {
                // Calculate the intensities of each vertex by using the normals.
                std::vector<float> intensities;

                std::cout << "normal 0 before normalize" << std::endl;
                normals[0].print();
                std::cout << "normal 1 before normalize" << std::endl;
                normals[1].print();
                std::cout << "normal 2 before normalize" << std::endl;
                normals[2].print();

                normals[0].normalize_inplace();
                normals[1].normalize_inplace();
                normals[2].normalize_inplace();

                std::cout << "normal 0 after normalize" << std::endl;
                normals[0].print();
                std::cout << "normal 1 after normalize" << std::endl;
                normals[1].print();
                std::cout << "normal 2 after normalize" << std::endl;
                normals[2].print();

                float intensity0 = dot(normals[0], light_dir);
                float intensity1 = dot(normals[1], light_dir);
                float intensity2 = dot(normals[2], light_dir);
                std::cout << "intensity 0: " << intensity0 << std::endl;
                std::cout << "intensity 1: " << intensity1 << std::endl;
                std::cout << "intensity 2: " << intensity2 << std::endl;

                intensities.push_back(intensity0);
                intensities.push_back(intensity1);
                intensities.push_back(intensity2);

                float intensity = dot(normal, light_dir);
                uint32_t color = SDL_MapRGBA(pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);
                
                // draw_line(viewport_coords[0], viewport_coords[1], viewport_coords[2], frame);
                draw_triangle(viewport_coords[0], viewport_coords[1], viewport_coords[2], frame, z_buffer, texture_coords[0], texture_coords[1], texture_coords[2], texture, intensities, color);
            }   
           
            // The index at which each face begins in mesh.indices.
            // Remember that each face could have a variable number of vertices (3, 4, 5, etc.), so the index at which a face begins must be adjusted
            // each time for each face that we process.
            f_index_begin += num_vertices;
        }
    }

    frame.flip_image_on_x_axis();
    
    // Main loop.
    while (!quit)
    {
        // Event loop.
        while (SDL_PollEvent(&event) != 0)
        {
            quit = (event.type == SDL_QUIT);
        }

        // Replace the screen with our frame buffer.
        // The last argument is the number of bytes between one row and the next.
        SDL_UpdateTexture(screen, NULL, frame.buffer, WINDOW_WIDTH * sizeof(uint32_t));

        // Render.
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

//    delete z_buffer; // deallocate z-buffer
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
