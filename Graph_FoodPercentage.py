import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create food stored percentage graph
plt.plot(data["Year"], data["FoodPercentage"])

plt.xlabel("Year")
plt.ylabel("FoodStores")
plt.title("Food Percentage Stored Over Time")

plt.show()