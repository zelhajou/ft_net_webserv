#!/usr/bin/env python3
import sys
import os

# Print the necessary headers
print("Content-Type: text/plain")
print("Status: 200 OK")
print()  # Blank line to separate headers from body

# Get the content length from the environment variable
content_length = int(os.environ.get('CONTENT_LENGTH', 0))

# Read the body from stdin
body = sys.stdin.read(content_length)

# Print the body back
print("Received body:")
print(body)

# You can also add some additional information
print("\nAdditional information:")
print(f"Content-Length: {content_length}")
print(f"Request Method: {os.environ.get('REQUEST_METHOD', 'Unknown')}")
print(f"Query String: {os.environ.get('QUERY_STRING', 'None')}")