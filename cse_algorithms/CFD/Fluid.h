#ifndef FLUID_HEADER_H
#define FLUID_HEADER_H 1
#pragma once

#include <array>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#define DEB(x) std::cout << #x << " =  " << x << "\t"
#define DEB_SHORT(x) std::cout << std::setw(25) << #x << " = " << std::setw(16) << x << ";           "
#define NEWLINE() std::cout << std::endl

#define DEBLINE() std::cout << __LINE__ << std::endl;
#define DEBFILE() std::cout << __FILE__ << std::endl;
#define DEBFUNC() std::cout << __func__ << std::endl;

// 20 was in the original paper
const int iter = 15;

// make the screen size by default 512 x 512
template <int N = 512>
class Fluid
{
public:           // TODO: make setters and getters for this
    int _size;    // size = N
    double _dt;   // length of the timestamp
    double _diff; // amount of diffusion
    double _visc; // viscosity

    int _Nx = N;
    int _Ny = N;

    double _hx = 1 / _Nx;
    double _hy = 1 / _Ny;

    // typedef std::array<std::array<double, N + 1>, N + 1> ArrayType;
    // typedef ArrayType&                                   ArrayReference;
    typedef std::vector<double> Vector;
    typedef std::vector<std::vector<double>> Matrix;
    typedef Matrix &MatrixReference;

    // double* _s;       // 2D array
    Matrix _density;
    Matrix _Vx;
    Matrix _Vy;

    // Should not need these
    // std::array<std::array<double, N>, N> _s;
    // std::array<std::array<double, N>, N> _Vx0;
    // std::array<std::array<double, N>, N> _Vy0;

public:
    // this is why this is callled constructor initializer list. It calls the
    // constructors of each member type
    Fluid(double diffusion, double viscosity, double dt)
            : _diff(diffusion), _visc(viscosity), _dt(dt), _density(_Nx + 1, Vector(_Ny + 1, 0)),
              _Vx(N + 1, Vector(N + 1, 0)), _Vy(N + 1, Vector(N + 1, 0))
    {
    }

    // 2. Each corner cell equal to the average of its three neighbors
    void SetBoundary(int b, MatrixReference x)
    {
        // every velocity in the layer next to the outer layer is mirrored
        for (int i = 1; i < N; i++) { // up and buttom walls
            x[i][0] = b == 2 ? -x[i][1] : x[i][1];
            x[i][N] = b == 2 ? -x[i][N - 1] : x[i][N - 1];
        }

        for (int j = 1; j < N; j++) {
            x[0][j] = b == 1 ? -x[1][j] : x[1][j];
            x[N][j] = b == 1 ? -x[N - 1][j] : x[N - 1][j];
        }
        x[0][0] = 0.5 * (x[1][0] + x[0][1]);
        x[0][N] = 0.5 * (x[1][N] + x[0][N - 1]);
        x[N][0] = 0.5 * (x[N][1] + x[N - 1][0]);
        x[N][N] = 0.5 * (x[N - 1][N] + x[N][N - 1]);
    }

    void AddDye(int x, int y, double amount)
    {
        // safe checkers: 0<=x,y<N
        if (0 <= x && x <= N && 0 <= y && y <= N) { this->_density[x][y] += amount; }
    }

    void AddVelocity(int x, int y, double amountX, double amountY)
    {
        // safe checkers: 0<=x,y<N
        if (0 <= x && x <= N && 0 <= y && y <= N) {
            this->_Vx[x][y] += amountX;
            this->_Vy[x][y] += amountY;
        }
    }

    // Diffuse(2, Vy0, Vy, visc, dt, 4, N);
    void Diffuse(int b, MatrixReference arr)
    {
        // Gauss-Seidel relaxation
        double a = _dt * _visc * _Nx * _Ny;
        if (b == 0) { a = _dt * _diff * _Nx * _Ny; }
        for (int k = 0; k < iter; k++) {
            for (int i = 1; i < N; i++) {
                for (int j = 1; j < N; j++) {
                    arr[i][j] = (arr[i][j] +
                                 a * (arr[i - 1][j] + arr[i + 1][j] + arr[i][j + 1] + arr[i][j - 1])) /
                                (1 + 4 * a);
                }
            }
            SetBoundary(b, arr);
        }
    }

    /**
     * \fn Advect
     * \brief This function works as follows: we have the grid points. We go
     * through each of them and based on the velocity that we have at that grid
     * point, we estimate the coordinates (x, y) from which the previous
     * concentration particle came. Since we don't the intermediate values, we
     * approximate it to the closest grid point. We then interpolate the value
     * at the previous cell based on the neighbours
     * @param b tells the function what kind of array we deal with: x, y
     * velocities or dye density; it can take values: x -> 1; y -> 2; dye
     * density->0;
     * @param arr a constant character pointer.
     * @see
     * @return
     */
    void Advect(int b, MatrixReference arr)
    {
        int i = 1, j = 1, i0 = 0, j0 = 0, i1 = 0, j1 = 0, s1 = 0, s0 = 0, k1 = 0, k0 = 0;
        double x = 0;
        double y = 0;
        Matrix arr_new(N + 1, Vector(N + 1, 0));

        double dt0 = _dt * (N - 1);
        for (i = 1; i < N; ++i) {
            for (j = 1; j < N; ++j) {
                x = (double)i - dt0 * _Vx[i][j];
                y = (double)j - dt0 * _Vy[i][j];

                if (x < 0.5) x = 0.5;
                if (x > (N - 1) + 0.5) x = (N - 1) + 0.5;

                // calculate the index of the grid point from which the density
                // came
                i0 = (int)x;
                i1 = i0 + 1;
                // interpolation coefficients
                s1 = (int)x - i0;
                s0 = 1 - s1;

                if (y < 0.5) y = 0.5;
                if (y > (N - 1) + 0.5) y = (N - 1) + 0.5;

                j0 = (int)y;
                j1 = j0 + 1;
                // interpolation coefficients for y
                k1 = (int)y - j0;
                k0 = 1 - k1;

                arr_new[i][j] = (double)k0 * ((double)s0 * arr[i0][j0] + (double)s1 * arr[i1][j0]) +
                                (double)k1 * ((double)s0 * arr[i0][j1] + (double)s1 * arr[i1][j1]);
            }
        }

        std::swap(arr, arr_new);
        SetBoundary(b, arr);
    }

    // this operation goes through all the cells and fixes them so the
    // amount that goes in is the exact amount that goes out
    void Project()
    {
        // compute the gradient field; Using Hodge decomposition, a velocity
        // field is the sum of a mass conserving field and a gradient field;
        Matrix div(N + 1, Vector(N + 1, 0));
        // for (auto& arr : div) { arr.fill(0.0); }

        for (int i = 1; i < N; i++) {
            for (int j = 1; j < N; j++) {
                div[i][j] = -0.5 * 1.0 / N *
                            (_Vx[i + 1][j] - _Vx[i - 1][j] + _Vy[i][j + 1] - _Vy[i][j - 1]); // p[i][j] = 0;
            }
        }

        SetBoundary(0, div);

        // const double f = 1 / (_hx * _hx);
        // const double g = 1 / (_hy * _hy);
        // const double k = -2 * (f + g);

        Matrix p(N + 1, Vector(N + 1, 0));

        // The system is sparse and we can re-use our Gauss-Seidel relaxation
        // code developed for the density diffusion step to solve it.
        for (int k = 0; k < iter; k++) {
            for (int i = 1; i < N; i++) {
                for (int j = 1; j < N; j++) {
                    p[i][j] = (div[i][j] + p[i + 1][j] + p[i - 1][j] + p[i][j + 1] + p[i][j - 1]) / 4;
                }
            }
            SetBoundary(0, p);
        }

        for (int i = 1; i < N; i++) {
            for (int j = 1; j < N; j++) {
                _Vx[i][j] -= 0.5 * (p[i + 1][j] - p[i - 1][j]) * N;
                _Vy[i][j] -= 0.5 * (p[i][j + 1] - p[i][j - 1]) * N;
            }
        }
        SetBoundary(1, _Vx);
        SetBoundary(2, _Vy);
    }

    /**
     * \fn Step
     * \brief
     * @see Diffuse()
     * @see Advect()
     * @see Project()
     */
    void Step()
    {
        // Diffuse all velocity components:2 for 2D
        Diffuse(1, _Vx);
        Diffuse(2, _Vy);

        // Fix the velocities so we maintain incompressibility
        Project();

        // Move the velocities around according to the velocities of the fluid
        Advect(1, _Vx);
        Advect(2, _Vy);

        // Fix again the velocities so we maintain incompressibility
        Project();

        // Diffuse the dye
        Diffuse(0, _density);

        // Spread the dye according to velocities
        Advect(0, _density);
    }
};

#endif //! FLUID_HEADER_H
