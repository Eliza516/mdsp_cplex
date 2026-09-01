import re
import csv

input_file = "paper_layout.txt"
output_file = "mdsp_paper_results.csv"

methods = ["nIP", "nFEAS", "nMAX", "IP", "FEAS", "MAX", "tIP", "tFEAS", "tMAX"]
headers = ["Instance", "|D|"]
for m in methods:
    headers.extend([f"{m}_LB", f"{m}_UB", f"{m}_gap", f"{m}_time"])

line_regex = re.compile(r'^\s*([a-zA-Z0-9\-]+)\s+(\d+)\s+(.*)$')
rows_extracted = 0

with open(input_file, "r") as f_in, open(output_file, "w", newline='') as f_out:
    writer = csv.writer(f_out)
    writer.writerow(headers)
    
    for line in f_in:
        line = line.strip()
        if any(line.startswith(prefix) for prefix in ["full-", "miss-", "join-", "joint-", "drand-", "luci-", "luciferase-"]):
            match = line_regex.match(line)
            if match:
                instance = match.group(1)
                if instance.startswith("joint-"):
                    instance = instance.replace("joint-", "join-")
                if instance.startswith("luciferase-removed-"):
                    instance = instance.replace("luciferase-removed-", "luci-")
                if instance == "luciferase-full":
                    instance = "luci-full"
                    
                D_val = match.group(2)
                numbers_str = match.group(3)
                numbers = re.findall(r'[-+]?\d*\.?\d+', numbers_str)
                
                # Exclude Table 4 rows which have length < 20
                if len(numbers) >= 20:
                    row = [instance, D_val] + numbers
                    # Pad with empty strings if less than 36
                    while len(row) < len(headers):
                        row.append("")
                    writer.writerow(row)
                    rows_extracted += 1

print(f"Successfully extracted {rows_extracted} rows to {output_file}")
