### Commit messages:
- **feat**: (new feature for the user, not a new feature for build script)
- **fix**: (bug fix for the user, not a fix to a build script)
- **docs**: (changes to the documentation)
- **style**: (formatting, missing semi colons, etc; no production code change)
- **refactor**: (refactoring production code, eg. renaming a variable)
- **test**: (adding missing tests, refactoring tests; no production code change)
- **chore**: (updating grunt tasks etc; no production code change)

### TODO
- [x] util.h has to be fixed
- [x] decode_jpg has to be fixed
- [x] IoT project has to be fixed
- [x] move allocator to allocator projects
- [ ] remove unneccessary files in CFD simulation
- [ ] finish ray_tracing using cuda or look if it is ok


## Deep Learning

- [ ] Image Segmentation using PyTorch
- [ ] image classification projects

- [Data Preprocessing And Visualization In C++](https://towardsdatascience.com/data-preprocessing-and-visualization-in-c-6d97ed236f3b)

- [Machine Learning Libraries for C++](https://www.analyticsvidhya.com/blog/2020/04/machine-learning-using-c-linear-logistic-regression/?utm_source=blog&utm_medium=introduction-machine-learning-libraries-c)


## Computational Science and Engineering (CSE) Algorithms
Each important lecture of my Master's degree will have an implementation here

- [x] **Fluid Simulation**, discretizing Navier-Stokes equations and rendering
- [ ] **Planet Simulation**, 
- [ ] make solvers for each iterative method; [Example](https://math.nist.gov/iml++/)
- [ ] FD scheme on Poisson Equation
- [ ] N-body simulation (Scientific Computing II)
- [ ] Parallel LU Decomposition (Parallel Numerics)

### Computer Graphics

#### Path Tracer Algorithm

- [ ] ray tracing using Cuda (in progress); found in [ray_tracing](ray_tracing)
- [ ] Triangle Rasterization; found in [TriangleRasterization](TriangleRasterization): [Triangle Rasterization Paper](https://joshbeam.com/articles/triangle_rasterization/)

## Allocator

- [ ] implement Bucketizer, FreeList 
- [x] implement PoolAllocator, MemoryPool and Segregator 
- [ ] implement a common complex allocator to be used by all applications

implemented mainly from Andrei Alexandrescu's talk at CppCon 2015: std::allocator Is to Allocation what std::vector Is to Vexation:

- [Video](https://www.youtube.com/watch?v=LIb3L4vKZ7U&t=1991s&ab_channel=CppCon)
- [Presentation](https://github.com/CppCon/CppCon2015/blob/master/Presentations/allocator%20Is%20to%20Allocation%20what%20vector%20Is%20to%20Vexation/allocator%20Is%20to%20Allocation%20what%20vector%20Is%20to%20Vexation%20-%20Andrei%20Alexandrescu%20-%20CppCon%202015.pdf)

## Data Structures

**Most important**: 

- [ ] *fast parallel HashMap* from CppCon 2017 presentation of Matt Kulukundis
- [ ] Union Find, Prefix Tree, Priority Queue 

found in [FastHashMap](FastHashMap)

## Projects considered as well:

- [ ] C++14 multiple-producer-multiple-consumer lockless queues based on circular buffer with std::atomic. Similar implementation [here](https://github.com/max0x7ba/atomic_queue)
- [ ] Monte Carlo Simulation on 2048 game
- [ ] C++ LRU in-memory cache implementation. Idea from [here](https://github.com/warxander/lruc)
- [ ] Create a basic web server (dedicated old machine). Uses: a cloud storage unit by adding a few hard drives

- [ ] Implementation of a profiling framework using instrumentation

I would have a lot to learn of profiling and how functions are called and how to call run-time instrumentation so it is worth a shot.

An implementation similar to what I am looking for is [easy_profiler](https://github.com/yse/easy_profiler) + some important files from there:
- [profiler.cpp](https://github.com/yse/easy_profiler/blob/develop/easy_profiler_core/profiler.cpp)
- [current_time.h](https://github.com/yse/easy_profiler/blob/develop/easy_profiler_core/current_time.h)


#### Useful Documentation
- [Modern C++ features: C++20/17/14/11](https://github.com/AnthonyCalandra/modern-cpp-features)

#### Development
- CMake > 3.8
- C++20 (concepts are used)
