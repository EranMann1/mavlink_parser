from pathlib import Path

def clean_dump(input_path, output_path):
    # Read the input file and process
    with open(input_path, 'r', encoding='utf-8') as infile:
        # Remove the first 25 chars from each line and remove spaces
        data = ''.join(line[9:50].strip().replace(' ', '') for line in infile)
    
    # Split into lines of 32 characters
    lines = [data[i:i+32] for i in range(0, len(data), 32)]
    
    # Write to the output file
    with open(output_path, 'w', encoding='utf-8') as outfile:
        outfile.write('\n'.join(lines[:-1]))


if __name__ == "__main__":
    
    for path in Path(".").rglob("hexa_dump*.txt"):
        clean_dump(path, f"{path.parent}/clean_{path.name}")

