import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Define the data
data = {
    "Function": ["Copy", "Scale", "Add", "Triad"],
    "Local": [186460.1, 191548.1, 190747.7, 193130.1],
    "Remote": [119251.6, 92202.8, 109768.6, 110839.5],
    "Interleave": [282041.1, 250191.5, 280735.4, 283726.4]
}

# Convert MB/s to GB/s
for key in ["Local", "Remote", "Interleave"]:
    data[key] = [x / 1024 for x in data[key]]

# Create DataFrame
df = pd.DataFrame(data)

# Define NUMA groups and functions
numa_configs = ["Local", "Remote", "Interleave"]
functions = df["Function"].tolist()
x = np.arange(len(numa_configs))  # 3 groups

# Bar width and offset for each function within each group
bar_width = 0.2
offsets = [-1.5 * bar_width, -0.5 * bar_width, 0.5 * bar_width, 1.5 * bar_width]

# Plot setup
fig, ax = plt.subplots(figsize=(10, 6))

# Plot each function as a separate bar per group
for idx, func in enumerate(functions):
    values = [df.loc[df["Function"] == func, config].values[0] for config in numa_configs]
    ax.bar(x + offsets[idx], values, width=bar_width, label=func)

# Format plot
ax.set_xlabel("NUMA Configuration")
ax.set_ylabel("Best Rate (GB/s)")
ax.set_title("STREAM Benchmark - Best Rate GB/s by Function and NUMA Configuration")
ax.set_xticks(x)
ax.set_xticklabels(numa_configs)
ax.legend(title="Function")
ax.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.savefig('stream bars.png')