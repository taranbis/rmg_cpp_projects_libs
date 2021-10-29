Boost.Math: FFT Utilities
Potential mentor: Christopher Kormanyos

Project requires knowledge of modern C++ template metaprogramming techniques.

Background
Boost.Math http://www.boost.org/libs/math is a large mathematical library that offers high-performance functions of pure and applied mathematics such as floating-point utilities, special functions, statistical functions, iterative equation-solving tools, integration routines and more.

Boost.Math has needed FFT utilities for a while now. Accordingly, we are looking for a student to assist with implementing and testing a FFT utilities. This GSoC project will include implementing, testing, banchmarking and checking compatibility with Boost.Math.

This challenging project will strengthen mathematical and algorithmic programming skills in template-based C++. It is best suited for students whose studies have included numerical transformations and, in particular, for those who have previously dabbled in programming them. Abilities in modern C++, high-performance mathematical programming, and advanced template and generic programming are mandatory. FFTs are enormously useful for transformations, signal and image processing, convolutions, modelling and numerous other fascinating domains of research and development.

PROJECT 1.
Implement a feasible realization of FFT. This project is based on an existing, modern community request for FFT here with additional further discussion.

Tasks include:

Define the FFT template interface.
Start with a simple first step such as one-dimensional real-to-half-complex.
Get a test system ready for this first step.
If time permits:

Extend FFT to two dimensions.
Consider threading aspects and potential multi-threading distribution of computational complexity.
Programming competency test
Show familiarity with both C++ template programming as well as programming numerical transformations such as FFT. Select a transformation with, say, one dimension. Use your imagination to create a naive one-way floating-point FFT, Laplace or similar transformation working for built-in types such as float and double. Show your mastery of this kind of programming.