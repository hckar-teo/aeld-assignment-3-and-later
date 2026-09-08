#!/bin/bash

# Check that both arguments were provided
if [ $# -ne 2 ]; then
    echo "Error: two arguments are required: writefile and writestr"
    exit 1
fi

writefile="$1"
writestr="$2"

echo "Inputs are: writefile $writefile and writestr $writestr"

# Create the parent directory if it does not exist
parent_dir=$(dirname "$writefile")

if [ ! -d "$parent_dir" ]; then
    mkdir -p "$parent_dir"

    if [ $? -ne 0 ]; then
        echo "Error: could not create directory '$parent_dir'"
        exit 1
    fi
fi

# Write the string to the file, overwriting any existing content
echo "$writestr" > "$writefile"

# Check whether the file was successfully created/written
if [ $? -ne 0 ]; then
    echo "Error: could not create file '$writefile'"
    exit 1
fi

exit 0