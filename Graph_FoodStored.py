import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Week"], data["Food"])

plt.xlabel("Week")
plt.ylabel("FoodStores")
plt.title("Food Stored Over Time")

plt.show()