#include "market.hpp"
#include "clock.hpp"
#include "strategy.hpp"
#include "Backtest.hpp"
#include "Buy_once_never_sell.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <set>
#include <iomanip>
namespace fs = std::filesystem;
std::ofstream error_log("errors.log");
std::ofstream warning_log("warnings.log");
std::ofstream valuation_log("valuations.csv");
int64_t parse_timestamp(const std::string &date_str)
{
    std::tm t = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&t, "%Y-%m-%d");
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    std::time_t time = std::mktime(&t);
    return static_cast<int64_t>(time);
}
std::vector<Candle> load_csv(const std::string &filename)
{
    std::vector<Candle> candles;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(ss, token, ','))
        {
            fields.push_back(token);
        }
        Timestamp ts = parse_timestamp(fields[0]);
        double open = std::stod(fields[1]);
        double high = std::stod(fields[2]);
        double low = std::stod(fields[3]);
        double close = std::stod(fields[4]);
        double volume = std::stod(fields[5]);
        candles.push_back({ts, open, high, low, close, volume});
    }

    return candles;
}

void run_phase_backtest(const std::string &phase_name, const std::string &phase_path)
{
    Market market;
    std::set<Timestamp> all_timestamps;
    std::set<std::string> all_symbols;
    std::string stocks[3] = {"AAPL", "AMZN", "MSFT"};
    for (const auto &symbol : stocks)
    {
        std::string filename = phase_path + symbol + ".csv";
        std::vector<Candle> candles = load_csv(filename);
        if (!candles.empty())
        {
            market.add_company(symbol, candles);
            all_symbols.insert(symbol);
            for (const auto &candle : candles)
                all_timestamps.insert(candle.timestamp);
        }
    }
    std::vector<Timestamp> time_series(all_timestamps.begin(), all_timestamps.end());
    std::sort(time_series.begin(), time_series.end());
    Clock clock(time_series);
    std::vector<std::string> symbols(all_symbols.begin(), all_symbols.end());
    BuyAndHoldStrategy strategy(symbols);
    double initial_cash = 100000.0;
    double tx_cost_pct = 0.001;
    Backtest backtest(clock, market, strategy, initial_cash, tx_cost_pct);
    BacktestResult result = backtest.run();
    for (size_t i = 0; i < result.timestamps.size(); ++i)
    {
        std::time_t t = static_cast<std::time_t>(result.timestamps[i]);
        std::tm tm = *std::localtime(&t);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
        valuation_log << phase_name << "," << buf << "," << result.portfolio_values[i] << "\n";
    }
    if (!result.portfolio_values.empty())
    {
        double initial_value = result.portfolio_values[0];
        double final_value = result.portfolio_values.back();
        double return_pct = ((final_value - initial_value) / initial_value) * 100;
        std::cout << "Initial Value: $" << std::fixed << std::setprecision(2) << initial_value << std::endl;
        std::cout << "Final Value: $" << final_value << std::endl;
        std::cout << "Return: " << std::setprecision(2) << return_pct << "%" << std::endl;
    }
}

int main()
{
    valuation_log << "Phase,Date,PortfolioValue\n";
    std::vector<std::pair<std::string, std::string>> phases = {
        {"Phase 1", "../phase1"},
        {"Phase 2", "../phase2"},
        {"Phase 3", "../phase3"}};
    for (const auto &[phase_name, phase_path] : phases)
    {
        run_phase_backtest(phase_name, phase_path);
    }
    error_log.close();
    warning_log.close();
    valuation_log.close();
    return 0;
}
