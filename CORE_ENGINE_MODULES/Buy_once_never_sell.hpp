#pragma once
#include "strategy.hpp"
#include <iostream>
#include <iomanip>

class BuyAndHoldStrategy : public Strategy
{
private:
    std::vector<std::string> symbols;
    bool first_call = true;

public:
    BuyAndHoldStrategy(const std::vector<std::string> &symbols)
        : symbols(symbols) {}

    std::unordered_map<std::string, double> generate_signals(const Market &market, Timestamp current_time) override
    {
        std::unordered_map<std::string, double> alloc;

        // Only allocate once at the beginning
        if (!first_call)
        {
            return alloc; // Return empty after first allocation
        }

        int available_symbols = 0;
        std::vector<std::pair<std::string, double>> symbol_prices;

        for (const auto &symbol : symbols)
        {
            if (market.Companies.at(symbol).has_candle(current_time))
            {
                const Candle *candle = market.Companies.at(symbol).get_candle(current_time);
                if (candle)
                {
                    available_symbols++;
                    symbol_prices.push_back({symbol, candle->close});
                    std::cout << "Symbol: " << symbol << ", Price: $" << std::fixed << std::setprecision(2) << candle->close << std::endl;
                }
            }
        }

        std::cout << "Available symbols: " << available_symbols << std::endl;

        if (available_symbols == 0)
        {
            std::cout << "No symbols available at current time" << std::endl;
            return alloc;
        }

        // Use a conservative allocation to avoid cash issues
        double total_allocation = 0.95; // Use 95% of cash, keep 5% buffer
        double weight_per_symbol = total_allocation / available_symbols;

        std::cout << "Allocation per symbol: " << std::fixed << std::setprecision(4) << weight_per_symbol << std::endl;

        for (const auto &[symbol, price] : symbol_prices)
        {
            alloc[symbol] = weight_per_symbol;
            std::cout << "Allocating " << (weight_per_symbol * 100) << "% to " << symbol << std::endl;
        }

        first_call = false;
        return alloc;
    }
};