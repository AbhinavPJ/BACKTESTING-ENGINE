import pandas as pd
import matplotlib.pyplot as plt
df = pd.read_csv("valuations.csv")
df['Date'] = pd.to_datetime(df['Date'])
plt.figure(figsize=(12, 6))
for phase in df['Phase'].unique():
    phase_data = df[df['Phase'] == phase]
    plt.plot(phase_data['Date'], phase_data['PortfolioValue'], label=phase)
plt.title("Portfolio Value Over Time by Phase")
plt.xlabel("Date")
plt.ylabel("Portfolio Value ($)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("portfolio_by_phase.png")
plt.show()
