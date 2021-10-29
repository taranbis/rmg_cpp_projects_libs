#include <GL/glew.h>
// Glew should be included first
// #include <GL/gl.h>
// #include <GLFW/glfw3.h>
#include "Fluid.h"

#include <glut.h>
#include <stdio.h>

void draw();
void FluidLoop(int value);
void mouseHandler(int button, int state, int x, int y);
void mouseMotionHandler(int x, int y);

int mouseXPos, mouseYPos;

static constexpr std::size_t N         = 512;
static constexpr std::size_t SCRWidth  = N;
static constexpr std::size_t SCRHeight = N;
std::unique_ptr<Fluid<N>>    globalFluid;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);

    glutInitWindowSize(SCRWidth, SCRHeight);
    glutCreateWindow("Fluid Simulation");

    //(X going from 0 to windowWidth and Y going from 0 to windowHeight)
    // Note that I've flipped the Y values because OpenGL coordinates start from
    // the bottom left corner of the window. So by flipping, I get a more
    // conventional (0,0) starting at the top left corner of the window rather.
    glOrtho(0, (double)SCRWidth, (double)SCRHeight, 0, 0, 1); // Orient and define grid

    double diffusion = 0.001;
    double viscosity = 0.01;
    double dt        = 0.001;
    globalFluid      = std::make_unique<Fluid<N>>(diffusion, viscosity, dt);

    glutDisplayFunc(draw);
    glutMouseFunc(mouseHandler);
    glutMotionFunc(mouseMotionHandler);

    FluidLoop(0);

    // calls your display callback over and over
    glutMainLoop();
    return 0;
}

// If you use glDrawPixels with a format argument of GL_LUMINANCE and a
// type argument of GL_UNSIGNED_BYTE, then you can pretty easily convert
// this code to operate on a byte array, which will keep the memory consumption
// down somewhat as compared with using 32-bit RGBA values.

void FluidLoop(int value)
{
    draw();
    // globalFluid->Step();
    glutTimerFunc(100, FluidLoop, 0);
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT); // blank display

    glDisable(GL_DEPTH_TEST);
    // TODO: _density needs to be a void* array :(
    glDrawPixels(N, N, GL_RGBA, GL_FLOAT, &globalFluid->_density[0][0]);
    glEnable(GL_DEPTH_TEST);

    glFlush();
    glutSwapBuffers();
}

void mouseHandler(int button, int state, int x, int y)
{
    mouseMotionHandler(x, y);

    if (state == GLUT_DOWN) { // button press
        if (button == GLUT_LEFT_BUTTON) {
            // left_click = true;
            // AddDye(int x, int y, double amount)
            globalFluid->AddDye(x, y, 100);
            globalFluid->AddVelocity(x, y, 10, 10);
            return;
        }
    }
}

void mouseMotionHandler(int x, int y)
{
    mouseXPos = x;
    mouseYPos = y;
}
