#include <iostream>
#include <string>
#include <random>
#include <array>

#include <SDL.h>

static const uint32_t SCREEN_WIDTH = 1280;
static const uint32_t SCREEN_HEIGHT = 640;
static const uint32_t WIDTH = 160, HEIGHT = 90;

static SDL_Window* s_Window = nullptr;
static SDL_Renderer* s_Renderer = nullptr;

struct Camera
{
    float x, y;
    uint32_t w, h;
};

struct Player
{
    float x, y;
};

static Camera camera;
static Player player;

static void GenerateNoise(std::array<std::array<std::string, 160>, 90>& grid, double noiseDensity)
{
    std::random_device rd;
    std::mt19937_64 gen64(rd());
    std::uniform_int_distribution uniformDist(1, 100);

    for (size_t j = 0; j < HEIGHT; j++)
    {
        for (size_t i = 0; i < WIDTH; i++)
        {
            auto random = uniformDist(gen64);
            if (random > noiseDensity)
            {
                grid[j][i] = "Floor";
            }
            else
            {
                grid[j][i] = "Wall";
            }
        }
    }
}


static void RenderGrid(const std::array<std::array<std::string, 160>, 90>& grid)
{
    int xoffset = 250;
    int yoffset = 250;
    int halfScreenPosX = (SCREEN_WIDTH / 2) - xoffset;
    int halfScreenPosY = (SCREEN_HEIGHT / 2) - yoffset;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (grid[y][x] == "Wall")
            {
                SDL_Rect s_BlackTile = { .x=(x * 16) - (int)camera.x, .y=(y * 16) - (int)camera.y, .w=16, .h=16 };
                SDL_SetRenderDrawColor(s_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(s_Renderer, &s_BlackTile);
            }
            else
            {
                SDL_Rect s_WhiteTile = { .x=(x * 16) - (int)camera.x, .y=(y * 16) - (int)camera.y, .w=16, .h=16 };
                SDL_SetRenderDrawColor(s_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(s_Renderer, &s_WhiteTile);
            }
        }
    }
}

static void ApplyCellularAutomaton(std::array<std::array<std::string, 160>, 90>& grid, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        std::array<std::array<std::string, 160>, 90>& tempGrid = grid;


        for (size_t j = 0; j < HEIGHT; j++)
        {
            for (size_t k = 0; k < WIDTH; k++)
            {
                auto neighborWallCount = 0;
                for (size_t y = j - 1; y <= j + 1; y++)
                {
                    for (size_t x = k - 1; x <= k + 1; x++)
                    {
                        if (y >= 0 && x >= 0 && y < HEIGHT && x < WIDTH)
                        {
                            if (y != j || x != k)
                                if (tempGrid[y][x] == "Wall")
                                    neighborWallCount++;
                        }
                        else
                            neighborWallCount++;
                    }
                }
                if (neighborWallCount > 4)
                    grid[j][k] = "Wall";
                else if (neighborWallCount < 4)
                    grid[j][k] = "Floor";
            }
        }
    }
}


static void InitSDL()
{
    SDL_Init(SDL_INIT_VIDEO);

    s_Window = SDL_CreateWindow("Procedural Generation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    s_Renderer = SDL_CreateRenderer(s_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

static void ShutdownSDL()
{
    SDL_DestroyRenderer(s_Renderer);
    SDL_DestroyWindow(s_Window);
    SDL_Quit();
}




static void RunApplication()
{
    InitSDL();

    bool running = true;
    uint32_t count = 0;
    double noise = 58.0;

    std::array<std::array<std::string, WIDTH>, HEIGHT> grid = { "" };
    GenerateNoise(grid, noise);
    ApplyCellularAutomaton(grid, count);

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                running = false;
                break;
             case SDL_KEYDOWN:
                {
                    std::cout << "Noise Value: " << noise << std::endl;
                    std::cout << "Iteration count: " << count << std::endl;


                    switch (e.key.keysym.sym)
                    {
                    case SDLK_0:
                        {
                            ApplyCellularAutomaton(grid, 1);
                            break;
                        }
                    case SDLK_UP:
                        {
                            noise > 100.0f ? (noise = 100.0f) : noise++;
                            GenerateNoise(grid, noise);
                            ApplyCellularAutomaton(grid, count);
                            break;
                        }
                    case SDLK_DOWN:
                        {
                            noise < 0.0f ? (noise = 0.0f) : noise--;
                            GenerateNoise(grid, noise);
                            ApplyCellularAutomaton(grid, count);
                            break;
                        }
                    case SDLK_k:
                        {
                            count > 100 ? (count = 100) : count++;
                            GenerateNoise(grid, noise);
                            ApplyCellularAutomaton(grid, count);
                            break;
                        }
                    case SDLK_j:
                        {
                            count < 1 ? (count = 1) : count--;
                            GenerateNoise(grid, noise);
                            ApplyCellularAutomaton(grid, count);
                            break;
                        }
                    }
                    
                    
                }
                break;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        const int SPEED = 5;

        if (state[SDL_SCANCODE_W])
        {
            player.y -= 1.0f * SPEED * 0.5f;
        }

        if (state[SDL_SCANCODE_S])
        {
            player.y += 1.0f * SPEED * 0.5f;
        }

        if (state[SDL_SCANCODE_A])
        {
            player.x -= 1.0f * SPEED * 0.5f;
        }

        if (state[SDL_SCANCODE_D])
        {
            player.x += 1.0f * SPEED * 0.5f;
        }

        camera.x = player.x - SCREEN_WIDTH / 2;
        camera.y = player.y - SCREEN_HEIGHT/ 2;


        SDL_RenderPresent(s_Renderer);
        SDL_SetRenderDrawColor(s_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(s_Renderer);
        RenderGrid(grid);
    }

    ShutdownSDL();
}

int main(int argc, char* arvg[])
{
    RunApplication();
    return 0;
}
