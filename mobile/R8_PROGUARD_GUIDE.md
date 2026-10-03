# R8 / Proguard Yapılandırma ve Build Rehberi

İleride Proguard / R8 (kod karartma ve küçültme) özelliğini aktif etmek istediğinizde aşağıdaki adımları sırasıyla uygulayabilirsiniz.

---

### 1. `app.json` Dosyasına Proguard Ayarını Eklemek
[app.json](file:///c:/Users/Rahman/source/apps/calculators/mobile/app.json) dosyasındaki `"android"` objesi altına `enableProguardInReleaseBuilds` ayarını ekleyin:

```json
"android": {
  "package": "com.rahmanyazgan.finansasistani",
  "enableProguardInReleaseBuilds": true,
  ...
}
```

---

### 2. (Opsiyonel) Özel Proguard Kuralları Gerekirse
`expo-build-properties` plugin'ini kullanarak özel Proguard kuralları tanımlayabilirsiniz:

1. Paketi yükleyin:
   ```bash
   npx expo install expo-build-properties
   ```
2. [app.json](file:///c:/Users/Rahman/source/apps/calculators/mobile/app.json) içerisindeki `"plugins"` listesine ekleyin:
   ```json
   "plugins": [
     "expo-font",
     [
       "expo-build-properties",
       {
         "android": {
           "enableProguardInReleaseBuilds": true
         }
       }
     ]
   ]
   ```

---

### 3. Yeni Production Build Alma
Değişiklikleri yaptıktan sonra EAS CLI ile yeni build tetikleyin:

```bash
eas build --platform android --profile production
```

---

### 4. `mapping.txt` Dosyasını Google Play Console'a Yükleme
Build tamamlandıktan sonra:
1. [Expo Dashboard](https://expo.dev) üzerinden projenize ve ilgili build sayfasına gidin.
2. **Artifacts** bölümünden oluşturulan `mapping.txt` (veya Deobfuscation file) dosyasını indirin.
3. **Google Play Console** > **Sürüm oluştur** (veya Uygulama Paketi Gezgini) > **İlgili Sürüm** > **Kod Gösterme Dosyaları (Deobfuscation Files)** alanından bu `mapping.txt` dosyasını yükleyin.

---

> **Not:** Şimdilik bu işlemi yapmadan mevcut `.aab` dosyanız ile yayına devam edebilirsiniz; Google Play bu uyarıya rağmen uygulamanızı onaylar.
