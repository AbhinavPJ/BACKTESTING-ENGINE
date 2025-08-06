#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
using Timestamp = int64_t;
struct Candle
{
    Timestamp timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
    Candle()
    {
    }
    Candle(Timestamp ts, double o, double h, double l, double c, double v)
    {
        this->timestamp = ts;
        this->high = h;
        this->low = l;
        this->open = o;
        this->volume = v;
        this->close = c;
    }
};
struct Company
{
    std::string symbol;
    std::vector<Candle> candles;
    std::unordered_map<Timestamp, int> ts_index_map;

    Company()
    {
    }

    Company(const std::string &sym, const std::vector<Candle> &data)
    {
        this->symbol = sym;
        this->candles = data;
        for (int i = 0; i < candles.size(); ++i)
            ts_index_map[candles[i].timestamp] = i;
    }

    void add_bar(const Candle &candle)
    {
        ts_index_map[candle.timestamp] = candles.size();
        candles.push_back(candle);
    }

    const Candle *get_candle(Timestamp ts) const
    {
        auto it = ts_index_map.find(ts);
        if (it == ts_index_map.end())
            return nullptr;
        return &candles[it->second];
    }
    bool has_candle(Timestamp ts) const
    {
        return ts_index_map.count(ts) > 0;
    }
};

struct Market
{
    std::unordered_map<std::string, Company> Companies;
    Market()
    {
    }
    void add_company(const std::string &symbol, const std::vector<Candle> &data)
    {
        Companies[symbol] = Company(symbol, data);
    }
    const Candle &get_candle(const std::string &symbol, Timestamp ts) const
    {
        auto it = Companies.find(symbol);
        if (it == Companies.end())
            throw std::out_of_range("Company not found");

        const Candle *candle = it->second.get_candle(ts);
        if (!candle)
            throw std::out_of_range("Candle not found at given timestamp");

        return *candle;
    }
};
