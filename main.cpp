#include <iostream>
#include <array>
#include <string>
#include <random>

#include <SDL.h>

static const uint32_t SCREEN_WIDTH = 1280;
static const uint32_t SCREEN_HEIGHT = 640;

static SDL_Window* s_Window = nullptr;
static SDL_Renderer* s_Renderer = nullptr;

template<int W, int H>
using Grid = std::array<std::array<std::string, W>, H>;

template<typename T>
static auto CreateGridNoise(double density)
{
    T grid = {};

    std::random_device rd;
    std::mt19937_64 gen64(rd());
    std::uniform_real_distribution uniformDist(1.0, 100.0);

    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            auto random = uniformDist(gen64);
            if (random > density) 
                grid[i][j] = "Floor";
            else
                grid[i][j] = "Wall";
        }
    }

    return grid;
}

template<typename T>
static void ApplyCellularAutomaton(T grid, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        auto tempGrid = grid;
        for (size_t j = 0; j < grid.size(); j++)
        {
            for (size_t k = 0; k < grid[0].size(); k++)
            {
                auto neighborWallCount = 0;
                for (size_t y = j - 1; y < j + 1; y++)
                {
                    for (size_t x = k - 1; x < k + 1; x++)
                    {
                        if (y >= 1 && x >= 1 && y <= grid.size() && x <= grid[0].size())
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
                else
                    grid[j][k] = "Floor";
            }
        }
    }
}

template<typename T>
static void RenderGrid(T grid)
{
    int xoffset = 250;
    int yoffset = 250;
    int halfScreenPosX = (SCREEN_WIDTH / 2) - xoffset;
    int halfScreenPosY = (SCREEN_HEIGHT / 2) - yoffset;

    for (int y = 0; y < grid.size(); y++)
    {
        for (int x = 0; x < grid[0].size(); x++)
        {
            if (grid[y][x] == "Wall")
            {
                SDL_Rect s_BlackTile = { .x=(x * 16) + halfScreenPosX, .y=(y * 16) + halfScreenPosY, .w=16, .h=16 };
                SDL_SetRenderDrawColor(s_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(s_Renderer, &s_BlackTile);
            }
            else
            {
                SDL_Rect s_WhiteTile = { .x=(x * 16) + halfScreenPosX, .y=(y * 16) + halfScreenPosY, .w=16, .h=16 };
                SDL_SetRenderDrawColor(s_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(s_Renderer, &s_WhiteTile);
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

    const uint32_t WIDTH = 32, HEIGHT = 32;
    auto grid = CreateGridNoise<Grid<WIDTH, HEIGHT>>(50.0);
    ApplyCellularAutomaton(grid, 40);
    
    bool running = true;
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
            }
        }

        SDL_RenderClear(s_Renderer);
        RenderGrid(grid);
        SDL_RenderPresent(s_Renderer);
    }

    ShutdownSDL();
}

int main(int argc, char* arvg[])
{
    RunApplication();
    return 0;
}
