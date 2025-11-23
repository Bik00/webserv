#!/usr/bin/env python3
import os

# CGI Status header test (RFC 3875 Section 6.3.3)
print("Status: 201 Created")
print("Content-Type: application/json")
print("X-Custom-Response: webserv-cgi")
print()
print('{"status": "created", "message": "CGI Status header works!"}')
