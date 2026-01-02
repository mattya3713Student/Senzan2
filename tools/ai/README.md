# tools/ai

This directory is legacy. Per project policy, GitHub Copilot or other AI tools should place generated files under `docs/ai-generated/`.

Guidelines:
- AI-generated outputs should be written under `docs/ai-generated/`.
- Do not commit generated outputs unless they are required for the build and cannot be reproduced.
- If you need a generated file to be tracked, add an explicit negate rule to `.gitignore` and commit only the specific file.

To stop tracking an already tracked generated file:

  git rm --cached <path/to/file>

