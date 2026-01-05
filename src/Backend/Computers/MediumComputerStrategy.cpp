#include "MediumComputerStrategy.h"
#include <vector>
#include <random>

Coordinates MediumComputerStrategy::CalculateFireCoordinates(
    ISegment& board,
    const Computer& self) 
const {
    size_t w = board.Width(), h = board.Height();
    // 100%: checkerboard
    std::vector<Coordinates> pool;
    for (size_t y = 0; y < h; ++y)
        for (size_t x = 0; x < w; ++x)
            if ((x + y) % 2 == 0 && !self.HasFiredAt(x,y))
                pool.push_back({x,y});

    if (pool.empty()) {
        for (size_t y=0;y<h;y++)
            for (size_t x=0;x<w;x++)
                if (!self.HasFiredAt(x,y))
                    pool.push_back({x,y});
    }

    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0,pool.size()-1);
    return pool[dist(rng)];
}
