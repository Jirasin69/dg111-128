# Setup-VSCode.ps1 - Automated VS Code Setup for C Programming & Raylib 5.0
# Description: Custom VS Code workspace setting generator for MSYS2 MinGW-w64/GCC.

# --- Styling & Colors ---
$Esc = [char]27
$Style = @{
    Reset      = "$Esc[0m"
    Bold       = "$Esc[1m"
    Underline  = "$Esc[4m"
    Cyan       = "$Esc[36m"
    Green      = "$Esc[32m"
    Yellow     = "$Esc[33m"
    Magenta    = "$Esc[35m"
    Red        = "$Esc[31m"
    Gray       = "$Esc[90m"
    White      = "$Esc[97m"
    BgBlue     = "$Esc[44m"
}

function Show-Banner {
    Clear-Host
    Write-Host "$($Style.Bold)$($Style.Cyan)"
    Write-Host "  ██████╗ ██████╗ ██████╗  ██████╗  ██████╗  ██████╗ ███████╗"
    Write-Host " ██╔════╝ ██╔══██╗██╔══██╗██╔═══██╗██╔════╝ ██╔═══██╗██╔════╝"
    Write-Host " ██║      ██████╔╝██████╔╝██║   ██║██║      ██║   ██║███████╗"
    Write-Host " ██║      ██╔═══╝ ██╔═══╝ ██║   ██║██║      ██║   ██║╚════██║"
    Write-Host " ╚██████╗ ██║      ██║     ╚██████╔╝╚██████╗╚██████╔╝███████║"
    Write-Host "  ╚═════╝ ╚═╝      ╚═╝      ╚═════╝  ╚═════╝  ╚═════╝ ╚══════╝"
    Write-Host "                                                    "
    Write-Host "     VS CODE SETUP FOR C PROGRAMMING & RAYLIB 5.0   "
    Write-Host "             DG111 COMPUTER PROGRAMMING             "
    Write-Host "$($Style.Reset)"
}

function Write-Info { param([string]$msg) Write-Host "$($Style.Cyan)[i] $msg$($Style.Reset)" }
function Write-Success { param([string]$msg) Write-Host "$($Style.Green)[√] $msg$($Style.Reset)" }
function Write-Warning { param([string]$msg) Write-Host "$($Style.Yellow)[!] $msg$($Style.Reset)" }
function Write-ErrorMsg { param([string]$msg) Write-Host "$($Style.Red)[X] $msg$($Style.Reset)" }
function Write-Section { param([string]$title) 
    Write-Host "`n$($Style.Bold)$($Style.Underline)$($Style.White)$title$($Style.Reset)" 
}

# --- ค้นหา Root ของโปรเจกต์ ---
function Find-ProjectRoot {
    param([string]$StartPath)
    $dir = Get-Item -LiteralPath $StartPath
    while ($null -ne $dir) {
        if ((Test-Path (Join-Path $dir.FullName ".git")) -or (Test-Path (Join-Path $dir.FullName "CLAUDE.md"))) {
            return $dir.FullName
        }
        $dir = $dir.Parent
    }
    return $null
}

# --- Main Logic ---
Show-Banner

# สคริปต์นี้อยู่ใน scripts/ เสมอ — ใช้ $PSScriptRoot เพื่อหา root ของโปรเจกต์
# แทนที่จะยึดตาม Get-Location ซึ่งเปลี่ยนไปตามตำแหน่งที่ผู้ใช้รันคำสั่ง
$projectRoot = Find-ProjectRoot -StartPath $PSScriptRoot
if (-not $projectRoot) {
    $projectRoot = Split-Path $PSScriptRoot -Parent
    Write-Warning "ไม่พบ .git หรือ CLAUDE.md ระหว่างค้นหา root — ใช้โฟลเดอร์แม่ของ scripts/ แทน: $projectRoot"
}
$workspaceDir = $projectRoot
Write-Info "โฟลเดอร์โปรเจกต์: $workspaceDir"

# --- ยืนยันก่อนเริ่มตั้งค่า ---
Write-Host ""
Write-Warning "สคริปต์นี้จะสร้าง/เขียนทับไฟล์การตั้งค่าในโฟลเดอร์ .vscode ของโปรเจกต์นี้"
$confirm = Read-Host "$($Style.Bold)ต้องการเริ่มตั้งค่า VS Code หรือไม่? (Y/N)$($Style.Reset)"
if ($confirm -notmatch '^(y|yes)$') {
    Write-Info "ยกเลิกการตั้งค่าแล้ว ไม่มีการเปลี่ยนแปลงใดๆ"
    exit
}

# 1. ค้นหา GCC Compiler และระบุประเภทของเครื่องมือ
$compilerPath = ""
$compilerDir = ""
$compilerType = "" # msys2-mingw64, msys2-ucrt64, custom-mingw64, path-gcc

$pathsToCheck = @(
    @{ Path = "C:\msys64\mingw64\bin\gcc.exe"; Type = "msys2-mingw64" },
    @{ Path = "C:\msys64\ucrt64\bin\gcc.exe"; Type = "msys2-ucrt64" },
    @{ Path = "C:\mingw64\bin\gcc.exe"; Type = "custom-mingw64" }
)

foreach ($item in $pathsToCheck) {
    if (Test-Path $item.Path) {
        $compilerPath = $item.Path
        $compilerDir = [System.IO.Path]::GetDirectoryName($item.Path)
        $compilerType = $item.Type
        break
    }
}

if ($compilerPath -eq "") {
    # ค้นหาใน PATH environment
    $gccCommand = Get-Command gcc.exe -ErrorAction SilentlyContinue
    if ($gccCommand) {
        $compilerPath = $gccCommand.Source
        $compilerDir = [System.IO.Path]::GetDirectoryName($compilerPath)
        $compilerType = "path-gcc"
    }
}

if ($compilerPath -eq "") {
    Write-ErrorMsg "ไม่พบ Compiler (GCC) ในเครื่องของคุณ!"
    Write-Host ""
    Write-Host "$($Style.Bold)💡 วิธีแก้ไขสำหรับ Windows:$($Style.Reset)"
    Write-Host "  1. ติดตั้ง MSYS2 ตามคู่มือ SETUP.md หรือใช้ winget:"
    Write-Host "     $($Style.Yellow)winget install MSYS2.MSYS2$($Style.Reset)"
    Write-Host "  2. เปิด MSYS2 MINGW64 terminal แล้วรันคำสั่งเพื่อติดตั้ง GCC & GDB:"
    Write-Host "     $($Style.Yellow)pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb$($Style.Reset)"
    Write-Host ""
    Write-Host "กดปุ่มใดก็ได้เพื่อจบการทำงาน..."
    [void][System.Console]::ReadKey($true)
    exit
}

Write-Host "$($Style.Green)[√] ตรวจพบ Compiler: $compilerPath ($compilerType)$($Style.Reset)"

# วิเคราะห์หา Path ของ Raylib 5.0
$baseDir = [System.IO.Path]::GetDirectoryName($compilerDir) # e.g. C:\msys64\mingw64
$raylibInc = "$baseDir\include".Replace("\", "/")
$raylibLib = "$baseDir\lib".Replace("\", "/")
$raylibHeader = "$baseDir\include\raylib.h"

$raylibFound = Test-Path $raylibHeader
if ($raylibFound) {
    Write-Host "$($Style.Green)[√] ตรวจพบ Library: Raylib พร้อมใช้งาน! 🎉$($Style.Reset)"
} else {
    Write-Warning "ไม่พบ Library Raylib ในตำแหน่งมาตรฐาน! ($raylibHeader)"

    $raylibPackage = if ($compilerType -eq "msys2-ucrt64") { "mingw-w64-ucrt-x86_64-raylib" } else { "mingw-w64-x86_64-raylib" }
    $pacmanPath = "C:\msys64\usr\bin\pacman.exe"

    if (($compilerType -eq "msys2-mingw64" -or $compilerType -eq "msys2-ucrt64") -and (Test-Path $pacmanPath)) {
        Write-Info "กำลังติดตั้ง Raylib อัตโนมัติผ่าน pacman ($raylibPackage) — อาจใช้เวลาสักครู่..."
        & $pacmanPath -S --noconfirm --needed $raylibPackage

        if (Test-Path $raylibHeader) {
            $raylibFound = $true
            Write-Success "ติดตั้ง Raylib สำเร็จ! 🎉"
        } else {
            Write-ErrorMsg "ติดตั้ง Raylib อัตโนมัติไม่สำเร็จ กรุณาติดตั้งด้วยตนเองโดยเปิด MSYS2 Terminal แล้วรันคำสั่งนี้:"
            Write-Host "   $($Style.Yellow)pacman -S $raylibPackage$($Style.Reset)"
        }
    } else {
        Write-Host "💡 คุณสามารถติดตั้ง Raylib บน MSYS2 ได้โดยเปิด MSYS2 Terminal แล้วรันคำสั่งนี้:"
        Write-Host "   $($Style.Yellow)pacman -S $raylibPackage$($Style.Reset)"
    }
}

# 2. จัดเตรียมโฟลเดอร์ .vscode และการสำรองข้อมูล
$vscodeDir = Join-Path $workspaceDir ".vscode"
if (Test-Path $vscodeDir) {
    $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $backupDir = Join-Path $vscodeDir "backup_$timestamp"
    New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
    
    $filesToBackup = @("c_cpp_properties.json", "tasks.json", "launch.json", "settings.json", "build.ps1")
    foreach ($file in $filesToBackup) {
        $src = Join-Path $vscodeDir $file
        if (Test-Path $src) {
            Copy-Item $src -Destination $backupDir -Force
        }
    }
    Write-Info "สำรองไฟล์การตั้งค่าเก่าเก็บไว้ใน: .vscode\backup_$timestamp\"
} else {
    New-Item -ItemType Directory -Path $vscodeDir -Force | Out-Null
}

$compPathJson = $compilerPath.Replace("\", "/")
$compDirJson = $compilerDir.Replace("\", "/").Replace("/", "\\")
$compPathDoubleBackslash = $compilerPath.Replace("\", "\\")
$compDirJsonForward = $compilerDir.Replace("\", "/")

# 3. สร้าง c_cpp_properties.json
$cCppPropertiesJson = @"
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "`${workspaceFolder}/**",
                "$raylibInc"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "$compPathJson",
            "cStandard": "c99",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64",
            "browse": {
                "path": [
                    "`${workspaceFolder}"
                ],
                "limitSymbolsToIncludedHeaders": true
            }
        }
    ],
    "version": 4
}
"@

[System.IO.File]::WriteAllText((Join-Path $vscodeDir "c_cpp_properties.json"), $cCppPropertiesJson, [System.Text.Encoding]::UTF8)
Write-Host "$($Style.Green)[√] สร้าง c_cpp_properties.json สำเร็จ!$($Style.Reset)"

# 4. สร้าง settings.json
$profileName = "MSYS2 MinGW64"
$shellArgs = '"-NoExit", "-Command", "& ''C:\\msys64\\msys2_shell.cmd'' -mingw64 -defterm -no-start -here"'

if ($compilerType -eq "msys2-ucrt64") {
    $profileName = "MSYS2 UCRT64"
    $shellArgs = '"-NoExit", "-Command", "& ''C:\\msys64\\msys2_shell.cmd'' -ucrt64 -defterm -no-start -here"'
}

$settingsJson = @"
{
    "terminal.integrated.profiles.windows": {
        "$profileName": {
            "path": "powershell.exe",
            "args": [
                "-NoExit",
                "-Command",
                "& 'C:\\msys64\\msys2_shell.cmd' -mingw64 -defterm -no-start -here"
            ],
            "icon": "terminal-powershell"
        }
    },
    "files.associations": {
        "*.h": "c",
        "*.c": "c"
    },
    "files.encoding": "utf8",
    "code-runner.fileDirectoryAsCwd": true,
    "code-runner.runInTerminal": true,
    "editor.formatOnSave": true,
    "C_Cpp.default.cStandard": "c99"
}
"@
# แก้ไขเครื่องหมายคำอ้างสำหรับ settings args
if ($compilerType -eq "msys2-ucrt64") {
    $settingsJson = $settingsJson.Replace("-mingw64", "-ucrt64")
}

[System.IO.File]::WriteAllText((Join-Path $vscodeDir "settings.json"), $settingsJson, [System.Text.Encoding]::UTF8)
Write-Host "$($Style.Green)[√] สร้าง settings.json สำเร็จ!$($Style.Reset)"

# 5. สร้าง build.ps1 (ตัวกลางที่ F5 / Ctrl+Shift+B เรียกใช้ — รองรับได้ทุกกรณี)
#    - ไฟล์เดียว หรือโฟลเดอร์ multi-file (มี main.c + .c อื่น) -> คอมไพล์ทั้งโฟลเดอร์
#    - ตรวจเจอ #include <raylib.h> ในโค้ด/ header ข้างเคียง -> ลิงก์ Raylib ให้เอง
#    - ปิดโปรแกรมเดิมที่ยังรันค้างอยู่ก่อน (ไม่ให้ไฟล์ .exe ถูกล็อกจน link ไม่ผ่าน)
#    - ลิงก์ -lm เสมอ (math.h) และคืน exit code ให้ VS Code หยุดก่อนดีบักถ้า build พัง
$buildPs1 = @'
# build.ps1 - compile ไฟล์ที่เปิดอยู่ หรือทั้งโฟลเดอร์ ถ้าเป็นโปรเจกต์ multi-file
# multi-file = โฟลเดอร์มี main.c และมี .c มากกว่า 1 ไฟล์
# output เป็น <ชื่อไฟล์ที่เปิด>.exe เสมอ ให้ตรงกับ launch.json
# Raylib: ตรวจอัตโนมัติจาก #include "raylib.h" (หรือบังคับด้วย -Raylib)
param(
    [Parameter(Mandatory)][string]$File,
    [switch]$Raylib
)

$gcc    = '__GCC__'
$binDir = '__BIN__'
$usrBin = '__USR__'
$rlInc  = '__INC__'
$rlLib  = '__LIB__'

$File = (Resolve-Path -LiteralPath $File).Path
$dir  = Split-Path $File -Parent
$name = [IO.Path]::GetFileNameWithoutExtension($File)
$out  = Join-Path $dir "$name.exe"

if ([IO.Path]::GetExtension($File) -ne '.c') {
    Write-Host "Active file is not a .c file: $File  (open a .c file, then press F5 again)"
    exit 1
}

$cFiles = @(Get-ChildItem -LiteralPath $dir -Filter *.c | ForEach-Object FullName)
$multi  = (Test-Path (Join-Path $dir 'main.c')) -and ($cFiles.Count -gt 1)
$sources = @(if ($multi) { $cFiles } else { $File })

# ตรวจว่าใช้ Raylib หรือไม่ (สแกน source ที่จะ build + header ในโฟลเดอร์เดียวกัน)
if (-not $Raylib) {
    $scan = @($sources) + @(Get-ChildItem -LiteralPath $dir -Filter *.h | ForEach-Object FullName)
    if (Select-String -LiteralPath $scan -Pattern '#\s*include\s*[<"]raylib\.h[>"]' -Quiet) { $Raylib = $true }
}

$flags = @('-fdiagnostics-color=always', '-g')
$libs  = @()
if ($Raylib) {
    $flags += @('-std=c99', '-Wall', '-Wextra', '-I', $rlInc)
    $libs  += @('-L', $rlLib, '-lraylib', '-lopengl32', '-lgdi32', '-lwinmm')
}
$libs += '-lm'

# ปิดโปรแกรมเดิมที่ยังรันค้าง เพื่อไม่ให้ .exe ถูกล็อก
Get-Process -Name $name -ErrorAction SilentlyContinue |
    Where-Object { $_.Path -eq $out } |
    ForEach-Object { Write-Host "Stopping running $name.exe (PID $($_.Id))"; Stop-Process -Id $_.Id -Force; Start-Sleep -Milliseconds 300 }

$leafs = ($sources | ForEach-Object { Split-Path $_ -Leaf }) -join ' '
$mode  = if ($Raylib) { 'Raylib' } else { 'Console' }
Write-Host "Building [$mode] $out from: $leafs"

$env:PATH = "$binDir;$usrBin;$env:PATH"
Push-Location $dir
& $gcc @flags @sources -o $out @libs
$code = $LASTEXITCODE
Pop-Location
if ($code -eq 0) { Write-Host "Build OK" } else { Write-Host "Build FAILED (exit $code)" }
exit $code
'@

$msysRoot = "C:\msys64"
$buildPs1 = $buildPs1.
    Replace("__GCC__", $compilerPath).
    Replace("__BIN__", $compilerDir).
    Replace("__USR__", "$msysRoot\usr\bin").
    Replace("__INC__", $raylibInc).
    Replace("__LIB__", $raylibLib)

# บันทึกแบบ UTF-8 with BOM เพื่อให้ Windows PowerShell 5.1 อ่านคอมเมนต์ภาษาไทยได้ถูกต้อง
[System.IO.File]::WriteAllText((Join-Path $vscodeDir "build.ps1"), $buildPs1, (New-Object System.Text.UTF8Encoding($true)))
Write-Host "$($Style.Green)[√] สร้าง build.ps1 สำเร็จ!$($Style.Reset)"

# 6. สร้าง tasks.json
$tasksJson = @"
{
    "version": "2.0.0",
    "tasks": [
        {
            "type": "shell",
            "label": "C/C++: Build (MSYS2)",
            "command": "powershell",
            "args": [
                "-NoProfile",
                "-ExecutionPolicy", "Bypass",
                "-File", "`${workspaceFolder}\\.vscode\\build.ps1",
                "`${file}"
            ],
            "options": {
                "cwd": "`${fileDirname}"
            },
            "problemMatcher": [
                "`$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "detail": "Build active file, or whole folder if it has main.c + other .c files (multi-file). Auto-links Raylib when raylib.h is included."
        },
        {
            "type": "cppbuild",
            "label": "C/C++: gcc.exe build active file",
            "command": "$compPathJson",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "`${file}",
                "-o",
                "`${fileDirname}\\`${fileBasenameNoExtension}.exe"
            ],
            "options": {
                "cwd": "$compDirJsonForward"
            },
            "problemMatcher": [
                "`$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Task generated by Debugger."
        },
        {
            "type": "shell",
            "label": "C/C++: Compile with Raylib",
            "command": "powershell",
            "args": [
                "-NoProfile",
                "-ExecutionPolicy", "Bypass",
                "-File", "`${workspaceFolder}\\.vscode\\build.ps1",
                "`${file}",
                "-Raylib"
            ],
            "options": {
                "cwd": "`${fileDirname}"
            },
            "problemMatcher": [
                "`$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "detail": "คอมไพล์โปรแกรมภาษา C ร่วมกับ Library Raylib 5.0 (สำหรับเกม/กราฟิก)"
        }
    ]
}
"@

[System.IO.File]::WriteAllText((Join-Path $vscodeDir "tasks.json"), $tasksJson, [System.Text.Encoding]::UTF8)
Write-Host "$($Style.Green)[√] สร้าง tasks.json สำเร็จ!$($Style.Reset)"

# 7. สร้าง launch.json
$gdbPath = "$compilerDir\gdb.exe".Replace("\", "/")
if (-not (Test-Path "$compilerDir\gdb.exe")) {
    $gdbPath = "C:/msys64/mingw64/bin/gdb.exe"
}

$launchJson = @"
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (Console Program)",
            "type": "cppdbg",
            "request": "launch",
            "program": "`${fileDirname}\\`${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "`${fileDirname}",
            "environment": [
                {
                    "name": "PATH",
                    "value": "$compDirJson;`${env:PATH}"
                }
            ],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "$gdbPath",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: Build (MSYS2)"
        },
        {
            "name": "Debug (Raylib Graphics/Game)",
            "type": "cppdbg",
            "request": "launch",
            "program": "`${fileDirname}\\`${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "`${fileDirname}",
            "environment": [
                {
                    "name": "PATH",
                    "value": "$compDirJson;`${env:PATH}"
                }
            ],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "$gdbPath",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: Compile with Raylib"
        }
    ]
}
"@

[System.IO.File]::WriteAllText((Join-Path $vscodeDir "launch.json"), $launchJson, [System.Text.Encoding]::UTF8)
Write-Host "$($Style.Green)[√] สร้าง launch.json สำเร็จ!$($Style.Reset)"

# --- บทสรุปการทำงาน ---
Write-Section "การตั้งค่าสำเร็จสมบูรณ์! 🎉"
Write-Host "$($Style.Bold)🎮 วิธีใช้งานใน VS Code:$($Style.Reset)"
Write-Host " 1. $($Style.Bold)การ คอมไพล์ และ รัน (Build Task):$($Style.Reset)"
Write-Host "    - กดปุ่ม $($Style.Cyan)Ctrl + Shift + B$($Style.Reset)"
Write-Host "    - เลือก $($Style.Bold)'C/C++: Build (MSYS2)'$($Style.Reset) สำหรับโปรแกรม C ทั่วไป"
Write-Host "    - เลือก $($Style.Bold)'C/C++: Compile with Raylib'$($Style.Reset) สำหรับโค้ดกราฟิก/เกม Raylib"
Write-Host ""
Write-Host " 2. $($Style.Bold)การ ดีบัก โค้ด (Debugging ด้วย F5):$($Style.Reset)"
Write-Host "    - ไปยังเมนู Run & Debug ($($Style.Cyan)Ctrl + Shift + D$($Style.Reset))"
Write-Host "    - เลือกตัวสลับหน้าจอตามการพัฒนา:"
Write-Host "      * $($Style.Bold)'Debug (Console Program)'$($Style.Reset) -> รันในหน้าจอดำภายนอก (แนะนำ สำหรับ input ข้อมูล)"
Write-Host "      * $($Style.Bold)'Debug (Raylib Graphics/Game)'$($Style.Reset) -> รันหน้าต่างเกม พร้อมเปิดหน้าจอดำภายนอกสำหรับ printf/cls"
Write-Host "    - กดปุ่ม $($Style.Cyan)F5$($Style.Reset) เพื่อคอมไพล์และเริ่มดีบักทันที!"
Write-Host ""
Write-Host " 3. $($Style.Bold)ระบบการจัดระเบียบโค้ดอัตโนมัติ (Format On Save):$($Style.Reset)"
Write-Host "    - เมื่อกดบันทึกไฟล์ (Save) โค้ดของท่านจะถูกจัดโครงสร้างให้สวยงามโดยอัตโนมัติ"
Write-Host ""
Write-Host "$($Style.Bold)ขอให้สนุกและมีความสุขกับการเรียนเขียนโปรแกรมที่ DG111! 🚀$($Style.Reset)"
Write-Host "--------------------------------------------------------"
Write-Host "กดปุ่มใดก็ได้เพื่อออก..."
[void][System.Console]::ReadKey($true)
