# Copilot Instructions

## General Guidelines
- First general instruction
- Second general instruction
- Maintain a local TODO markdown file for tasks (e.g., `docs/TODO_Boss_ImGui_And_JSON.md`). This file should include prioritized tasks, affected files, test steps, and instructions for another AI to implement JSON save/load and ImGui controls for Boss states.

## Code Style
- Use specific formatting rules
- Follow naming conventions

## Localization
- Prefer using the `IMGUI_JP` macro for ImGui localization: `IMGUI_JP("...")`

## Project-Specific Rules
- Implement JSON save/load support for Boss attack states similar to Player AttackCombo states.
- Add `LoadSettings` and `SaveSettings` methods in `BossAttackStateBase` to facilitate JSON save/load for boss attack states.
- Override `GetSettingsFileName` for each state to specify the settings file name.
