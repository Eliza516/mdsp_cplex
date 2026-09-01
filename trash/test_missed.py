import re
row_regex = re.compile(r'^\s*([a-zA-Z0-9\-]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s+(\d+)\s+([\d\.]+)\s*$')

in_table = False
with open("temp_pdf.txt") as f:
    for line in f:
        if "Table 4: Results preprocessing" in line:
            in_table = True
            continue
        if "Table 5:" in line:
            break
        if in_table:
            line = line.strip()
            if line.startswith("full") or line.startswith("miss") or line.startswith("joint") or line.startswith("drand"):
                if not row_regex.match(line):
                    print("Missed:", line)
