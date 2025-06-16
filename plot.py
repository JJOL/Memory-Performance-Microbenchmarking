import sys
import pandas as pd
import matplotlib.pyplot as plt
import os

# Check command-line arguments
if len(sys.argv) != 2:
    print("Usage: python plot.py <filename.csv>")
    sys.exit(1)

input_file = sys.argv[1]
base_name = os.path.splitext(os.path.basename(input_file))[0]
output_file = f"{base_name}.png"

# Step 1: Read the first line (title)
with open(input_file, 'r', encoding='utf-8-sig') as f:
    plot_title = f.readline().strip()

# Step 2: Load the rest of the data, using the second row as headers
df = pd.read_csv(input_file, skiprows=1)
df.columns = df.columns.str.strip()

# Extract axis names from header
x_label = df.columns[0]
y_label = df.columns[1]
label_column = df.columns[2]

print(df)

# Step 3: Plot each label group
plt.figure(figsize=(12, 7))
show_expotential_increasing = False
if not show_expotential_increasing:
    for label, group in df.groupby(label_column):
        if label.startswith("_"): label = label[1:]
        plt.plot(group[x_label], group[y_label], marker='o', label=label)

    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(plot_title)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

else:
    # Create a categorical x-axis with 18 evenly spaced indices (0 to 17)
    ref = 'Latency'
    x_positions = range(len(df[df['Label'] == ref]))
    x_labels = df[df['Label'] == ref][x_label].tolist()  # keep actual values as labels

    for label, group in df.groupby(label_column):
        print('Label: ' + label)
        if label.startswith("_"): label = label[1:]
        plt.plot(x_positions, group[y_label], marker='o', label=label)

    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(plot_title)
    plt.xticks(ticks=x_positions, labels=x_labels, rotation=45)  # set actual x labels
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

# Step 4: Save the plot
plt.savefig(output_file)
print(f"Plot saved as: {output_file}")

