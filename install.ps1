# install.ps1 - installs the bm bookmarks CLI
# Usage:  irm https://raw.githubusercontent.com/AidenSperr/bookmarker-cli/master/install.ps1 | iex

$ErrorActionPreference = "Stop"

$repo = "AidenSperr/bookmarker-cli"
$installDir = Join-Path $HOME ".bookmarker-cli"

Write-Host "Installing bookmarker-cli to $installDir ..." -ForegroundColor Cyan

if (-not (Test-Path $installDir)) {
    New-Item -ItemType Directory -Path $installDir | Out-Null
}

$release = Invoke-RestMethod -Uri "https://api.github.com/repos/$repo/releases/latest"
$asset = $release.assets | Where-Object { $_.name -eq "bm.exe" }

if (-not $asset) {
    Write-Error "Could not find bm.exe attached to the latest release of $repo."
    exit 1
}

Invoke-WebRequest -Uri $asset.browser_download_url -OutFile (Join-Path $installDir "bm.exe")

$wrapperContent = @'
function bm {
    param(
        [Parameter(Position = 0)]
        [string]$Name
    )
    $bmExe = Join-Path $PSScriptRoot "bm.exe"
    if ($Name) {
        $path = & $bmExe get $Name
        if ($LASTEXITCODE -eq 0) {
            Set-Location -Path $path
        }
    }
    else {
        & $bmExe list
        $choice = Read-Host "Jump to"
        if ($choice) { bm $choice }
    }
}

function bma {
    param(
        [Parameter(Mandatory, Position = 0)]
        [string]$Name,

        [Parameter(Position = 1)]
        [string]$Path
    )
    $bmExe = Join-Path $PSScriptRoot "bm.exe"
    if ($Path) {
        & $bmExe add $Name $Path
    }
    else {
        & $bmExe add $Name
    }
}

function bmrm {
    param(
        [Parameter(Mandatory, Position = 0)]
        [string]$Name
    )
    $bmExe = Join-Path $PSScriptRoot "bm.exe"
    & $bmExe rm $Name
}

function bml {
    $bmExe = Join-Path $PSScriptRoot "bm.exe"
    & $bmExe list
}
'@

Set-Content -Path (Join-Path $installDir "Bookmarks.ps1") -Value $wrapperContent -Encoding UTF8

$dotSourceLine = ". `"$installDir\Bookmarks.ps1`""

if (-not (Test-Path $PROFILE)) {
    New-Item -ItemType File -Path $PROFILE -Force | Out-Null
}

$profileContent = Get-Content $PROFILE -Raw -ErrorAction SilentlyContinue
if (-not $profileContent -or $profileContent -notmatch [regex]::Escape($dotSourceLine)) {
    Add-Content -Path $PROFILE -Value "`n$dotSourceLine"
    Write-Host "Added bookmarker-cli to your PowerShell profile." -ForegroundColor Green
}
else {
    Write-Host "Already present in your PowerShell profile." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Done. Restart your terminal, then try: bma myprojectpath" -ForegroundColor Cyan