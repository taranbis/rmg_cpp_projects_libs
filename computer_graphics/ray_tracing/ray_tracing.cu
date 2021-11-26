#include <fstream>
#include <iostream>
#include <math.h>

#include <cuda.h>
#include <cuda_runtime.h>

#include "ray.hpp"
#include "vec3.hpp"

#define checkCudaErrors(val) check_cuda((val), #val, __FILE__, __LINE__)
void check_cuda(cudaError_t result, char const* const func, const char* const file, int const line)
{
    if (result) {
        std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << "at " << file << ": line = " << line
                  << ", func = "
                  << "\n";
    }
}

__global__ void render_image(vec3* fb, int max_x, int max_y)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;

    if ((i >= max_x) || (j >= max_y)) return;

    // int pixel_index      = j * max_x * 3 + i * 3;
    // fb[pixel_index]     = float(i) / max_x;
    // fb[pixel_index + 1] = float(j) / max_y;
    // fb[pixel_index + 2] = 0.2;
}

__device__ float hit_sphere(const vec3& centre, float radius, const ray& r)
{
    vec3  oc = r.origin() - centre;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return -1.0f;
    } else {
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }
}

__device__ vec3 color(const ray& r)
{
    float t = hit_sphere(vec3(0, 0, -1), 0.5f, r);
    if (t > 0.0f) {
        vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1));
        return 0.5f * vec3(N.x() + 1, N.y() + 1, N.z() + 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5f * (unit_direction.y() + 1.0f);
    return (1.0f - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

__global__ void render(vec3* fb, int max_x, int max_y, vec3 lower_left_corner, vec3 horizontal, vec3 vertical,
                       vec3 origin)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= max_x) || (j >= max_y)) { return; }

    int   pixel_index = j * max_x + i;
    float u = float(i) / float(max_x);
    float v = float(j) / float(max_y);
    ray   r(origin, lower_left_corner + u * horizontal + v * vertical);
    fb[pixel_index] = color(r);
}

void print_image()
{
    const int nx = 200;
    const int ny = 100;
    // output as .ppm image
    std::ofstream fout("out.ppm", std::ios_base::out | std::ios_base::binary);
    fout << "P6" << std::endl << nx << ' ' << ny << std::endl << "255" << std::endl;
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
            int  ir = int(255.99f * col[0]);
            int  ig = int(255.99f * col[1]);
            int  ib = int(255.99f * col[2]);

            fout << ir << " " << ig << " " << ib << "\n";
        }
    }
}

int main()
{
    int    nx = 1024, ny = 512;
    int    num_pixels = nx * ny;
    size_t fb_size = num_pixels * sizeof(vec3);

    vec3* fb;
    checkCudaErrors(cudaMallocManaged((void**)&fb, fb_size));

    int  tx = 8, ty = 8;
    dim3 blocks(nx / tx, ny / ty);
    dim3 threads(tx, ty);

    cudaEvent_t start, stop;
    float       time;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    /**
     * \note Using “0” in this argument means that the default stream object
     * is used for synchronization.
     */
    cudaEventRecord(start, 0);

    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);

    render<<<blocks, threads>>>(fb, nx, ny, lower_left_corner, horizontal, vertical, origin);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    /**
     * \note time value is expressed as milliseconds
     *  and it is accurate to about 0.5 microseconds
     */
    cudaEventElapsedTime(&time, start, stop);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    checkCudaErrors(cudaGetLastError());

    /**
     * \note \fn cudaDeviceSynchronize lets the CPU know when the
     */
    checkCudaErrors(cudaDeviceSynchronize());

    // for (int j = ny - 1; j >= 0; j--)
    // {
    //     for (int i = 0; i < nx; i++)
    //     {
    //         size_t pixel_index = j * nx + i;
    //         std::cout << "fb[" << pixel_index << "]= " << fb[pixel_index]
    //                   << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // output as .ppm image
    std::ofstream fout("out.ppm", std::ios_base::out | std::ios_base::binary);
    fout << "P6" << std::endl << nx << ' ' << ny << std::endl << "255" << std::endl;
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            size_t pixel_index = j * nx + i;
            int    ir = int(255.99 * fb[pixel_index].r());
            int    ig = int(255.99 * fb[pixel_index].g());
            int    ib = int(255.99 * fb[pixel_index].b());
            fout << (char)ir << (char)ig << (char)ib;
        }
    }
    fout.close();

    checkCudaErrors(cudaFree(fb));
    std::cout << "time: " << time << std::endl;
    return 0;
}

// int main()
// {
//     vec3* fb = new vec3(3, 4, 0);
//     // fb->x(1);
//     // fb->y(2);
//     // fb->z(3);

//     fb->make_unit_vector();

//     std::cout << *fb;
//     return 0;
// }