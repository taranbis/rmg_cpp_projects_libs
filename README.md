This repo contains a series of projects that I wanted to implement for learning or using or just for fun(e.g. game automation), in C++. Although these are all different projects, I considered keeping them all of them in this repo as it easier to maintain. They can be built separately by configuring the perameters in CMakeLists.txt of the root path.

### TODO
- [x] util.hpp has to be fixed
- [x] decode_jpg has to be fixed
- [x] IoT project has to be fixed
- [x] move allocator to allocator projects
- [ ] Fast2048 needs to be finished and polished
- [ ] Fourier Transform Visualization & Drawing needs to be finished & polished
- [ ] remove unneccessary files in CFD simulation
- [ ] finish ray_tracing using cuda or look if it is ok


## [Gaming](./gaming)
- [ ] [Asteroids](./gaming/asteroids): game implemented using SFML <mark>**in progress**</mark>

One can also find here games that I have automated: 
- [ ] [2048](./gaming/fast2048), using OpenCV & X11 for screenshots and for simulating key presses
    - Monte Carlo Simulation on 2048 game
- [ ] [Flappy Bird](./gaming/game_automation/flappy_bird), using OpenCV & X11 for screenshots 
- [ ] [Pong](./gaming/game_automation/pong), using OpenCV & ADB to use the game on a mobile phone


## [Computational Science and Engineering (CSE) Algorithms](./cse_algorithms)
each important lecture of my Master's degree will have an implementation here:
- [x] [**Fluid Simulation**](./cse_algorithms/CFD), discretizing Navier-Stokes equations and rendering using SDL2 library
- [x] [**Fourier Transform Visualization**](./cse_algorithms/fourier_transform) using SFML
- [ ] [**Fourier Transform Drawing usind Epicycles**](./cse_algorithms/fourier_transform)  <mark>**in progress**</mark>
- [ ] **Planet Simulation**, 
- [x] [**Bezier Functions**](./cse_algorithms/bezier_functions) using SDL for rendering
- [ ] make solvers for each iterative method; [Example](https://math.nist.gov/iml++/)
- [ ] FD scheme on Poisson Equation
- [ ] N-body simulation (Scientific Computing II)
- [ ] Parallel LU Decomposition (Parallel Numerics)

## [Util](./util)
- [x] [CSV parser](./util/csv): implementation of a CSV parser. i needed it mainly when working with data for DL.

### [Networking](./util/network)
- [x] implement tcp connection manager (multiple connections, client, server)
- [x] implement a basic http web server & basic client <mark>**in progress**</mark>
    - still to implement: better parsing and addition of request/response headers
- [ ] implement WebSocket server & WS client <mark>**in progress**</mark>


## [IoT Project](./IoT_project)
It consists of two programs written in C++: 
- a simple random location data generator: it tries to simulate random position data (including noise) from a forklift
- location data processor which computes using the Kalman filter for processing data the speed and the total distance travelled using data received from the generator using MQTT broker.
The two programs communicate via MQTT protocol.


## [Image Processing](./img_processing_library)
- [ ] [Fourier Transform](./img_processing_library/fourier_transform) for eliminating noise in an image by converting it to frequency domain and back to spatial domain
- [ ] JPEG decoder <mark>**in progress**</mark>
- [ ] PNG decoder <mark>**in progress**</mark>


## [Deep Learning](./deep_learning)
- [ ] Image Segmentation using PyTorch
- [ ] Image classification projects
- [ ] Market Prediction

Resources:
- [Data Preprocessing And Visualization In C++](https://towardsdatascience.com/data-preprocessing-and-visualization-in-c-6d97ed236f3b)
- [Machine Learning Libraries for C++](https://www.analyticsvidhya.com/blog/2020/04/machine-learning-using-c-linear-logistic-regression/?utm_source=blog&utm_medium=introduction-machine-learning-libraries-c)


## [Computer Graphics](./computer_graphics)
- [ ] [Algorithms Visualization, e.g. Bubble Sort](./computer_graphics/algorithms) using SFML
- [ ] [ray tracing](./computer_graphics/ray_tracing) using Cuda <mark>**in progress**</mark>
- [ ] Triangle Rasterization: [Triangle Rasterization Paper](https://joshbeam.com/articles/triangle_rasterization/)


## [Allocator](./allocator)
implementation of different allocators and benchmarks for std::pmr

- [ ] implement Bucketizer, FreeList 
- [x] implement PoolAllocator, MemoryPool and Segregator 
- [x] Tracking Allocator, Mallocator

Resources:
Andrei Alexandrescu's talk at CppCon 2015: "std::allocator Is to Allocation what std::vector Is to Vexation"
- [Video](https://www.youtube.com/watch?v=LIb3L4vKZ7U&t=1991s&ab_channel=CppCon)
- [Presentation](https://github.com/CppCon/CppCon2015/blob/master/Presentations/allocator%20Is%20to%20Allocation%20what%20vector%20Is%20to%20Vexation/allocator%20Is%20to%20Allocation%20what%20vector%20Is%20to%20Vexation%20-%20Andrei%20Alexandrescu%20-%20CppCon%202015.pdf)


## Projects considered as well:
- [ ] Implementation of a profiling framework using instrumentation
An implementation similar to what I am looking for is [easy_profiler](https://github.com/yse/easy_profiler) + some important files from there:
    - [profiler.cpp](https://github.com/yse/easy_profiler/blob/develop/easy_profiler_core/profiler.cpp)
    - [current_time.h](https://github.com/yse/easy_profiler/blob/develop/easy_profiler_core/current_time.h)
- [ ] C++14 multiple-producer-multiple-consumer lockless queues based on circular buffer with std::atomic. Similar implementation [here](https://github.com/max0x7ba/atomic_queue)
- [ ] C++ LRU in-memory cache implementation. Idea from [here](https://github.com/warxander/lruc)

### Data Structures
Implementation of different helpful data structures:
- [ ] *fast parallel HashMap* from CppCon 2017 presentation of Matt Kulukundis, 
- [ ] Union Find
- [ ] Prefix Tree
- [ ] Priority Queue 

#### Useful Documentation:
- [Modern C++ features: C++20/17/14/11](https://github.com/AnthonyCalandra/modern-cpp-features)
- [Cpp Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Guidelines Support Library](https://github.com/microsoft/GSL)

### Commit messages:
- **feat**: (new feature for the user, not a new feature for build script)
- **fix**: (bug fix for the user, not a fix to a build script)
- **docs**: (changes to the documentation)
- **style**: (formatting, missing semi colons, etc; no production code change)
- **refactor**: (refactoring production code, eg. renaming a variable)
- **test**: (adding missing tests, refactoring tests; no production code change)
- **chore**: (updating grunt tasks etc; no production code change)

#### Development
- CMake > 3.8
- C++20 (concepts, spaceship operator(<=>) etc. are used)
