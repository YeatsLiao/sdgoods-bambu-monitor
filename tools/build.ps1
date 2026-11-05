param([switch]$Merge)
$ErrorActionPreference='Stop'
$env:PROCESSOR_ARCHITECTURE='AMD64'
if (-not $env:IDF_PATH) { $env:IDF_PATH='D:\1.Soft\Espressif\frameworks\esp-idf-v5.5.5' }
if (-not $env:IDF_TOOLS_PATH) { $env:IDF_TOOLS_PATH='D:\1.Soft\Espressif' }
if (-not $env:IDF_PYTHON_ENV_PATH) { $env:IDF_PYTHON_ENV_PATH=Join-Path $env:IDF_TOOLS_PATH 'python_env\idf5.5_py3.11_env' }
$toolBins=@("$env:IDF_PYTHON_ENV_PATH\Scripts", "$env:IDF_PATH\tools")
foreach($toolName in @('xtensa-esp-elf','cmake','ninja')) {
    $toolRoot=Join-Path $env:IDF_TOOLS_PATH "tools\$toolName"
    $toolVersion=Get-ChildItem -LiteralPath $toolRoot -Directory | Sort-Object Name -Descending | Select-Object -First 1
    if($toolName -eq 'xtensa-esp-elf') {$toolBins+=Join-Path $toolVersion.FullName 'xtensa-esp-elf\bin'}
    elseif($toolName -eq 'cmake') {$toolBins+=Join-Path $toolVersion.FullName 'bin'}
    else {$toolBins+=$toolVersion.FullName}
}
$env:PATH=($toolBins -join ';')+';'+$env:PATH
$romRoot=Join-Path $env:IDF_TOOLS_PATH 'tools\esp-rom-elfs'
if(Test-Path -LiteralPath $romRoot) {
    $romSet=Get-ChildItem -LiteralPath $romRoot -Directory | Sort-Object Name -Descending | Select-Object -First 1
    $env:ESP_ROM_ELF_DIR=$romSet.FullName
}
$projectRoot=Split-Path $PSScriptRoot -Parent
Set-Location $projectRoot
$actions=if($Merge){@('build','merge-bin')}else{@('build')}
& "$env:IDF_PYTHON_ENV_PATH\Scripts\python.exe" "$env:IDF_PATH\tools\idf.py" @actions
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
if($Merge) {
    Copy-Item -LiteralPath "$projectRoot\build\merged-binary.bin" -Destination "$projectRoot\build\sdgoods-bambu-monitor-full.bin"
    & "$env:IDF_PYTHON_ENV_PATH\Scripts\python.exe" "$projectRoot\tools\check_full_image.py" --build-dir build --image build/sdgoods-bambu-monitor-full.bin
    exit $LASTEXITCODE
}
exit 0
