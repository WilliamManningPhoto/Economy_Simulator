import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV produced by my simulation
data = pd.read_csv("simulation.csv")

print(data.columns)
print(data.head())

# Create population graph
plt.plot(data["Week"], data["AgeDeath"], label="Age Death")
plt.plot(data["Week"], data["Starvation"], label="Starvation")

plt.xlabel("Week")
plt.ylabel("Deaths")
plt.title("Deaths Over Time")

plt.legend()

plt.show()