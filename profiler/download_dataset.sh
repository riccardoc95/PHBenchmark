#!/bin/bash

# Google Drive file ID
FILE_ID="1nkNx3gY6DwbqRAUk3jhr5-15mHdTXOQ3"

# Output filename (optional)
OUTPUT_NAME="imgs.zip"

# Download using gdown
gdown --id "$FILE_ID" -O "$OUTPUT_NAME"	
