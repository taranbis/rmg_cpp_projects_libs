#ifndef UTIL_HEADER_H
#define UTIL_HEADER_H 1
#pragma once

#include <algorithm>
#include <cstdio>
#include <deque>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

// Extracts the first argument out of a list of any number
// of arguments
#define ARG_1(a, ...) a

/********************************************************************************
 *                              Helpful macros
 *******************************************************************************/

#ifndef personal_OS
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(linux) || defined(__linux) ||          \
            defined(__linux__) || defined(sun) || defined(__sun) || defined(BSD) || defined(__OpenBSD__) || \
            defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__) || defined(sgi) ||        \
            defined(__sgi) || defined(__OSX__) || defined(__MACOSX__) || defined(__APPLE__) ||              \
            defined(__CYGWIN__)
#define personal_OS 1
#elif defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || \
            defined(_WIN64) || defined(__WIN64__)
#define personal_OS 2
#else
#define personal_OS 0
#endif
#elif !(personal_OS == 0 || personal_OS == 1 || personal_OS == 2)
#error CImg Library: Invalid configuration variable 'cimg_OS'.
#error(correct values are '0 = unknown OS', '1 = Unix-like OS', '2 = Microsoft Windows').
#endif

#ifdef __has_builtin
#define PERSONAL_HAS_BUILTIN(x) __has_builtin(x)
#else
#define PERSONAL_HAS_BUILTIN(x) 0
#endif

#ifdef __clang__
#define PERSONAL_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#define PERSONAL_CLANG_VERSION 0
#endif

#ifdef __INTEL_COMPILER
#define PERSONAL_ICC_VERSION __INTEL_COMPILER
#else
#define PERSONAL_ICC_VERSION 0
#endif

#ifdef _WIN32
#define PERSONAL_USE_WINDOWS_H 1
#else
#define PERSONAL_USE_WINDOWS_H 0
#endif

#ifdef _MSC_VER
#define PERSONAL_MSC_VER _MSC_VER
#else
#define PERSONAL_MSC_VER 0
#endif

// modify these
#if defined(__GNUC__) && !defined(__clang__)
#define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#define FMT_GCC_VERSION 0
#endif

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#define FMT_HAS_GXX_CXX11 FMT_GCC_VERSION
#else
#define FMT_HAS_GXX_CXX11 0
#endif

#if defined __x86_64__ && !defined __ILP32__
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#define __WORDSIZE32_SIZE_ULONG 0
#define __WORDSIZE32_PTRDIFF_LONG 0
#endif

/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
#ifndef __intptr_t_defined
typedef long int intptr_t;
#define __intptr_t_defined
#endif
typedef unsigned long int uintptr_t;
#else
#ifndef __intptr_t_defined
typedef int intptr_t;
#define __intptr_t_defined
#endif
typedef unsigned int uintptr_t;
#endif

/*****************************************************************************
 *                             fork.cpp
 *****************************************************************************/

#include <iostream>

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

// int main()
// {
//     int childPid = fork();
//     if (childPid < 0) {
//         std::cout << "fork failed" << std::endl;
//     } else if (childPid == 0) {
//         // child
//         std::cout << "child" << std::endl;
//     } else {
//         // parent
//         std::cout << "parent" << std::endl;
//     }
//     return 0;
// }

#if defined(_WIN32) && defined(UA_DYNAMIC_LINKING)
#ifdef UA_DYNAMIC_LINKING_EXPORT /* export dll */
#ifdef __GNUC__
#define UA_EXPORT __attribute__((dllexport))
#else
#define UA_EXPORT __declspec(dllexport)
#endif
#else /* import dll */
#ifdef __GNUC__
#define UA_EXPORT __attribute__((dllimport))
#else
#define UA_EXPORT __declspec(dllimport)
#endif
#endif
#else /* non win32 */
#if __GNUC__ || __clang__
#define UA_EXPORT __attribute__((visibility("default")))
#endif
#endif
#ifndef UA_EXPORT
#define UA_EXPORT /* fallback to default */
#endif

#if !defined(_UA_BEGIN_DECLS)
#ifdef __cplusplus
#define _UA_BEGIN_DECLS extern "C" {
#else
#define _UA_BEGIN_DECLS
#endif
#endif
#if !defined(_UA_END_DECLS)
#ifdef __cplusplus
#define _UA_END_DECLS }
#else
#define _UA_END_DECLS
#endif
#endif

/********************************************************************************
 *         Header file that defines all sorts of useful functions
 *******************************************************************************/

#define all(x) x.begin(), x.end()
#define sortall(x) sort(all(x))

#define DEB(x) std::cout << #x << " =  " << x << std::endl
#define DEB_HEX(x) std::cout << #x << " = 0x" << std::hex << x << std::dec << " "
#define DEB_LONG(x) std::cout << std::setw(50) << #x << " = " << std::setw(12) << x << " "
#define DEB_SHORT(x) std::cout << std::setw(25) << #x << " = " << std::setw(5) << x << " "
#define NEWLINE() std::cout << std::endl
#define DEBLINE() std::cout << __LINE__ << std::endl;
#define DEBFILE() std::cout << __FILE__ << std::endl;

#define WORD_SIZE sizeof(void *)

#if (personal_OS == 1)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#elif (personal_OS == 2)
#define __FILENAME__ (strrchr(__FILE__, "\\") ? strrchr(__FILE__, "\\") + 1 : __FILE__)
#endif

#define persLog(x) \
    std::cout << #x << " =  " << x << "; line: " << __LINE__ << "; file: " << __FILENAME__ << std::endl
#define persLogHex(x)                                                                \
    std::cout << #x << " =  " << std::hex << x << std::dec << "; line: " << __LINE__ \
              << "; file: " << __FILENAME__ << std::endl
#define persLogBool(x)                                                                                       \
    std::cout << #x << " =  " << std::boolalpha << x << "; line: " << __LINE__ << "; file: " << __FILENAME__ \
              << std::endl
#define persLogBinary(x)             \
    std::bitset<sizeof(x) * 8> y(x); \
    std::cout << #x << " =  " << y << "; line: " << __LINE__ << "; file: " << __FILENAME__ << std::endl

#define PAUSE_THREAD(x)                                        \
    static int counter = 0;                                    \
    counter++;                                                 \
    if (counter == x) {                                        \
        std::this_thread::sleep_for(std::chrono::seconds(10)); \
        counter = 0;                                           \
    }

namespace personal
{
// TODO: rewrite hexdump!!!
static void hexdump(FILE *fp, const char *name, const void *ptr, size_t len)
{
    const char *p = (const char *)ptr;
    size_t of = 0;

    if (name) fprintf(fp, "%s hexdump (%zd bytes):\n", name, len);

    for (of = 0; of < len; of += 16) {
        char hexen[16 * 3 + 1];
        char charen[16 + 1];
        int hof = 0;

        int cof = 0;
        int i;

        for (i = of; i < (int)of + 16 && i < (int)len; i++) {
            hof += sprintf(hexen + hof, "%02x ", p[i] & 0xff);
            cof += sprintf(charen + cof, "%c", isprint((int)p[i]) ? p[i] : '.');
        }
        fprintf(fp, "%08zx: %-48s %-16s\n", of, hexen, charen);
    }
}

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

template <typename Type>
void swap(Type *&i, Type *&j)
{
    // copy constructor
    Type aux = i;

    // copy operation
    i = j;
    j = aux;
}

/**
 * @fn CreateMatrix
 * @brief Converts an into to a bool.
 * @param n An integer.
 * @return @em true if n not equal to zero, @em false otherwise
 * TODO: create it serious template
 */
template <typename T>
void CreateMatrix()
{
    int RR = 4;
    int CC = 5;

    std::vector<std::vector<char>> grid(RR);
    for (int i = 0; i < RR; ++i) { grid[i].resize(CC); }
}

/**
 * @fn ToBool
 * @brief Converts an into to a bool.
 * @param n An integer.
 * @return @em true if n not equal to zero, @em false otherwise
 */
inline bool ToBool(int n)
{
    return n != 0;
}

/**
 * @fn ToInt
 * @brief Converts the boolean into a C integer true/false value.
 * @param b A boolean
 * @return Zero if b is false, non-zero if b is true.
 */
inline int ToInt(bool b)
{
    return b ? (!0) : 0;
}

/**
 * @fn ToString
 * @brief Gets a valid string for the char pointer.
 * @param cstr A C-string pointer
 * @return A string copy of the C array. If `cstr` is NULL, this returns an
 *  	   empty string.
 */
inline std::string ToString(const char *cstr)
{
    return cstr ? std::string(cstr) : std::string();
}

/**
 * @fn ExtractNumbersOutOfFile
 * @brief Extract numbers out of a given string
 * e.g. [[],[100],[80],[60],[70],[60],[75],[85]]
 * Output: 100, 80, 60, 70, 60, 75, 85
 *
 * Used in Stockspanner problem from leetcode
 * @param file_name numeric floating point type
 * @return
 */
inline std::vector<int> ExtractNumbersOutOfFile(const char *file_name)
{
    std::ifstream file(file_name);
    std::stringstream ss;
    ss << file.rdbuf();
    std::vector<int> v;
    while (ss.good()) { // explain ss.good() and getline
        std::string input;
        getline(ss, input, ',');
        std::string output = std::regex_replace(input, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
        try { // it is done beucase it might be empty
            v.emplace_back(stoi(output));
        } catch (std::exception &e) {
            // std::cout << e.what() << std::endl;
            continue;
        }
    }
    return v;
}

/**
 * @brief computes square root of a number
 *
 * @tparam n
 * @return double
 */
template <typename NumericType>
inline double computeSquareRoot(NumericType n)
{
    double beg = 0, end = n, mid;
    for (std::size_t i = 0; i < 500; ++i) {
        mid = (beg + end) / 2;
        if (mid * mid == n) return mid;
        if (mid * mid > n)
            end = mid;
        else {
            beg = mid;
        }
    }
    return mid;
}

/**
 * @brief reads a file line by line into a matrix
 *
 * @tparam T
 * @param grid
 */
template <typename T>
void ReadGridFromFile(std::vector<std::vector<T>> &grid)
{
    // std::ifstream file("tests.txt");
    // std::string line;
    // while(std::getline(file,line)){
    //     std::cout << line << std::endl;
    //     std::string::iterator it_line = line.begin();
    //     std::vector<char> v;
    //     while(it_line != line.end()){
    //         if(*it_line != ' ' && *it_line != '\r'){
    //             v.emplace_back(*it_line);
    //         }
    //         it_line++;
    //     }
    //     grid.emplace_back(v);
    // }

    std::ifstream file("tests.txt");
    std::stringstream ss;
    ss << file.rdbuf();
    const std::string &s = ss.str();
    std::string::const_iterator it_s = s.begin();

    std::vector<T> vec(s.begin(), s.end());
    for (; it_s != s.end() && *it_s != '\000'; ++it_s) {
        std::vector<T> v;
        while (it_s != s.end() && *it_s != '\r' && *it_s != '\n') {
            if (*it_s != ' ') v.emplace_back((*it_s) - 48);
            it_s++;
        }
        if (v.size() != 0) { grid.emplace_back(v); }
    }
}

template <typename T>
void PrintMatrix(std::vector<std::vector<T>> &grid)
{
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) { std::cout << grid[i][j] << " "; }
        std::cout << std::endl; // works also with "\n", don't need to
                                // refresh buffer
    }
}

template <typename T>
void PrintVector(std::vector<T> &v)
{
    std::cout << "Printing Vector" << '\n';
    size_t size = v.size();
    if (size == 0) return;
    std::cout << "[";
    size_t i = 0;
    for (; i < size - 1; ++i) {
        // std::cout << "v[ " << i << "]= " << v[i] << std::endl;
        std::cout << v[i] << ", ";
    }
    std::cout << v[i];
    std::cout << "] " << std::endl;
}

} // namespace personal

/*****************************************************************************
 *     Macros to test for successful allocation of memory and to free it
 *****************************************************************************/
#define MEMCHECK(x) (((x) == NULL) ? 101 : 0)
#define FREE(x)     \
    do {            \
        free(x);    \
        (x) = NULL; \
    } while (0)

/*****************************************************************************
 *     Inline functions to be used in place of the standard functions
 *****************************************************************************/
template <typename T>
inline int INT(T x)
{
    return static_cast<int>(x);
}

template <typename T>
inline int FRAC(T x)
{
    return x - static_cast<int>(x);
}

template <typename T>
inline T ABS(T x)
{
    return ((x < 0) ? (-x) : x);
}
template <typename T>
inline T MAX(T x, T y)
{
    return ((x >= y) ? x : y);
}

template <typename T>
inline T MIN(T x, T y)
{
    return ((x <= y) ? x : y);
}

#define ROUND(x) (((x) >= 0) ? (int)((x) + .5) : (int)((x)-.5))
// round-off of x
#define MOD(x, y) ((x) % (y))           // x modulus y
#define SQR(x) ((x) * (x))              // x-squared
#define SGN(x) (((x) < 0) ? (-1) : (1)) // sign of x
#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x) & ~32) : (x))
// uppercase char of x

/*****************************************************************************
 *              Macro to evaluate function x with error checking
 *              (Fatal errors are numbered higher than 100)
 *****************************************************************************/
#define ERRCODE(x) (errcode = ((errcode > 100) ? (errcode) : (x)))

// Constructs fp from an IEEE754 double. It is a template to prevent compile
// errors on platforms where double is not IEEE754.

// template <typename Double>
// explicit fp(Double d)
// {
//     // Assume double is in the format [sign][exponent][significand].
//     typedef std::numeric_limits<Double> limits;
//     const int double_size = sizeof(Double) * char_size;
//     const int exponent_size = double_size - double_significand_size - 1; //
//     -1 for sign const uint64_t significand_mask = implicit_bit - 1; const
//     uint64_t exponent_mask = (~0ull >> 1) & ~significand_mask; const int
//     exponent_bias = (1 << exponent_size) - limits::max_exponent - 1; auto u =
//     bit_cast<uint64_t>(d); auto biased_e = (u & exponent_mask) >>
//     double_significand_size; f = u & significand_mask; if (biased_e != 0)
//         f += implicit_bit;
//     else
//         biased_e = 1; // Subnormals use biased exponent 1 (min exponent).
//     e = static_cast<int>(biased_e - exponent_bias - double_significand_size);
// }

// // Normalizes the value converted from double and multiplied by (1 << SHIFT).
// template <int SHIFT = 0>
// void normalize()
// {
//     // Handle subnormals.
//     auto shifted_implicit_bit = implicit_bit << SHIFT;
//     while ((f & shifted_implicit_bit) == 0) {
//         f <<= 1;
//         --e;
//     }
//     // Subtract 1 to account for hidden bit.
//     auto offset = significand_size - double_significand_size - SHIFT - 1;
//     f <<= offset;
//     e -= offset;
// }
// }
// ;

// namespace personal {
// template <typename Exception>//take a look at these
// inline void do_throw(const Exception& x)
// {
//     // Silence unreachable code warnings in MSVC because these are nearly
//     // impossible to fix in a generic code.
//     volatile bool b = true;
//     if (b)
//         throw x;
// }
// } // namespace internal
#endif // !UTIL_HEADER_H
