#!/bin/bash

# Check if the folder path is provided
if [ -z "$1" ]; then
    echo "Please provide a folder path."
    exit 1
fi

# Get the folder path from the first argument
folder="$1"

# Check if the provided folder exists
if [ ! -d "$folder" ]; then
    echo "The provided path is not a valid directory."
    exit 1
fi

# Collect all files in the folder (excluding .bak files)
files=()
for file in "$folder"/*; do
    if [[ ! "$file" =~ \.bak$ ]]; then
        files+=("$file")
    fi
done

# Check if there are any files in the folder
if [ ${#files[@]} -eq 0 ]; then
    echo "No files found in the folder."
    exit 1
fi

# Run ./bank with all the files as arguments
echo "./bank 10 ${files[@]}"

