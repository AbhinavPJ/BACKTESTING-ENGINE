#pragma once
#include <string>
#include <unordered_map>
#include "market.hpp"

class Strategy
{
public:
    virtual ~Strategy()
    {
    }
    virtual std::unordered_map<std::string, double> generate_signals(const Market &market, Timestamp current_time) = 0;
};
