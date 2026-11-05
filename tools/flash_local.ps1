# Local SINGLE-mode flashing: one merged image at offset 0, like ai-passport.
# SPDX-License-Identifier: MIT
param(
    [string]$Port='COM6',
    [int]$Baud=460800,
    [string]$EspPython='D:\1.Soft\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe',
    [switch]$DryRun
)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path $PSScriptRoot -Parent
$full=Join-Path $projectRoot 'build\sdgoods-bambu-monitor-full.bin'
foreach($file in @($EspPython,$full)) {
    if(-not (Test-Path -LiteralPath $file -PathType Leaf)){throw "Missing file: $file"}
}
$flashArgs=@('-m','esptool','--chip','esp32s3','--port',$Port,'--baud',"$Baud",
    '--before','default_reset','--after','hard_reset','write_flash',
    '0x0',$full)
Write-Host 'Factory application will be replaced with BambuMonitor (SINGLE mode).'
Write-Host 'Merged image at 0x0 includes bootloader, partition table, app and initial OTA data.'
Write-Host 'The merged range includes NVS: saved network/system settings will be reset.'
if($DryRun) {
    Write-Host ($EspPython+' '+(($flashArgs | ForEach-Object {'"'+$_+'"'}) -join ' '))
    return
}
& $EspPython @flashArgs
if($LASTEXITCODE -ne 0){throw "esptool failed: $LASTEXITCODE"}
