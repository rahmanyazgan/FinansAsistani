Write-Host "APK (Release) derleniyor..." -ForegroundColor Cyan
Set-Location -Path $PSScriptRoot
$env:GRADLE_USER_HOME = "C:\Users\Rahman\source\apps\_cache\gradle"
$env:JAVA_HOME        = "C:\Program Files\Android\Android Studio\jbr"
$env:ANDROID_HOME     = "C:\Users\Rahman\AppData\Local\Android\Sdk"
.\gradlew.bat assembleRelease

$apkPath = "app\build\outputs\apk\release\app-release.apk"
$destPath = "com.rahmanyazgan.finansasistani.apk"

if (Test-Path $apkPath) {
    Copy-Item -Path $apkPath -Destination $destPath -Force
    Write-Host "Basarili! APK dosyasi olusturuldu: $destPath" -ForegroundColor Green
} else {
    Write-Host "Hata! APK dosyasi bulunamadi." -ForegroundColor Red
}
