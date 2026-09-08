
#!/bin/bash

# Check that both arguments were provided
if [ $# -ne 2 ]; then
    echo "Error: two arguments are required: filesdir and searchstr"
    exit 1
fi

filesdir="$1"
searchstr="$2"

echo "Inputs are: filesdir $filesdir and searchstr $searchstr"

# Check that filesdir is a directory
if [ ! -d "$filesdir" ]; then
    echo "Error: '$filesdir' is not a directory"
    exit 1
fi

# # Count all regular files recursively
file_count=$(find "$filesdir" -type f | wc -l)

# # Count all matching lines recursively
matching_lines=$(grep -r -h -F "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $file_count and the number of matching lines are $matching_lines"

exit 0