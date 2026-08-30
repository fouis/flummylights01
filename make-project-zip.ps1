# make-project-zip.ps1

$ErrorActionPreference = "Stop"

Set-Location $PSScriptRoot

$ProjectName = Split-Path $PSScriptRoot -Leaf
$Timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$ZipName = "${ProjectName}_${Timestamp}.zip"

# Remove previous exports
Get-ChildItem "${ProjectName}_*.zip" -File -ErrorAction SilentlyContinue |
    Remove-Item -Force

# Warn about changes which are not committed yet
if (git status --porcelain) {
    Write-Warning "There are uncommitted changes!"
    Write-Warning "They will NOT be included in the ZIP."
    Write-Host ""
}

git archive `
    --format=zip `
    --output="$ZipName" `
    HEAD

if ($LASTEXITCODE -ne 0) {
    throw "git archive failed."
}

Write-Host ""
Write-Host "Created: $ZipName"