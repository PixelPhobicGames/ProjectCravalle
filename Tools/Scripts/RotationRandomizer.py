import sys
import random

# Check if the input file path is provided as a command line argument
if len(sys.argv) != 2:
    print("Usage: python script.py input_file.txt")
    sys.exit(1)

input_file_path = sys.argv[1]

# Read the content of the input file
with open(input_file_path, 'r') as file:
    file_content = file.read()

# Split the file content into model descriptors
model_descriptors = file_content.split("Model")

# Modify and build the updated content
updated_content = ""
for descriptor in model_descriptors:
    # Split the descriptor into its components
    components = descriptor.split(":")
    if len(components) >= 6:
        # Check if the fifth value is '1.000000'
        if components[5] == '1.000000':
            # Generate a random integer between 1 and 50
            random_number = random.randint(1, 360)
            components[5] = str(random_number)

        # Reconstruct the descriptor with the updated fifth value
        updated_descriptor = ':'.join(components)

        # Append the updated descriptor to the updated content
        updated_content += "Model" + updated_descriptor

# Write the updated content back to the file
with open(input_file_path, 'w') as file:
    file.write(updated_content)
