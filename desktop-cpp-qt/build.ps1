# Finans Asistanı Qt C++ Build & Deploy Script (Tek Dosya & Klasör Dağıtımı)
$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $ScriptDir

$CacheQt = "C:\Users\Rahman\source\apps\_cache\qt"
$QtVersion = "6.8.3"
$QtBin = "$CacheQt\$QtVersion\mingw_64\bin"
$QtLib = "$CacheQt\$QtVersion\mingw_64\lib"
$QtCMake = "$CacheQt\$QtVersion\mingw_64\lib\cmake"
$MinGWBin = "$CacheQt\Tools\mingw1310_64\bin"
$NinjaBin = "$CacheQt\Tools\Ninja"
$7zBin = "C:\Program Files\7-Zip\7z.exe"

if (-not (Test-Path $QtBin)) {
    Write-Error "Qt bin klasörü bulunamadı: $QtBin"
}

# Ortam değişkenleri
$env:PATH = "$MinGWBin;$NinjaBin;$QtBin;$env:PATH"
$env:CMAKE_PREFIX_PATH = "$CacheQt\$QtVersion\mingw_64"

Write-Host "=== 1. CMake Yapılandırması (Ninja) ===" -ForegroundColor Cyan
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$CacheQt\$QtVersion\mingw_64"

Write-Host "`n=== 2. Derleme Başlatılıyor ===" -ForegroundColor Cyan
cmake --build build --config Release

Write-Host "`n=== 3. Doğrulama Testi (Parity Test) ===" -ForegroundColor Cyan
$TestExe = "$ScriptDir\build\taxcalc_test.exe"
if (Test-Path $TestExe) {
    & $TestExe
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Test başarısız oldu!"
    }
} else {
    Write-Warning "Test exe bulunamadı: $TestExe"
}

Write-Host "`n=== 4. Dağıtım Paketi Hazırlanıyor ===" -ForegroundColor Cyan
$DistDir = "$ScriptDir\dist"
$BundleDir = "$ScriptDir\build\bundle"

if (Test-Path $BundleDir) { Remove-Item -Recurse -Force $BundleDir }
if (Test-Path $DistDir) { Remove-Item -Recurse -Force $DistDir }
New-Item -ItemType Directory -Path $BundleDir | Out-Null
New-Item -ItemType Directory -Path $DistDir | Out-Null

$BuiltExe = "$ScriptDir\build\FinansAsistani.exe"
if (-not (Test-Path $BuiltExe)) {
    Write-Error "Derlenen exe bulunamadı: $BuiltExe"
}

Copy-Item $BuiltExe "$BundleDir\FinansAsistani.exe"

Write-Host "`n=== 5. windeployqt ile Bağımlılıklar Paketleniyor (OpenGL-SW eleniyor) ===" -ForegroundColor Cyan
& "$QtBin\windeployqt.exe" --release --no-translations --compiler-runtime --no-opengl-sw "$BundleDir\FinansAsistani.exe"

# Gereksiz 20MB opengl32sw ve D3Dcompiler sil
Remove-Item "$BundleDir\opengl32sw.dll" -ErrorAction SilentlyContinue
Remove-Item "$BundleDir\D3Dcompiler_47.dll" -ErrorAction SilentlyContinue

# MinGW ek DLL'leri ekle
$MinGwDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
foreach ($dll in $MinGwDlls) {
    $src = "$MinGWBin\$dll"
    if (Test-Path $src) {
        Copy-Item $src "$BundleDir\$dll" -Force
    }
}

Write-Host "`n=== 6. Tek Dosya EXE (Single-File Portable) Üretiliyor ===" -ForegroundColor Cyan
$ZipPath = "$ScriptDir\build\payload.zip"
if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }

if (Test-Path $7zBin) {
    & $7zBin a -tzip $ZipPath "$BundleDir\*" -mx=9 | Out-Null
} else {
    tar.exe -cf $ZipPath -C "$BundleDir" .
}

$LauncherRc = "$ScriptDir\build\launcher.rc"
$LauncherRes = "$ScriptDir\build\launcher.res"
@"
IDI_ICON1 ICON "$($ScriptDir.Replace('\','/'))/app.ico"
101 RCDATA "$($ZipPath.Replace('\','/'))"
"@ | Set-Content $LauncherRc -Encoding ASCII

windres.exe -i $LauncherRc -O coff -o $LauncherRes
gcc.exe -mwindows -O3 "$ScriptDir\src\launcher.c" $LauncherRes -o "$DistDir\FinansAsistani.exe"

$SingleExe = Get-Item "$DistDir\FinansAsistani.exe"
$SizeMB = [math]::Round($SingleExe.Length / 1MB, 2)

Write-Host "`n[BAŞARILI] Tek Dosya Portable Exe Oluşturuldu!" -ForegroundColor Green
Write-Host "Dosya: $($SingleExe.FullName)" -ForegroundColor Yellow
Write-Host "Boyut: $SizeMB MB (PyQt 39.2 MB sürümünden çok daha kompakt!)`n" -ForegroundColor Green

Get-ChildItem $DistDir | Select-Object Name, @{Name="Boyut (MB)"; Expression={[math]::Round($_.Length / 1MB, 2)}}
