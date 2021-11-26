#include "Fluid.h"

#include <cmath>
#include <iostream>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>

static constexpr std::size_t N = 256;
static constexpr std::size_t SCRWidth = N;
static constexpr std::size_t SCRHeight = N;
std::unique_ptr<Fluid<N>>    globalFluid;

int main(int argc, char* argv[])
{
    double diffusion = 0.02;
    double viscosity = 0.02;
    double dt = 0.02;

    // double diffusion = 2;
    // double viscosity = 2;
    // double dt        = 2;

    // Fluid(0.1, 0, 0); -> original code
    globalFluid = std::make_unique<Fluid<N>>(diffusion, viscosity, dt);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window*   window = SDL_CreateWindow("Fluid Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCRWidth, SCRHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture*  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                             SCRWidth, SCRHeight);

    int  mouseX = -1;
    int  mouseY = -1;
    bool mouseClick = false;
    int  running = 1;

    int counter = 0;

    unsigned char*             lockedPixels = nullptr;
    int                        pitch = 0;
    std::vector<unsigned char> pixels(SCRWidth * SCRHeight * 4, 0);

    while (running) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                const unsigned int offset = (SCRWidth * 4 * i) + j * 4;

                uint32_t tmp;

                if (globalFluid->_density[i][j] > 255) {
                    tmp = 255;
                } else {
                    tmp = (uint32_t)globalFluid->_density[i][j];
                }

                // we can also add the masks
                pixels[offset + 0] = (uint32_t)tmp % 256; // b
                pixels[offset + 1] = (uint32_t)tmp % 256; // g
                pixels[offset + 2] = (uint32_t)tmp % 256; // r
                pixels[offset + 3] = SDL_ALPHA_OPAQUE;    // a SDL_ALPHA_OPAQUE is
                                                          // now considered opaque.

                // DEB(offset);
                // DEB(pixels[offset + 0]);
                // DEB(pixels[offset + 1]);
                // DEB(pixels[offset + 2]);
                // NEWLINE();
            }
        }
        SDL_LockTexture(texture, NULL, reinterpret_cast<void**>(&lockedPixels), &pitch);
        std::memcpy(lockedPixels, pixels.data(), pixels.size());
        SDL_UnlockTexture(texture);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: running = 0; break;
            case SDL_MOUSEMOTION: {
                int amtY = mouseX - event.motion.x;
                int amtX = mouseY - event.motion.y;
                // DEB_SHORT(amtY);
                // DEB_SHORT(amtX);
                // NEWLINE();

                mouseX = event.motion.y;
                mouseY = event.motion.x;

                globalFluid->AddVelocity(mouseX, mouseY, amtX, amtY);
                if (mouseClick) { globalFluid->AddDye(mouseX, mouseY, 10000); }
                globalFluid->Step();
                break;
            }
            }
        }

        globalFluid->AddDye(128, 128, 10000);
        globalFluid->AddVelocity(128, 128, 1, 1);
        globalFluid->Step();

        DEB_SHORT(globalFluid->_density[128][128]);
        DEB_SHORT(globalFluid->_Vx[128][128]);
        DEB_SHORT(globalFluid->_Vy[128][128]);
        NEWLINE();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
