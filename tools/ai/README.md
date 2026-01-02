# tools/ai

This directory is the designated location for AI / assistant generated files.

Guidelines:
- Any automated tools or AI assistants must write generated outputs under `tools/ai/generated/`.
- Do not commit generated outputs unless they are required for the build and cannot be reproduced.
- If you need a generated file to be tracked, add an explicit negate rule to `.gitignore` and commit only the specific file.

To stop tracking an already tracked generated file:

  git rm --cached <path/to/file>

