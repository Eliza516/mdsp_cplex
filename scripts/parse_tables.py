import re
input_file = "paper_layout.txt"
line_regex = re.compile(r'^\s*([a-zA-Z0-9\-]+)\s+(\d+)\s+(.*)$')
all_miss = set()
for line in open(input_file):
    line = line.strip()
    if line.startswith("miss-"):
        match = line_regex.match(line)
        if match:
            # check if it looks like table 5-9 data (i.e. has 24 or 36 numbers)
            numbers = re.findall(r'[-+]?\d*\.?\d+', match.group(3))
            if len(numbers) >= 24:
                all_miss.add(match.group(1))

print(f"Total miss in layout with >=24 numbers: {len(all_miss)}")
