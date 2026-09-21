import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Year"], data["Price"])

plt.xlabel("Year")
plt.ylabel("Price")
plt.title("Food Price Over Time")

plt.show()