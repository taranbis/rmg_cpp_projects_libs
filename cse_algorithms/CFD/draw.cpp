#include "Fluid.h"

#include <math.h> /* fmod */
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define REDMASK (0xff0000)
#define GREENMASK (0x00ff00)
#define BLUEMASK (0x0000ff)

static constexpr std::size_t N = 400;
static constexpr std::size_t SCRWidth = N;
static constexpr std::size_t SCRHeight = N;
std::unique_ptr<Fluid<N>>    globalFluid;

int main(int argc, char* argv[])
{
    // double diffusion = 0.02;
    // double viscosity = 0.02;
    // double dt        = 0.02;

    double diffusion = 2;
    double viscosity = 2;
    double dt = 2;

    // Fluid(0.1, 0, 0); -> original code
    globalFluid = std::make_unique<Fluid<N>>(diffusion, viscosity, dt);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Fluid Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCRWidth, SCRHeight, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                             SCRWidth, SCRHeight);

    int  mouseX = -1;
    int  mouseY = -1;
    bool mouseClick = false;
    int  running = 1;

    while (running) {
        unsigned char*             lockedPixels = nullptr;
        int                        pitch = 0;
        std::vector<unsigned char> pixels(SCRWidth * SCRHeight * 4, 0);

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
                // pixels[offset + 0] = (uint32_t)tmp % 256; // b
                // pixels[offset + 1] = (uint32_t)tmp % 256; // g
                pixels[offset + 0] = 0;                   // b
                pixels[offset + 1] = 0;                   // g
                pixels[offset + 2] = (uint32_t)tmp % 256; // r
                pixels[offset + 3] = SDL_ALPHA_OPAQUE;    // a SDL_ALPHA_OPAQUE is
                                                          // now considered opaque.
            }
        }
        SDL_LockTexture(texture, NULL, reinterpret_cast<void**>(&lockedPixels), &pitch);
        memcpy(lockedPixels, pixels.data(), pixels.size());
        SDL_UnlockTexture(texture);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // find other keys here:
            // http:// sdl.beuc.net/sdl.wiki/SDLKey
            switch (event.type) {
            case SDL_QUIT: running = 0; break;
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE: {
                    globalFluid->AddDye(N / 2, N / 2, 15000);
                    break;
                }
                case SDLK_ESCAPE: {
                    running = 0;
                    break;
                }
                default: {
                    break;
                }
                }
            }
            case SDL_MOUSEMOTION: {
                const int scale = 5;
                int       amtY = (mouseX - event.motion.y) * scale;
                int       amtX = (mouseY - event.motion.x) * scale;

                mouseX = event.motion.y;
                mouseY = event.motion.x;

                globalFluid->AddVelocity(mouseX, mouseY, amtX, amtY);
                if (mouseClick) { globalFluid->AddDye(mouseX, mouseY, 15000); }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                mouseClick = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                mouseX = event.motion.y;
                mouseY = event.motion.x;
                mouseClick = true;
                globalFluid->AddDye(mouseX, mouseY, 15000);
                break;
            }
            default: {
                break;
            }
            }
        }
        globalFluid->Step();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
