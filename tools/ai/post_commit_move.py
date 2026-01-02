#!/usr/bin/env python3
import subprocess
import sys
import os
from pathlib import Path

# Patterns that indicate AI/Copilot-generated files
PATTERNS = ['copilot', '.copilot', 'copilot-vs', 'ai.log', 'ai-debug.log']

REPO_ROOT = Path.cwd()
DEST_ROOT = REPO_ROOT / 'docs' / 'ai-generated'

def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)

def main():
    # Get files in last commit
    r = run(['git', 'diff-tree', '--no-commit-id', '--name-only', '-r', 'HEAD'])
    if r.returncode != 0:
        print('Failed to get last commit files:', r.stderr)
        return 1
    files = [f for f in r.stdout.splitlines() if f]
    to_move = []
    for f in files:
        lf = f.lower()
        if any(p in lf for p in PATTERNS):
            # skip if already under docs
            if os.path.commonpath([str(REPO_ROOT / f), str(REPO_ROOT / 'docs')]) == str(REPO_ROOT / 'docs'):
                continue
            to_move.append(f)
    if not to_move:
        # nothing to do
        return 0

    print('AI-related files found in last commit:')
    for f in to_move:
        print(' -', f)

    # Ensure dest root exists
    DEST_ROOT.mkdir(parents=True, exist_ok=True)

    moved_any = False
    for f in to_move:
        src = Path(f)
        if not src.exists():
            print(f'Warning: {f} not found on filesystem, skipping')
            continue
        dest = DEST_ROOT / src.name
        # avoid name collision
        i = 1
        base = dest.stem
        suff = dest.suffix
        while dest.exists():
            dest = DEST_ROOT / f"{base}-{i}{suff}"
            i += 1
        dest.parent.mkdir(parents=True, exist_ok=True)
        # use git mv if possible
        r = run(['git', 'mv', '-f', str(src), str(dest)])
        if r.returncode != 0:
            # fallback to filesystem move
            try:
                src.replace(dest)
            except Exception as e:
                print(f'Failed to move {src} to {dest}: {e}')
                continue
        print(f'Moved {src} -> {dest}')
        moved_any = True

    if moved_any:
        # stage and amend commit
        r = run(['git', 'add', '-A'])
        if r.returncode != 0:
            print('git add failed:', r.stderr)
            return 1
        r = run(['git', 'commit', '--amend', '--no-edit'])
        if r.returncode != 0:
            print('git commit --amend failed:', r.stderr)
            return 1
        print('Amended last commit to relocate AI-generated files under docs/ai-generated/')
    return 0

if __name__ == '__main__':
    sys.exit(main())
