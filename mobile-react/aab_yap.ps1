$env:GRADLE_USER_HOME = "C:\Users\Rahman\source\apps\_cache\gradle"
$env:NPM_CONFIG_CACHE  = "C:\Users\Rahman\source\apps\_cache\npm"
$env:JAVA_HOME         = "C:\Program Files\Android\Android Studio\jbr"
$env:ANDROID_HOME      = "C:\Users\Rahman\AppData\Local\Android\Sdk"

Set-Location -Path $PSScriptRoot

Write-Host "AAB (Release) derleniyor..." -ForegroundColor Cyan
Set-Location -Path "$PSScriptRoot\android"
.\gradlew.bat bundleRelease

$aabPath = "app\build\outputs\bundle\release\app-release.aab"
$destPath = "$PSScriptRoot\com.rahmanyazgan.finansasistani.aab"

if (Test-Path $aabPath) {
    Copy-Item -Path $aabPath -Destination $destPath -Force
    Write-Host "`n==========================================" -ForegroundColor Cyan
    Write-Host "[BASARILI] AAB olusturuldu:" -ForegroundColor Green
    Write-Host "Dosya: $destPath" -ForegroundColor Yellow
    Write-Host "==========================================" -ForegroundColor Cyan
} else {
    Write-Host "`n==========================================" -ForegroundColor Red
    Write-Host "[HATA] AAB dosyasi bulunamadi." -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
}

Write-Host "`nCikmak icin herhangi bir tusa basin..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
