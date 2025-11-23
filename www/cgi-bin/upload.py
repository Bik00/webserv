#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb
import json
from datetime import datetime

# Enable CGI error reporting
cgitb.enable()

# Configuration
UPLOAD_DIR = "www/uploads"
MAX_FILE_SIZE = 10 * 1024 * 1024  # 10MB
ALLOWED_EXTENSIONS = {'.jpg', '.jpeg', '.png', '.gif', '.bmp', '.webp'}

def send_response(status, message, success=False):
    """Send JSON response"""
    print("Content-Type: application/json")
    if not success:
        print(f"Status: {status}")
    print()
    response = {
        "success": success,
        "message": message
    }
    print(json.dumps(response))
    sys.exit(0)

def get_file_extension(filename):
    """Get file extension in lowercase"""
    return os.path.splitext(filename)[1].lower()

def sanitize_filename(filename):
    """Sanitize filename to prevent path traversal"""
    # Remove path components
    filename = os.path.basename(filename)
    
    # Replace spaces and special characters
    safe_chars = []
    for char in filename:
        if char.isalnum() or char in '._-':
            safe_chars.append(char)
        elif char == ' ':
            safe_chars.append('_')
    
    filename = ''.join(safe_chars)
    
    # Add timestamp to make filename unique
    name, ext = os.path.splitext(filename)
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    return f"{name}_{timestamp}{ext}"

def main():
    # Check request method
    request_method = os.environ.get('REQUEST_METHOD', '')
    if request_method != 'POST':
        send_response(405, "Method Not Allowed", False)
    
    # Get content type
    content_type = os.environ.get('CONTENT_TYPE', '')
    if not content_type.startswith('multipart/form-data'):
        send_response(400, "Invalid content type. Expected multipart/form-data", False)
    
    # Parse multipart form data
    try:
        form = cgi.FieldStorage()
    except Exception as e:
        send_response(400, f"Failed to parse form data: {str(e)}", False)
    
    # Get uploaded file
    if 'file' not in form:
        send_response(400, "No file uploaded", False)
    
    fileitem = form['file']
    
    # Check if file was actually uploaded
    if not fileitem.filename:
        send_response(400, "No file selected", False)
    
    # Validate file extension
    ext = get_file_extension(fileitem.filename)
    if ext not in ALLOWED_EXTENSIONS:
        send_response(400, f"File type not allowed. Allowed: {', '.join(ALLOWED_EXTENSIONS)}", False)
    
    # Sanitize filename
    safe_filename = sanitize_filename(fileitem.filename)
    
    # Create upload directory if it doesn't exist
    os.makedirs(UPLOAD_DIR, exist_ok=True)
    
    # Full path for the uploaded file
    filepath = os.path.join(UPLOAD_DIR, safe_filename)
    
    # Save file
    try:
        with open(filepath, 'wb') as f:
            # Read file data
            file_data = fileitem.file.read()
            
            # Check file size
            if len(file_data) > MAX_FILE_SIZE:
                send_response(400, f"File too large. Maximum size: {MAX_FILE_SIZE // (1024*1024)}MB", False)
            
            # Write file
            f.write(file_data)
        
        # Success response
        file_size = len(file_data)
        send_response(200, f"File uploaded successfully: {safe_filename} ({file_size} bytes)", True)
        
    except Exception as e:
        send_response(500, f"Failed to save file: {str(e)}", False)

if __name__ == "__main__":
    main()
