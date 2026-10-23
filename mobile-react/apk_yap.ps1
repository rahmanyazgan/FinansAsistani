$env:GRADLE_USER_HOME = "C:\Users\Rahman\source\apps\_cache\gradle"
$env:NPM_CONFIG_CACHE  = "C:\Users\Rahman\source\apps\_cache\npm"
$env:JAVA_HOME         = "C:\Program Files\Android\Android Studio\jbr"
$env:ANDROID_HOME      = "C:\Users\Rahman\AppData\Local\Android\Sdk"

Set-Location -Path $PSScriptRoot

Write-Host "APK (Release) derleniyor..." -ForegroundColor Cyan
Set-Location -Path "$PSScriptRoot\android"
.\gradlew.bat assembleRelease

$apkPath = "app\build\outputs\apk\release\app-release.apk"
$unsignedApkPath = "app\build\outputs\apk\release\app-release-unsigned.apk"
$destPath = "$PSScriptRoot\com.rahmanyazgan.finansasistani.apk"

if (-not (Test-Path $apkPath) -and (Test-Path $unsignedApkPath)) {
    $apkPath = $unsignedApkPath
}

if (Test-Path $apkPath) {
    Copy-Item -Path $apkPath -Destination $destPath -Force
    Write-Host "`n==========================================" -ForegroundColor Cyan
    Write-Host "[BASARILI] APK olusturuldu:" -ForegroundColor Green
    Write-Host "Dosya: $destPath" -ForegroundColor Yellow
    Write-Host "==========================================" -ForegroundColor Cyan
} else {
    Write-Host "`n==========================================" -ForegroundColor Red
    Write-Host "[HATA] APK dosyasi bulunamadi." -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
}

Write-Host "`nCikmak icin herhangi bir tusa basin..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
