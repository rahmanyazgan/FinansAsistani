Write-Host "AAB (Release) derleniyor..." -ForegroundColor Cyan
Set-Location -Path $PSScriptRoot
$env:GRADLE_USER_HOME = "C:\Users\Rahman\source\apps\_cache\gradle"
$env:JAVA_HOME        = "C:\Program Files\Android\Android Studio\jbr"
$env:ANDROID_HOME     = "C:\Users\Rahman\AppData\Local\Android\Sdk"
.\gradlew.bat bundleRelease

$aabPath = "app\build\outputs\bundle\release\app-release.aab"
$destPath = "com.rahmanyazgan.finansasistani.aab"

if (Test-Path $aabPath) {
    Copy-Item -Path $aabPath -Destination $destPath -Force
    Write-Host "Basarili! AAB dosyasi olusturuldu: $destPath" -ForegroundColor Green
} else {
    Write-Host "Hata! AAB dosyasi bulunamadi." -ForegroundColor Red
}
