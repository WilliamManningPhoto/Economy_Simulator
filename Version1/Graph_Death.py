import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Year"], data["AgeDeath"], label="Age Death")
plt.plot(data["Year"], data["Starvation"], label="Starvation")
plt.plot(data["Year"], data["PlagueDeath"], label="Plague Death")

plt.xlabel("Year")
plt.ylabel("Deaths")
plt.title("Deaths Over Time")

plt.legend()

plt.show()