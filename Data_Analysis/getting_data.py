import yfinance as yf
import os
stocks = ["AAPL", "MSFT", "AMZN"]
start_date = "2010-01-01"
end_date = "2023-12-31"
for stock in stocks:
    df = yf.Ticker(stock).history(start=start_date, end=end_date, interval="1d")
    df = df.reset_index()
    df.to_csv(f"../data/{stock}.csv", index=False)
