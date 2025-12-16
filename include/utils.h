#pragma once

#include <cmath>
#include <chrono>
#include <mutex>
#include <iostream>
#include <sstream>

#define GAME_DURATION_SECONDS 30.0
    
std::pair<int, int> randomDirection(int length);

bool game_is_over(std::chrono::steady_clock::time_point start);
double seconds_since_start(std::chrono::steady_clock::time_point start);

struct my_print
{
    inline static std::mutex mtx;
    std::stringstream ss;

    template<typename T>
    my_print& operator<<(T&& value)
    {
        ss << std::forward<T>(value);
        return *this;
    }

    my_print& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(ss);
        return *this;
    }

    ~my_print()
    {
        std::lock_guard<std::mutex> lck(mtx);
        std::cout << ss.str();
        std::cout.flush();
    }
};