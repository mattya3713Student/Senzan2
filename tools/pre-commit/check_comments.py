#!/usr/bin/env python3
import sys
import re
import subprocess

# Read staged files
try:
    result = subprocess.run(["git","diff","--cached","--name-only"], capture_output=True, text=True, check=True)
    files = [f.strip() for f in result.stdout.splitlines() if f.strip()]
except Exception as e:
    print("Failed to get staged files:", e)
    sys.exit(1)

# Patterns to check: file header must contain @author and @brief
header_pattern = re.compile(r"/\*{5,}[^@]*@author\s*:\s*.+?@date\s*:\s*\d{4}/\d{2}/\d{2}.[\s\S]*?@brief\s*:\s*.+?\*/", re.MULTILINE)

failures = []
for f in files:
    if not (f.endswith('.cpp') or f.endswith('.h')):
        continue
    try:
        with open(f, 'r', encoding='utf-8') as fh:
            content = fh.read(1024)  # only need start of file
            if not header_pattern.search(content):
                failures.append(f)
    except Exception as e:
        print(f"Failed to open {f}: {e}")
        failures.append(f)

if failures:
    print("Files missing required file header (author/date/brief):")
    for f in failures:
        print(" - ", f)
    print("\nPlease add the standardized file header as defined in docs/CodingStyle_Comments.md")
    sys.exit(1)

print("Comment style check passed.")
sys.exit(0)
