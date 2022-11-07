#include <iostream>
#include <array>
#include <string>
#include <random>

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
			    if (y != j || x !=k)
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
static void PrintGrid(T grid)
{
    for (auto& r : grid)
	    for (auto& c : r)
		    std::cout << c << std::endl;
}

int main()
{
    const uint32_t WIDTH = 32, HEIGHT = 32;
    auto grid = CreateGridNoise<Grid<WIDTH, HEIGHT>>(50.0);
    ApplyCellularAutomaton(grid, 1);
    PrintGrid(grid);
    return 0;
}
