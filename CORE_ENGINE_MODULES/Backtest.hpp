#pragma once
#include "clock.hpp"
#include "market.hpp"
#include "portfolio.hpp"
#include "strategy.hpp"
#include <vector>
#include <iostream>
struct BacktestResult
{
    std::vector<Timestamp> timestamps;
    std::vector<double> portfolio_values;
};
class Backtest
{
private:
    Clock clock;
    const Market &market;
    Strategy &strategy;
    Portfolio portfolio;

public:
    Backtest(const Clock &clock, const Market &market, Strategy &strategy, double initial_cash, double transaction_cost_pct = 0.001)
        : clock(clock),
          market(market),
          strategy(strategy),
          portfolio(initial_cash, transaction_cost_pct)
    {
    }
    BacktestResult run()
    {
        BacktestResult result;
        while (clock.has_next())
        {
            Timestamp now = clock.now();
            std::unordered_map<std::string, double> desired_alloc = strategy.generate_signals(market, now);
            double total_value = portfolio.total_value(market, now);
            for (const auto &[symbol, target_weight] : desired_alloc)
            {
                const Candle &candle = market.get_candle(symbol, now);
                double current_qty = portfolio.get_holding(symbol);
                double current_val = current_qty * candle.close;
                double target_val = total_value * target_weight;
                double target_qty = target_val / candle.close;
                double delta_qty = target_qty - current_qty;
                if (std::abs(delta_qty) * candle.close < 1e-6)
                    continue;
                if (delta_qty > 0)
                    portfolio.buy(symbol, delta_qty, candle.close);
                else
                    portfolio.sell(symbol, -delta_qty, candle.close);
            }
            result.timestamps.push_back(now);
            result.portfolio_values.push_back(portfolio.total_value(market, now));
            clock.next();
        }

        return result;
    }
};
