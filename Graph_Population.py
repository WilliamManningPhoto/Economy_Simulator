import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Year"], data["Population"])

plt.xlabel("Year")
plt.ylabel("Population")
plt.title("Population Over Time")

plt.show()