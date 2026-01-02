#!/usr/bin/env python3
import sys
import subprocess

# Ensure any AI-generated files are under tools/ai/generated/
try:
    result = subprocess.run(["git","diff","--cached","--name-only"], capture_output=True, text=True, check=True)
    files = [f.strip() for f in result.stdout.splitlines() if f.strip()]
except Exception as e:
    print("Failed to get staged files:", e)
    sys.exit(1)

violations = []
for f in files:
    if any(part.startswith('tools/ai') for part in f.split('/')):
        continue
    # if file name or path looks AI-ish
    if 'copilot' in f or '.copilot' in f or 'ai.log' in f or 'ai-debug.log' in f:
        violations.append(f)

if violations:
    print('AI-generated files must live under tools/ai/generated/ or be explicitly allowed:')
    for v in violations:
        print(' -', v)
    sys.exit(1)

sys.exit(0)
