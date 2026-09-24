# build.ps1 - compile ไฟล์ที่เปิดอยู่ หรือทั้งโฟลเดอร์ ถ้าเป็นโปรเจกต์ multi-file
# multi-file = โฟลเดอร์มี main.c และมี .c มากกว่า 1 ไฟล์
# output เป็น <ชื่อไฟล์ที่เปิด>.exe เสมอ ให้ตรงกับ launch.json
# Raylib: ตรวจอัตโนมัติจาก #include "raylib.h" (หรือบังคับด้วย -Raylib)
param(
    [Parameter(Mandatory)][string]$File,
    [switch]$Raylib
)

$gcc    = 'C:\msys64\mingw64\bin\gcc.exe'
$binDir = 'C:\msys64\mingw64\bin'
$usrBin = 'C:\msys64\usr\bin'
$rlInc  = 'C:/msys64/mingw64/include'
$rlLib  = 'C:/msys64/mingw64/lib'

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