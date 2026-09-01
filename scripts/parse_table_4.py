import re
import csv

input_file = "temp_pdf.txt"
output_file = "table_4_extracted.csv"

# Header for the CSV
header = [
    "Instances", "Size", "Trivial_tLB", "Trivial_tUB", 
    "HD_HD", "HD_Time", 
    "LB_nLB", "LB_Time", 
    "UB_nUB", "UB_Time", 
    "DUB_nDUB", "DUB_Time"
]

in_table = False
data_rows = []

# Regex to match a data row: starts with a string, followed by numbers
# e.g., "full-5-30-1   10   5   11   0   0.0   5   0.1   6   13.5   5   12.5"
row_regex = re.compile(r'^\s*([a-zA-Z0-9\-]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s*$')

with open(input_file, 'r') as f:
    for line in f:
        # Check if we are starting Table 4
        if "Table 4: Results preprocessing" in line:
            in_table = True
            continue
        
        # If we hit the next table, we can stop
        if "Table 5:" in line:
            break
            
        if in_table:
            match = row_regex.match(line)
            if match:
                data_rows.append(match.groups())

with open(output_file, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    writer.writerows(data_rows)

print(f"Extracted {len(data_rows)} rows to {output_file}")
