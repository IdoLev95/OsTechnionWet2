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

# Iterate through all files in the folder
for file in "$folder"/*; do
    # Check if it's a file (skip directories)
    if [ -f "$file" ]; then
        # Print the "running" message
        echo "Running: $file"
        
        # Run ./bank with the file as an argument
        ./bank 10 "$file"
    fi
done

