$filepath="$PSScriptRoot\build\juicin64.z64"
$tempFilePath="$PSScriptRoot\build\temp_juicin64.z64"
$bytes  = [System.IO.File]::ReadAllBytes($filepath)
$offset = 0x3C

$bytes[$offset]   = 0x45
$bytes[$offset+1] = 0x44
$bytes[$offset+3] = 0x10

[System.IO.File]::WriteAllBytes($tempFilePath, $bytes)

Remove-Item -Path $filePath
Move-Item -Path $tempFilePath -Destination $filePath