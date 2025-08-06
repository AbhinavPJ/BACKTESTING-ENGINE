#pragma once
#include <vector>
#include <chrono>
using Timestamp = int64_t;

struct Clock
{
    std::vector<Timestamp> time_series;
    size_t current_index = 0;
    Clock()
    {
    }
    Clock(const std::vector<Timestamp> &time_series)
    {
        this->time_series = time_series;
    }
    Timestamp now() const
    {
        return time_series[current_index];
    }
    bool has_next() const
    {
        return current_index + 1 < time_series.size();
    }
    void next()
    {
        if (has_next())
            current_index++;
    }
    void reset()
    {
        current_index = 0;
    }
    int index() const
    {
        return static_cast<int>(current_index);
    }
};
