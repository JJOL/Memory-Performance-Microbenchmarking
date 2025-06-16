import pandas as pd
import sys
def main():
    if len(sys.argv) != 2:
        print("Usage: python cache_report.py <csv file>")
        return
    file = sys.argv[1]

    df = pd.read_csv(file, skiprows=1)
    l1_lat = df[df['Data Size (KB)'] <= 48]['Latency (ns)'].mean()
    l2_lat = df[(48 < df['Data Size (KB)']) &(df['Data Size (KB)'] <= 2048)]['Latency (ns)'].mean()
    l3_lat = df[(2048 < df['Data Size (KB)']) &(df['Data Size (KB)'] <= 131072)]['Latency (ns)'].mean()
    
    print(f"L1 mean latency: {l1_lat:.4f} (ns)\t1x")
    print(f"L2 mean latency: {l2_lat:.4f} (ns)\t{(l2_lat / l1_lat):.2f}x")
    print(f"L3 mean latency: {l3_lat:.4f} (ns)\t{(l3_lat / l2_lat):.2f}x")
    

if __name__ == "__main__":
    main()