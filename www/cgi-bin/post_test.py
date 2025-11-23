#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print()
print("<html><head><title>POST Test</title></head><body>")
print("<h1>POST Request Test</h1>")
print("<h2>Request Method:</h2>")
print(f"<p>{os.environ.get('REQUEST_METHOD', 'N/A')}</p>")
print("<h2>Content-Type:</h2>")
print(f"<p>{os.environ.get('CONTENT_TYPE', 'N/A')}</p>")
print("<h2>Content-Length:</h2>")
print(f"<p>{os.environ.get('CONTENT_LENGTH', 'N/A')}</p>")
print("<h2>POST Body (from stdin):</h2>")

# Read POST body from stdin
content_length = os.environ.get('CONTENT_LENGTH', '0')
if content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))
    print(f"<pre>{body}</pre>")
else:
    print("<p>No body received</p>")

print("</body></html>")
