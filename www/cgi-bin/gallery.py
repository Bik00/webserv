#!/usr/bin/env python3
import os
import json
import sys
from datetime import datetime

# Configuration
UPLOAD_DIR = "www/uploads"
ALLOWED_EXTENSIONS = {'.jpg', '.jpeg', '.png', '.gif', '.bmp', '.webp'}

def get_file_info(filename, filepath):
    """Get file information"""
    try:
        stat = os.stat(filepath)
        return {
            "name": filename,
            "url": f"/uploads/{filename}",
            "size": stat.st_size,
            "modified": datetime.fromtimestamp(stat.st_mtime).strftime('%Y-%m-%d %H:%M:%S')
        }
    except Exception as e:
        return None

def main():
    # Send headers
    print("Content-Type: application/json")
    print()
    
    # Check if upload directory exists
    if not os.path.exists(UPLOAD_DIR):
        response = {
            "success": True,
            "images": [],
            "count": 0
        }
        print(json.dumps(response))
        return
    
    # Get list of image files
    images = []
    
    try:
        for filename in os.listdir(UPLOAD_DIR):
            filepath = os.path.join(UPLOAD_DIR, filename)
            
            # Skip if not a file
            if not os.path.isfile(filepath):
                continue
            
            # Check file extension
            ext = os.path.splitext(filename)[1].lower()
            if ext not in ALLOWED_EXTENSIONS:
                continue
            
            # Get file info
            file_info = get_file_info(filename, filepath)
            if file_info:
                images.append(file_info)
        
        # Sort by modification time (newest first)
        images.sort(key=lambda x: x['modified'], reverse=True)
        
        # Send response
        response = {
            "success": True,
            "images": images,
            "count": len(images)
        }
        print(json.dumps(response))
        
    except Exception as e:
        response = {
            "success": False,
            "error": str(e),
            "images": [],
            "count": 0
        }
        print(json.dumps(response))

if __name__ == "__main__":
    main()
