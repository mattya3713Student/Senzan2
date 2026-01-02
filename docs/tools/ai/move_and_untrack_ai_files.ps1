# Move tracked AI-related files into docs/tools/ai/generated/ and untrack them
# Usage: powershell -ExecutionPolicy Bypass -File docs/tools/ai/move_and_untrack_ai_files.ps1

$root = (git rev-parse --show-toplevel) 2>$null
if (-not $?) { Write-Error "Not a git repository or git not found."; exit 1 }
$root = $root.Trim()
Set-Location $root

$patterns = @('copilot', '\.copilot', 'ai\.log', 'ai-debug\.log')

$files = git ls-files
if (-not $files) { Write-Host "No tracked files."; exit 0 }

$toProcess = @()
foreach ($f in $files) {
    # skip files already under docs/tools/ai/generated
    if ($f -like 'docs/tools/ai/generated/*') { continue }
    foreach ($p in $patterns) {
        try {
            if ($f -match $p) { $toProcess += $f; break }
        } catch {
            # ignore regex errors
        }
    }
}

$toProcess = $toProcess | Select-Object -Unique
if ($toProcess.Count -eq 0) { Write-Host "No tracked AI-related files found to move."; exit 0 }

# Ensure destination root exists
$destRoot = Join-Path $root 'docs/tools/ai/generated'
New-Item -ItemType Directory -Force -Path $destRoot | Out-Null

Write-Host "Found $($toProcess.Count) files to move and untrack."
foreach ($f in $toProcess) {
    $src = $f
    $dest = Join-Path 'docs/tools/ai/generated' $f
    $destDir = [System.IO.Path]::GetDirectoryName($dest)
    if (-not (Test-Path $destDir)) { New-Item -ItemType Directory -Force -Path $destDir | Out-Null }

    Write-Host "Moving: $src -> $dest"
    # Use git mv to preserve history in index
    git mv -f -- "$src" "$dest"
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "git mv failed for $src -> $dest; attempting filesystem move."
        try { Move-Item -Force -Path $src -Destination $dest } catch { Write-Error ("Failed to move {0}: {1}" -f $src, $_); continue }
        # If we moved by filesystem, remove from index
        git rm --cached -- "$src" 2>$null
    }

    # Now untrack the file at new location
    Write-Host "Untracking: $dest"
    git rm --cached -- "$dest"
}

Write-Host "Done. Review 'git status' and commit the changes if acceptable."
