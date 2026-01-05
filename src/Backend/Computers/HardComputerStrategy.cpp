#include "HardComputerStrategy.h"
#include <vector>
#include <random>

Coordinates HardComputerStrategy::CalculateFireCoordinates(
    ISegment& board,
    const Computer& self) 
const {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> chance(0.0,1.0);

    size_t w = board.Width(), h = board.Height();
    const auto& grid = board.Segments();

    // 20%: cheatin'
    if (chance(rng) <= 0.20) {
        std::vector<Coordinates> targets;
        for (size_t y=0;y<h;y++)
            for (size_t x=0;x<w;x++)
                if (grid[y][x] && !self.HasFiredAt(x,y))
                    targets.push_back({x,y});

        if (!targets.empty()) {
            std::uniform_int_distribution<size_t> d(0,targets.size()-1);
            return targets[d(rng)];
        }
    }

    // 80%: checkerboard
    std::vector<Coordinates> pool;
    for (size_t y=0;y<h;y++)
        for (size_t x=0;x<w;x++)
            if ((x+y)%2==0 && !self.HasFiredAt(x,y))
                pool.push_back({x,y});

    if (pool.empty()) {
        for (size_t y=0;y<h;y++)
            for (size_t x=0;x<w;x++)
                if (!self.HasFiredAt(x,y))
                    pool.push_back({x,y});
    }

    std::uniform_int_distribution<size_t> d(0,pool.size()-1);
    return pool[d(rng)];
}
