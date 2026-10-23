# Finans Asistanı — C++ / Qt 6 Masaüstü Sürümü (`desktop-cpp-qt`)

PyQt5 tabanlı Finans Asistanı masaüstü uygulamasının C++17 ve Qt 6 Widgets mimarisine taşınmış modern sürümüdür.

## Özellikler
- **💱 Canlı Kur Dönüşümü:** ExchangeRate API üzerinden anlık kur çekme, 45+ para birimi, bellek ve disk önbellekleme (`rates_cache.json`), çevrimdışı (offline) modu.
- **📊 Yüzde Hesabı:** 5 farklı matematiksel ve finansal mod (X'in %Y'si, % değişim, KDV dahil/hariç vb.).
- **🏛️ Gelir Vergisi & Hayat Sigortası İadesi:** 2026 Gelir Vergisi dilimleri ve kümülatif matrah hesabı, dinamik çoklu poliçe ekleme, aylık bordro yansıma tablosu, modern özet dashboard'u, PNG & PDF dışa aktarma.
- **📈 Bileşik Kar Hesaplayıcısı:** Günlük/haftalık/aylık bileşik frekanslar, varyans aralığı, etkileşimli QtCharts grafik eğrisi ve yıllık döküm tablosu.
- **🎨 6 Dinamik Tema:** Okyanus, Sade, Koyu Mavi, Mor Gece, Yeşil Orman, Gün Batımı.
- **💾 Kalıcı Ayarlar:** `settings.json` ile tema, son sekme ve vergi dilimleri yapılandırması.

---

## 1. Hızlı Derleme ve Dağıtım (PowerShell)

Projeyi derlemek ve `dist/` klasörüne bağımsız çalışabilir (portable) exe paketini çıkarmak için:

```powershell
cd desktop-cpp-qt
.\build.ps1
```

Bu script sırasıyla:
1. `_cache\qt` altındaki Qt 6.8.3, MinGW 13.1.0 ve Ninja araçlarını otomatik kullanır.
2. CMake ile Release modunda derler.
3. Python sürümüyle tam matematiksel uyuşmayı denetleyen **Parity Test**'lerini çalıştırır.
4. `desktop-cpp-qt/dist` klasörünü oluşturur ve `windeployqt` ile tüm gerekli Qt DLL'lerini kopyalar.

Üretilen çıktı:
`desktop-cpp-qt/dist/FinansAsistani.exe` (Çift tıklayarak doğrudan çalıştırabilirsiniz).

---

## 2. Qt Creator Entegrasyonu

Qt Creator'da projeyi açıp geliştirme ve hata ayıklama (debug) yapmak için bir defaya mahsus şu adımları izleyin:

1. **Qt Creator**'ı açın.
2. **Edit (Düzenle) → Preferences (Tercihler) → Kits (Kitler)** menüsüne gidin:
   - **Qt Versions:** `Add` butonuna tıklayın ve şu dosyayı seçin:  
     `C:\Users\Rahman\source\apps\_cache\qt\6.8.3\mingw_64\bin\qmake.exe`
   - **Compilers (Derleyiciler):** MinGW g++ otomatik algılanmadıysa `Add → GCC → C++` seçip yolu girin:  
     `C:\Users\Rahman\source\apps\_cache\qt\Tools\mingw1310_64\bin\g++.exe`
   - **Kits:** Yeni bir kit oluşturup yukarıdaki Qt Version ve Compiler'ı eşleştirin. CMake generator olarak Ninja'yı seçin.
3. **File (Dosya) → Open File or Project (Dosya veya Proje Aç)** ile `desktop-cpp-qt/CMakeLists.txt` dosyasını seçin.
4. Projenizi doğrudan Qt Creator içerisinden `Ctrl+R` ile derleyip çalıştırabilirsiniz.

---

## Klasör Yapısı
```
desktop-cpp-qt/
├── CMakeLists.txt          # CMake yapılandırması (Widgets, Network, Charts, PrintSupport)
├── build.ps1               # Tek tıkla derleme ve windeployqt dağıtım betiği
├── resources.qrc           # Gömülü ikon ve kaynaklar
├── app.rc & app.ico        # Windows exe ikonu
├── src/
│   ├── main.cpp            # Uygulama başlangıcı ve tema başlatma
│   ├── mainwindow.h/.cpp   # Ana pencere, sekmeler ve menüler
│   ├── theme.h/.cpp        # 6 tema paleti ve CSS üretici
│   ├── settings.h/.cpp     # settings.json yönetimi
│   ├── rateprovider.h/.cpp # Asenkron kur sağlayıcı ve fabrika
│   ├── taxcalc.h/.cpp      # Saf C++ vergi ve dilim algoritması
│   ├── compound.h/.cpp     # Saf C++ bileşik faiz algoritması
│   ├── util.h/.cpp         # Sayı biçimlendirme ve ayrıştırma yardımcıları
│   ├── tabs/
│   │   ├── currencytab.h/.cpp
│   │   ├── percenttab.h/.cpp
│   │   ├── taxtab.h/.cpp
│   │   └── compoundinteresttab.h/.cpp
│   └── dialogs/
│       ├── taxsettingsdialog.h/.cpp
│       └── resultdetailsdialog.h/.cpp
├── tests/
│   └── taxcalc_test.cpp    # Matematiksel doğruluk parite testleri
└── dist/                   # Çalıştırılabilir taşınabilir paket
    └── FinansAsistani.exe
```
