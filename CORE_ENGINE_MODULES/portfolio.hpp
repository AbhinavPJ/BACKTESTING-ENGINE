#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "market.hpp"
#include <iostream>
class Portfolio
{
private:
    double cash;
    double transaction_cost_pct;
    std::unordered_map<std::string, double> holdings;

public:
    Portfolio(double initial_cash, double transaction_cost_pct)
    {
        this->cash = initial_cash;
        this->transaction_cost_pct = transaction_cost_pct;
    }
    double get_cash() const
    {
        return this->cash;
    }
    double get_holding(const std::string &symbol) const
    {
        auto it = this->holdings.find(symbol);
        if (it == this->holdings.end())
            return 0.0;
        return it->second;
    }
    const std::unordered_map<std::string, double> &get_all_holdings() const
    {
        return this->holdings;
    }
    void buy(const std::string &symbol, double quantity, double price)
    {
        double cost = quantity * price * (1.0 + transaction_cost_pct);
        if (cost > cash)
        {
            std::cout << quantity << std::endl;
            std::cout << cash << std::endl;
            throw std::runtime_error("Insufficient cash to buy");
        }

        cash -= cost;
        holdings[symbol] += quantity;
    }

    void sell(const std::string &symbol, double quantity, double price)
    {
        if (holdings[symbol] < quantity)
            throw std::runtime_error("Insufficient shares to sell");

        double proceeds = quantity * price * (1.0 - transaction_cost_pct);
        cash += proceeds;
        holdings[symbol] -= quantity;

        if (holdings[symbol] <= 1e-8)
            holdings.erase(symbol);
    }

    double total_value(const Market &market, Timestamp ts) const
    {
        double value = cash;
        for (const auto &[symbol, quantity] : holdings)
        {
            const Candle &candle = market.get_candle(symbol, ts);
            value += quantity * candle.close;
        }
        return value;
    }
};
