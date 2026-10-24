# 🏛️ Finans Asistanı

**Finans Asistanı**, hem masaüstünde (**C++ / Qt 6** ve **Python / PyQt5**) hem de mobilde (**Native Android Kotlin** ve **React Native / Expo**) çalışan çok platformlu, modern ve kullanıcı dostu bir finansal hesaplama aracıdır.

---

## 📁 Proje Klasör Mimarisi

| Klasör | Teknoloji | Açıklama |
|---|---|---|
| **[desktop-cpp-qt](file:///c:/Users/Rahman/source/apps/calculators/desktop-cpp-qt)** | C++17, Qt 6 Widgets | Yüksek performanslı masaüstü sürümü. Tek tıkla çalışan **13.8 MB** taşınabilir (portable single-file) `.exe` üretir. |
| **[desktop-python-qt](file:///c:/Users/Rahman/source/apps/calculators/desktop-python-qt)** | Python 3, PyQt5 | İlk prototip ve PyQt5 tabanlı masaüstü hesaplama uygulaması. |
| **[mobile-kotlin](file:///c:/Users/Rahman/source/apps/calculators/mobile-kotlin)** | Kotlin, Jetpack Compose | Modern Android standartlarında geliştirilmiş yerel (native) Android uygulaması. |
| **[mobile-react](file:///c:/Users/Rahman/source/apps/calculators/mobile-react)** | React Native, TypeScript, Expo | Çift platform (Android & iOS) destekli mobil uygulama sürümü. |
| **[assets](file:///c:/Users/Rahman/source/apps/calculators/assets)** | Ortak Kaynaklar | Uygulamaların ortak kullandığı ikonlar, logolar ve grafikler. |

---

## 🚀 Öne Çıkan Özellikler

### 1. 💱 Kur Dönüşümü
* **Canlı Veri:** `ExchangeRate API` üzerinden 150+ para birimi için anlık döviz kurları.
* **Akıllı Önbellek & Çevrimdışı Modu:** İnternet kesintilerinde yerel önbellekten son kurları kullanabilme.
* **Hızlı Takas:** Tek butonla kaynak ve hedef kurları anında yer değiştirme.

### 2. 🏛️ Vergi İadesi & Sigorta Prim İndirimi
* **2026 Gelir Vergisi Dilimleri:** Ücretli ve ücret dışı çalışanlar için kümülatif dilim (%15, %20, %27, %35, %40) hesaplamaları.
* **Dinamik Poliçe Yönetimi:** Birden fazla hayat ve sağlık poliçesini başlangıç ayı ve para birimiyle (TRY/USD) ekleyebilme.
* **Aylık Net Bordro Yansıması:** Yıllık indirim sınırları ve her ay maaşa yansıyan net iade dökümü.
* **Dashboard & Dışa Aktarma:** Grafik dökümü, ASCII tablo ve tek tıkla **PNG / PDF** formatında rapor kaydetme.

### 3. 📊 Yüzde Hesaplayıcı
* Bir sayının %X'i kaçtır?
* X, Y'nin yüzde kaçıdır?
* Yüzde değişim (artış / azalış).
* İki değer arası yüzdelik fark.
* KDV dahil ↔ hariç dönüşümleri (%1, %10, %20).

### 4. 📈 Bileşik Kar Hesaplayıcı
* İlk yatırım, aylık katkı ve vadeye göre gelecekteki değer (FV) simülasyonu.
* Günlük, haftalık, aylık ve yıllık bileşik frekans seçenekleri.
* Varyans aralığı (±%) ile düşük ve yüksek piyasa senaryoları.
* Etkileşimli büyüme grafiği ve yıllık döküm tablosu.

---

## 🎨 Tema Seçenekleri
Göz yormayan 6 farklı modern tema desteği:
* **Okyanus (Varsayılan)**
* **Koyu Mavi**
* **Mor Gece**
* **Sade (Açık Tema)**
* **Yeşil Orman**
* **Gün Batımı**

---

## 📦 Kurulum ve Çalıştırma

### 1. 💻 Masaüstü C++ / Qt 6 (Önerilen)
En hızlı ve en kompakt masaüstü deneyimi:
```powershell
cd desktop-cpp-qt
.\build.ps1
```
* **Çıktı:** `desktop-cpp-qt/dist/FinansAsistani.exe` (Hiçbir harici DLL gerektirmeyen tek dosya, ~13.8 MB portable paket).

### 2. 🐍 Masaüstü Python / PyQt5
```bash
cd desktop-python-qt
pip install PyQt5 requests PyQtChart
python calculators.pyw
```

### 3. 📱 Mobil Native Android (Kotlin & Jetpack Compose)
`mobile-kotlin` klasörünü Android Studio ile açabilir veya kökteki betikleri kullanabilirsiniz:
* **APK Üretimi:** `apk_yap.ps1`
* **Google Play (AAB) Üretimi:** `aab_yap.ps1`

### 4. ⚛️ Mobil React Native & Expo
```bash
cd mobile-react
npm install
npx expo start
```

---

## 📜 Lisans & Bilgi
Bu proje geliştirme ve kişisel kullanım amaçlıdır.

**Geliştirici:** Rahman Yazgan (rahmanyazgan@gmail.com)
