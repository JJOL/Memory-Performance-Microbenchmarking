import pandas as pd
import matplotlib.pyplot as plt
from io import StringIO
import numpy as np

# ————————————————————————————————
# 1) Load your data
# Option A: Paste your block into raw_data:
raw_data = """
2 KB, 1x parallelism, 2392.308350 MB/s
2 KB, 2x parallelism, 3188.232910 MB/s
# … (all your lines here) …
4096 KB, 32x parallelism, 2347.417725 MB/s
"""

# Option B: Or save your block to 'cache_data.csv' with columns: size,parallelism,bandwidth
# df = pd.read_csv('cache_data.csv', names=['size','parallelism','bandwidth'])

# Using Option A here:
df = pd.read_csv(
    "memlatency_parallel.csv",
    sep=",",
    names=["size", "parallelism", "bandwidth"],
    engine="python"
)

# ————————————————————————————————
# 2) Clean & convert types
df["size"] = df["size"].str.strip()
df["parallelism"] = (
    df["parallelism"]
      .str.replace("x parallelism", "", regex=False)
      .astype(int)
)
df["bandwidth"] = (
    df["bandwidth"]
      .str.replace("MB/s", "", regex=False)
      .astype(float)
)
# Extract numeric size in KB for plotting labels
df["size_kb"] = df["size"].str.replace(" KB", "", regex=False).astype(int)

# ————————————————————————————————
# 3) Pivot to wide form: each size → its own column
pivot_df = df.pivot(
    index="parallelism",
    columns="size_kb",
    values="bandwidth"
)

# Number of distinct sizes
n_sizes = len(pivot_df.columns)

# Choose a perceptually uniform colormap
cmap = plt.cm.viridis

# Generate a color for each size
colors = cmap(np.linspace(0, 1, n_sizes))

# Optional: a few line styles to mix in
line_styles = ['-', '--', '-.', ':']
ls_cycle = (line_styles[i % len(line_styles)] for i in range(n_sizes))

# ————————————————————————————————
# 4) Plot
plt.figure(figsize=(10, 6))
for (sz, color) in zip(sorted(pivot_df.columns), colors):
    plt.plot(
        pivot_df.index,
        pivot_df[sz],
        label=f"{sz} KB",
        color=color,
        linewidth=2,
        alpha=0.8
    )

plt.xlabel("Parallelism (streams)")
plt.ylabel("Bandwidth (MB/s)")
plt.title("Cache Bandwidth vs Parallelism for Various Array Sizes")
plt.legend(title="Array Size")
plt.grid(True)
plt.tight_layout()
plt.savefig('Parallel Cache.png')