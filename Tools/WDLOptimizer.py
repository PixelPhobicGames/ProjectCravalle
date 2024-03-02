import re
import sys

def is_float(value):
    try:
        float(value)
        return True
    except ValueError:
        return False

def round_floats_in_file(input_file, output_file):
    with open(input_file, 'r') as file:
        data = file.read()

    print("Original Data:\n", data)

    Chunks = data.split(":")
    new_chunks = []

    for s in Chunks:
        if is_float(s):
            print("Original float:", s)
            rounded = round(float(s), 2)
            print("Rounded float:", rounded)
            new_chunks.append(str(rounded))
        else:
            new_chunks.append(s)

    new_data = ":".join(new_chunks)

    print("\nNew Data:\n", new_data)

    with open(output_file, 'w') as file:
        file.write(new_data)


# Replace 'input.txt' and 'output.txt' with your file names
round_floats_in_file(sys.argv[1], 'WorldOptimized.wdl')