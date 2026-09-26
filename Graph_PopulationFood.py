import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create graph
plt.plot(data["Week"], data["Population"], label="Population")
plt.plot(data["Week"], data["Food"], label="Food Stored")

plt.xlabel("Week")
plt.ylabel("Amount")
plt.title("Population and Food Over Time")

plt.legend()
plt.show()