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
		if ($choice) {
			bm $choice
		}
	}
}

function bml {
    $bmExe = Join-Path $PSScriptRoot "bm.exe"

    & $bmExe list
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