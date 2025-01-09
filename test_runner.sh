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

# Collect all files in the folder and run ./bank with all the filenames as arguments
files=("$folder"/*)

# Check if there are any files in the folder
if [ ${#files[@]} -eq 0 ]; then
    echo "No files found in the folder."
    exit 1
fi

# Run ./bank with all files as arguments
./bank 10 "${files[@]}"

