git fetch --all --tags

$VERSION_TAG = git describe --always --tags --abbrev=0
$COMMIT_HASH = git rev-parse --short HEAD
$OS_INFO = (Get-WMIObject win32_operatingsystem).caption + " " + `
           (Get-WMIObject win32_operatingsystem).version + " " + `
           (Get-WMIObject win32_operatingsystem).OSArchitecture

$SDL2_DIR = "SDL2-2.30.3\x86_64-w64-mingw32"
$RV32_DIR = "rv32"
$MACHINE_DIR = "machine"
$ELF_DIR = "elf"

if (!(Test-Path -Path "build")) {
    mkdir -Force -Path build > $null

    Write-Host -NoNewline "Building RV32 core... "
    Set-Location build

    gcc -c -I"..\$($RV32_DIR)" "..\$($RV32_DIR)\*.c"

    Write-Output "done"

    Write-Host -NoNewline "Building machine... "

    gcc -c -I"..\$($MACHINE_DIR)" "..\$($MACHINE_DIR)\*.c"

    Write-Output "done"

    Write-Host -NoNewline "Building ELF loader... "

    gcc -c -I"..\$($ELF_DIR)" "..\$($ELF_DIR)\*.c"

    Write-Output "done"

    Set-Location ..
}

mkdir -Force -Path bin > $null

c++ -I"$($RV32_DIR)" `
    -I"$($MACHINE_DIR)" `
    -I"$($ELF_DIR)" `
    "build\*.o" `
    "tools\testbench.cpp" `
    -o "bin\testbench.exe" `
    -DREP_VERSION="`"$($VERSION_TAG)`"" `
    -DREP_COMMIT_HASH="`"$($COMMIT_HASH)`"" `
    -DOS_INFO="`"$($OS_INFO)`"" `
    -L"$($SDL2_DIR)\lib" `
    -m64 -lSDL2main -lSDL2 `
    -Wall -pedantic `
    -ffast-math -Ofast -flto

Copy-Item -Path "$($SDL2_DIR)\bin\SDL2.dll" -Destination "bin"