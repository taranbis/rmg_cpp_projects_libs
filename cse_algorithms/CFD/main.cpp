#include "Fluid.h"

void setup() {}

#define SCALE 5

// Render the results
void draw
{
    // TODO: shoot dye randomly from centerly in all directions (360 directions)
    // TODO: add rainbow colows
    addDensity(mouseX / SCALE, mouseY / SCALE, 100);
    fluid.step();

    render;
}

void mouseDrag()
{
    fluid.addVelocity(mouseX / SCALE, mouseY / SCALE, amtX, amt Y);
}

int main()
{
    const int iter = 10;
    //(double diffusion, double viscosity, double dt)
    double                 diffusion = 0.001;
    double                 viscosity = 0.01;
    double                 dt = 0.001;
    std::unique_ptr<Fluid> fluid = std::make_unique(diffusion, viscosity, dt);
}