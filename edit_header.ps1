# A Windows powershell script to edit the rom header.
#
# The edit is only to assist EverDrive users.
# It sets 3 bytes in the rom header to tell the
# EverDrive to load the game with eeprom4k enabled.
# Alternatively you could just edit the save_db.txt

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