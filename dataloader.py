#!/usr/bin/env python3
import json
import subprocess
import sys
import shutil
import os
import re
import platform

def get_clipboard():
    system = platform.system()
    
    if system == 'Linux':
        try:
            result = subprocess.run(['xclip', '-selection', 'clipboard', '-o'], capture_output=True, text=True, timeout=10)
            return result.stdout
        except FileNotFoundError:
            print("Error: xclip not installed (Linux)")
            sys.exit(1)
    
    elif system == 'Windows':
        try:
            result = subprocess.run(['powershell', '-command', 'Get-Clipboard'], capture_output=True, text=True, timeout=10)
            return result.stdout
        except FileNotFoundError:
            print("Error: PowerShell not available (Windows)")
            sys.exit(1)
    
    elif system == 'Darwin':
        try:
            result = subprocess.run(['pbpaste'], capture_output=True, text=True, timeout=10)
            return result.stdout
        except FileNotFoundError:
            print("Error: pbpaste not available (macOS)")
            sys.exit(1)
    
    else:
        print(f"Error: Unsupported platform: {system}")
        sys.exit(1)

if len(sys.argv) != 2:
    print("Usage: dataloader.py <path-to-html-file>")
    sys.exit(1)

html_file = sys.argv[1]

if not os.path.exists(html_file):
    print(f"Error: File '{html_file}' not found")
    sys.exit(1)

clip_content = get_clipboard().strip()

if not clip_content:
    print("Error: Clipboard is empty")
    sys.exit(1)

if not clip_content.startswith('let database'):
    print("Error: Clipboard content does not start with 'let database'")
    print(f"Content starts with: {clip_content[:50]}...")
    sys.exit(1)

if not clip_content.endswith('];'):
    print("Error: Clipboard content does not end with '];'")
    print(f"Content ends with: ...{clip_content[-50:]}")
    sys.exit(1)

array_match = re.search(r'let database = \[(.+)\];', clip_content, re.DOTALL)
if not array_match:
    print("Error: Could not parse database array from clipboard")
    sys.exit(1)

array_str = '[' + array_match.group(1) + ']'

try:
    test_parse = json.loads(array_str)
    print(f"✓ Clipboard contains valid database ({len(test_parse)} elements)")
except json.JSONDecodeError as e:
    print(f"Error: Clipboard content is not valid JSON: {e}")
    sys.exit(1)

shutil.copy(html_file, html_file + '.bak')

with open(html_file, 'r') as f:
    lines = f.readlines()

new_lines = []
in_database = False

for i, line in enumerate(lines):
    if 'let database = [' in line and not in_database:
        in_database = True
        new_lines.append(clip_content + '\n')
    elif in_database:
        if line.strip() == '];':
            in_database = False
            continue
    else:
        new_lines.append(line)

with open(html_file, 'w') as f:
    f.writelines(new_lines)

print(f"✓ Database replaced in {html_file}")