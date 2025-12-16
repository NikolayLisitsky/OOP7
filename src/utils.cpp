#include "../include/utils.h"
#include <chrono>
#include <random>
#include <cmath>

std::pair<int, int> randomDirection(int length)
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 2.0f * M_PI);

    float angle = dist(rng);
    return { static_cast<int>(std::cos(angle) * length), static_cast<int>(std::sin(angle) * length) };
}

double seconds_since_start(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - start).count();
}

bool game_is_over(std::chrono::steady_clock::time_point start)
{
    return seconds_since_start(start) >= GAME_DURATION_SECONDS;
}
