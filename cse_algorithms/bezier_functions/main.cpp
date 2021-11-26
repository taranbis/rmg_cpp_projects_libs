#include <math.h>
#include <vector>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define REDMASK (0xff0000)
#define GREENMASK (0x00ff00)
#define BLUEMASK (0x0000ff)

// static constexpr std::size_t N = 400;
static constexpr std::size_t SCRWidth = 800;
static constexpr std::size_t SCRHeight = 600;

struct Point {
    int x;
    int y;
};

struct Color {
    int r;
    int g;
    int b;
    int opacity = SDL_ALPHA_OPAQUE;
};

void drawCircle(SDL_Renderer* renderer, int center_x, int center_y, int radius_, const Color& color)
{
    // Setting the color to be RED with 100% opaque (0% trasparent).
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.opacity);

    // Drawing circle
    for (int x = center_x - radius_; x <= center_x + radius_; x++) {
        for (int y = center_y - radius_; y <= center_y + radius_; y++) {
            if ((std::pow(center_y - y, 2) + std::pow(center_x - x, 2)) <= std::pow(radius_, 2)) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    // Show the change on the screen
    SDL_RenderPresent(renderer);
}

void moveCircle()
{
    // Setting the color to be RED with 100% opaque (0% trasparent).

    // SDL_Event event; // Event variable
    // while (!(event.type == SDL_QUIT)) {
    //     // Circle will go down!
    //     for (int i = 0; i < height; i++) {
    //         SDL_Delay(10);         // setting some Delay
    //         SDL_PollEvent(&event); // Catching the poll event.
    //         if (event.type == SDL_QUIT) return;
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    //         SDL_RenderClear(renderer);
    //         draw_circle(width / 2, i, 25);
    //     }

    //     // Circle will go up!
    //     for (int i = height; i > 0; i--) {
    //         SDL_Delay(10);         // setting some Delay
    //         SDL_PollEvent(&event); // Catching the poll event.
    //         if (event.type == SDL_QUIT) return;
    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    //         SDL_RenderClear(renderer);
    //         draw_circle(width / 2, i, 25);
    //     }
    // }
}

// bezier(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4)

// double lerp(double x1, double x2, double t)
// {
//     return x1 + (x2 - x1) * t;
// }

// Receives three points and calculates the Bezier point for that respective t
Point quadratic(SDL_Renderer* renderer, const Point& p0, const Point& p1, const Point& p2, double t)
{
    Point h1;
    h1.x = lerp(p0.x, p1.x, t);
    h1.y = lerp(p0.y, p1.y, t);

    Point h2;
    h2.x = lerp(p1.x, p2.x, t);
    h2.y = lerp(p1.y, p2.y, t);

    SDL_RenderDrawLine(renderer, h1.x, h1.y, h2.x, h2.y);

    Point rv;
    rv.x = lerp(h1.x, h2.x, t);
    rv.y = lerp(h1.y, h2.y, t);
    return rv;
}

Point cubic(SDL_Renderer* renderer, const Point& p0, const Point& p1, const Point& p2, const Point& p3, double t)
{
    Point v1 = quadratic(renderer, p0, p1, p2, t);
    Point v2 = quadratic(renderer, p1, p2, p3, t);

    SDL_RenderDrawLine(renderer, v1.x, v1.y, v2.x, v2.y);

    Point rv;
    rv.x = lerp(v1.x, v2.x, t);
    rv.y = lerp(v1.y, v2.y, t);
    return rv;
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Bezier Functions", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCRWidth, SCRHeight, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                             SCRWidth, SCRHeight);

    std::vector<Point*> points;
    std::vector<Point*> controlPoints;
    std::vector<Point*> anchorPoints;
    // Create control points
    std::unique_ptr<Point> c1(new Point{250, 50});
    controlPoints.push_back(c1.get());
    std::unique_ptr<Point> c2(new Point{500, 400});
    controlPoints.push_back(c2.get());

    // Create anchors
    std::unique_ptr<Point> a0(new Point{50, 200});
    anchorPoints.push_back(a0.get());
    std::unique_ptr<Point> a3(new Point{750, 250});
    anchorPoints.push_back(a3.get());

    std::merge(controlPoints.begin(), controlPoints.end(), anchorPoints.begin(), anchorPoints.end(),
               std::back_inserter(points));

    int    mousePosX = -1;
    int    mousePosY = -1;
    bool   mouseClick = false;
    bool   running = true;
    double dt = 0.05;

    Point* draggedPoint = nullptr;

    std::vector<Point> bezierCurve;
    bezierCurve.reserve(10000);

    while (running) {
        unsigned char*             lockedPixels = nullptr;
        std::vector<unsigned char> pixels(SCRWidth * SCRHeight * 4, 0);

        if (mouseClick) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
        }

        for (auto& p : controlPoints) drawCircle(renderer, p->x, p->y, 5, Color{0, 0, 255});
        for (auto& p : anchorPoints) drawCircle(renderer, p->x, p->y, 5, Color{0, 255, 0});
        // SDL_RenderDrawLine(renderer, a0->x, a0->y, a3->x, a3->y);
        // SDL_RenderDrawLine(renderer, a0->x, a0->y, c1->x, c1->y);
        // SDL_RenderDrawLine(renderer, c2->x, c2->y, a3->x, a3->y);

        // Quadratic Bezier Curve
        // bezierCurve.clear();
        // for (double t = 0; t <= 1.0001; t += dt) {
        //     Point p1;
        //     p1.x = lerp(a0->x, c1->x, t);
        //     p1.y = lerp(a0->y, c1->y, t);

        //     Point p2;
        //     p2.x = lerp(c1->x, a3->x, t);
        //     p2.y = lerp(c1->y, a3->y, t);

        //     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //     SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

        //     Point newBezierPoint;
        //     newBezierPoint.x = lerp(p1.x, p2.x, t);
        //     newBezierPoint.y = lerp(p1.y, p2.y, t);

        //     bezierCurve.emplace_back(newBezierPoint);
        // }
        // for(auto &p : bezierCurve) drawCircle(renderer, p.x, p.y, 1, Color{255, 0, 0});

        // Cubic Bezier Curve - draw point by point
        // bezierCurve.clear();
        // for (double t = 0; t < 1; t += dt) {
        //     Point newPoint;
        //     newPoint.x = (1 - t) * (1 - t) * (1 - t) * a0->x + 3 * (1 - t) * (1 - t) * t * c1->x + 3 * (1 - t) *
        //     t * t * c2->x + t * t * t * a3->x; newPoint.y = (1 - t) * (1 - t) * (1 - t) * a0->y + 3 * (1 - t) *
        //     (1 - t) * t * c1->y + 3 * (1 - t) * t * t * c2->y + t * t * t * a3->y;
        //     bezierCurve.emplace_back(newPoint);
        // }
        // If we draw all the tangent lines that form the curve, there is no need for drawing the points as well
        // for(auto &p : bezierCurve) drawCircle(renderer, p.x, p.y, 1, Color{255, 0, 0});

        // Cubic Bezier Curve - drawn using tangent lines and interpolationg the 2 cuadratics.
        bezierCurve.clear();
        for (double t = 0; t <= 1.0001; t += dt) bezierCurve.emplace_back(cubic(renderer, *a0, *c1, *c2, *a3, t));
        for (auto& p : bezierCurve) drawCircle(renderer, p.x, p.y, 5, Color{255, 0, 0});

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // find other keys here:
            // http:// sdl.beuc.net/sdl.wiki/SDLKey
            switch (event.type) {
            case SDL_QUIT: running = false; break;
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE: {
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

                if (draggedPoint && mouseClick) {
                    draggedPoint->x = event.motion.x;
                    draggedPoint->y = event.motion.y;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                draggedPoint = nullptr;
                mouseClick = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                mouseClick = true;

                mousePosX = event.motion.x;
                mousePosY = event.motion.y;

                for (auto& p : points) {
                    double dist = std::sqrt((p->x - mousePosX) * (p->x - mousePosX) +
                                            (p->y - mousePosY) * (p->y - mousePosY));
                    if (dist <= 5) {
                        // save point that we want to move;
                        draggedPoint = p;
                    }
                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
