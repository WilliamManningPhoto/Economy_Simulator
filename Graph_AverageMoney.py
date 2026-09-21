import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Year"], data["AverageMoney"])

plt.xlabel("Year")
plt.ylabel("AverageMoney")
plt.title("Average Money  Over Time")

plt.show()