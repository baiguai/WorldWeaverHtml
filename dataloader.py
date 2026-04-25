#!/usr/bin/env python3
import json
import subprocess
import sys
import shutil
import os
import re

if len(sys.argv) != 2:
    print("Usage: replace_database.py <path-to-html-file>")
    sys.exit(1)

html_file = sys.argv[1]

if not os.path.exists(html_file):
    print(f"Error: File '{html_file}' not found")
    sys.exit(1)

try:
    result = subprocess.run(['xclip', '-selection', 'clipboard', '-o'], capture_output=True, text=True, timeout=10)
    clip_content = result.stdout
except FileNotFoundError:
    print("Error: xclip is not installed")
    sys.exit(1)
except subprocess.TimeoutExpired:
    print("Error: xclip timed out")
    sys.exit(1)

clip_content = clip_content.strip()

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