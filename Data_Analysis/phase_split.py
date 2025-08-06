import pandas as pd
import os
stocks=["AAPL","AMZN","MSFT"]
for stock in stocks:
    df = pd.read_csv(f"../data/{stock}.csv", parse_dates=["Date"])
    df["Date"] = pd.to_datetime(df["Date"])
    if df["Date"].dt.tz is not None:
        df["Date"] = df["Date"].dt.tz_localize(None)
    phase1 = df[(df['Date'] >= pd.Timestamp("2016-01-01")) & (df['Date'] <= pd.Timestamp("2018-12-31"))]
    phase1.to_csv(f"../phase1/{stock}.csv", index=False)
    phase2 = df[(df['Date'] >= pd.Timestamp("2020-01-01")) & (df['Date'] <= pd.Timestamp("2020-09-30"))]
    phase2.to_csv(f"../phase2/{stock}.csv", index=False)
    phase3 = df[(df['Date'] >= pd.Timestamp("2022-01-01")) & (df['Date'] <= pd.Timestamp("2023-06-30"))]
    phase3.to_csv(f"../phase3/{stock}.csv", index=False)
