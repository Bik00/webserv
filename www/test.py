#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print()
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI Script Works!</h1>")
print("<h2>Environment Variables:</h2>")
print("<ul>")
for key in sorted(os.environ.keys()):
    if key.startswith('HTTP_') or key in ['REQUEST_METHOD', 'QUERY_STRING', 'SCRIPT_NAME', 'PATH_INFO']:
        print(f"<li><b>{key}</b>: {os.environ[key]}</li>")
print("</ul>")
print("</body>")
print("</html>")
