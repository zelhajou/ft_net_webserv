import cgi

# Retrieve form data from environment variables (POST)
form = cgi.FieldStorage()
username = form.getvalue('username')
message = form.getvalue('message')

# Process data (example: store in a file)
with open('user_data.txt', 'a') as f:
  f.write(f"{username}: {message}\n")

# Generate response content (HTML)
response_body = f"""
<!DOCTYPE html>
<html>
<body>
  <h1>Data Submitted Successfully!</h1>
  <p>Username: {username}</p>
  <p>Message: {message}</p>
</body>
</html>
"""

# Set HTTP headers (optional)
print("Content-Type: text/html")
print("")  # Separate headers from body

# Print response content
print(response_body)
