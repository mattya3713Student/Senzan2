# PowerShell script to find tracked AI-related files and git rm --cached them
$patterns = @('copilot','\.copilot','tools/ai/generated','ai.log','ai-debug.log','*.copilot.*')

$files = git ls-files
$toUntrack = @()
foreach ($p in $patterns) {
    $matches = $files | Where-Object { $_ -match $p }
    if ($matches) { $toUntrack += $matches }
}

$toUntrack = $toUntrack | Select-Object -Unique
if ($toUntrack.Count -eq 0) {
    Write-Host "No tracked AI-related files found."
    exit 0
}

Write-Host "Files to untrack:"
$toUntrack | ForEach-Object { Write-Host " - $_" }

foreach ($f in $toUntrack) {
    git rm --cached -- "$f"
}

Write-Host "Untracked listed files from index. Please review 'git status' and commit the changes."
