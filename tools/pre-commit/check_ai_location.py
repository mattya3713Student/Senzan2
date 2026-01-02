#!/usr/bin/env python3
import sys
import subprocess

# Ensure any AI-generated files are under docs/ai-generated/
try:
    result = subprocess.run(["git","diff","--cached","--name-only"], capture_output=True, text=True, check=True)
    files = [f.strip() for f in result.stdout.splitlines() if f.strip()]
except Exception as e:
    print("Failed to get staged files:", e)
    sys.exit(1)

violations = []
for f in files:
    # allow if under docs/ai-generated
    if f.startswith('docs/ai-generated') or f.startswith('docs\\ai-generated'):
        continue
    # if file name or path looks AI-ish
    lf = f.lower()
    if 'copilot' in lf or '.copilot' in lf or 'ai.log' in lf or 'ai-debug.log' in lf:
        violations.append(f)

if violations:
    print('AI-generated files must live under docs/ai-generated/ or be explicitly allowed:')
    for v in violations:
        print(' -', v)
    sys.exit(1)

sys.exit(0)
