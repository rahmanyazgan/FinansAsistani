# Finansal Hesap Makinesi - Uygulama Spesifikasyonu (Spec)

Bu döküman, `calculators.pyw` dosyasında geliştirilen çok sekmeli PyQt5 finansal hesap makinesi uygulamasının yapısını, içerdiklerini ve çalışma mantığını açıklar. İleride yapılacak geliştirmeler için referans kaynağıdır.

## 1. Genel Mimari ve Teknoloji Yığını
- **Dil:** Python 3.x
- **Kütüphane:** PyQt5 (kullanıcı arayüzü), `requests` (API erişimi)
- **Dosya Yapısı:** Tek dosya uygulaması (`calculators.pyw`). Yapılandırmalar ve veri önbelleği yerel diske (`settings.json`) kaydedilir.
- **Tasarım Deseni (Design Pattern):**
  - Nesne Yönelimli Programlama (OOP) (Sınıf tabanlı widget yapısı)
  - Abstraction & Factory Pattern (Farklı kur sağlayıcılarını kolayca entegre etmek için `RateProvider` taban sınıfı ve `create_provider` fabrikası)
- **Asenkron Çalışma:** API istekleri arayüzü dondurmaması için `QThread` (arka plan iş parçacığı) kullanılarak yapılır.

## 2. Arayüz Özellikleri ve Temalandırma
- **Tema Mekanizması (`generate_style`, `get_palette`):**
  - Sabit CSS yerine `generate_style(theme_dict)` fonksiyonu ile dinamik CSS ve `get_palette(theme_dict)` ile `QPalette` üretilir.
  - `THEMES` sözlüğünde şu an için 6 farklı tema bulunur: Sade (açık tema), Koyu Mavi, Mor Gece, Yeşil Orman, Okyanus, Gün Batımı.
  - Tema seçimi, ekranın sağ altında (Statusbar içinde) yer alan bir `QPushButton` + `QMenu` kombinasyonuyla yapılır.
  - Menü, program dışına / aşağıya taşmasını engellemek için butonun tam üzerinde yukarı doğru açılacak şekilde (`pos.y() - sizeHint().height()`) pozisyonlanır.
  - Seçilen tema anında uygulanır ve kalıcı olması için doğrudan `settings.json` dosyasına `"theme"` anahtarıyla kaydedilir.

## 3. Modüller ve Sekmeler

### 3.1. Sekme 1: Kur Dönüşümü (`CurrencyTab`)
- **Amacı:** Bir para birimini diğerine dönüştürme ve canlı döviz kurlarını görüntüleme.
- **Veri Kaynağı (`ExchangeRateProvider`):** "ExchangeRate API" (`https://open.er-api.com/v6`) - Ücretsiz, API anahtarı gerektirmez ve 150+ para birimine erişim sağlar.
- **Para Birimleri Listesi:**
  - Kurların isimlerini çekmek için harici istek atıp bekletmemek adına 45 popüler para birimi ve Türkçe adları `CURRENCY_NAMES` sözlüğünde statik olarak tanımlanmıştır. Öncelikli kurlar (TRY, USD, EUR, GBP) listenin en üstünde yer alır.
- **Performans Optimizasyonu:**
  - Kurlar bellekte (cache) tutulur ve varsayılan olarak **10 dakika** boyunca tekrar API'ye istek atılmaz (`cache_ttl_seconds`).
- **Özellikler:**
  - `⇅ Değiştir` butonu hedef ve kaynak kurları anında yer değiştirir (swap).

### 3.2. Sekme 2: Yüzde Hesabı (`PercentTab`)
- **Amacı:** Gündelik ve finansal yüzdelik matematik hesaplamaları.
- **Hesaplama Modları `QRadioButton` ile Seçilir:**
  1. *Bir sayının %X'i kaçtır?* (Örn: 100'ün %20'si = 20)
  2. *X, Y'nin yüzde kaçıdır?* (Örn: 50, 200'ün %25'idir)
  3. *Yüzde değişim (artış/azalış):* (Zarar/kar hesaplamaları. Formül: `((Yeni - Eski) / |Eski|) * 100`)
  4. *İki değer arası yüzdelik fark:* Kesin iki büyüklüğün kıyası.
  5. *KDV Hesaplama:* KDV Hariçten Dahile ya da Dahilden Harice dönüşüm. (%1, %10, %20 oranları ile)
- **Detay Görünümü:** Her hesap sonucunun altındaki silik / italic yazıyla (`infoLabel` / `detail_label`) kullanılan matematiksel formül ve döküm açıkça gösterilir.

### 3.3. Sekme 3: Gelir Vergisi İadesi Hesaplama (`TaxTab`)
- **Amacı:** 2026 Türkiye Gelir Vergisi Mevzuatına göre brüt gelirden net ödenecek vergiyi bulmak ve Hayat/Şahıs sigortası indirimlerini hesaplamak.
- **Algoritmayan ve Vergi Dilimleri:**
  - Ücret (Maaşlı çalışan) ve Ücret Dışı (Serbest meslek) gelirler için iki ayrı dilim dizisi (`UCRET_VERGI_DILIMLERI_2026`, `UCRET_DISI_VERGI_DILIMLERI_2026`).
  - %15, %20, %27, %35 ve %40 olmak üzere artan oranlı dilim sistemi fonksiyonu (`hesapla_vergi`). Marjinal oranı bulan `marjinal_vergi_orani` fonksiyonu.
- **Sigorta Primi İndirimi Kuralları:**
  - Birikimli hayat sigortası: Primlerin **%50'si**.
  - Diğer şahıs sigortaları: Primlerin **%100'ü**.
  - Yasal üst sınır: Beyan edilen gelirin (brüt maaşın) **%15'i** VE **yıllık brüt asgari ücret** (`YILLIK_BRUT_ASG_UCRET_2026 = 312.066 ₺`) miktarı aşılamaz.
- **Çıktı Raporu:** `QTextEdit` üzerinde monospaced (eşit aralıklı) font kullanılarak fiş/fatura formatında detaylı (vergi matrahı, indirim edilen kısım, dilim dilim ödenen vergi) bir metin raporu çıkarılır.

## 4. Kullanıcı Ayarları (`settings.json`)
- Uygulama çalıştırıldığında aynı klasörde (`Path(__file__).parent / "settings.json"`) bir ayar dosyası oluşturulur veya okunur.
- **İçerik:**
  - `"theme"`: Seçili temanın adı (Örn: "Sade").
  - `"active_provider"`: Hangi kur API'sinin kullanılacağı (Örn: "exchangerate").
  - `"providers"`: API ayarları (Base URL, gerekirse Api Key).
  - `"cache_ttl_seconds"`: Kur önbellekleme süresi saniye cinsinden.

## 5. İleride Eklenebilecek Özellikler (Roadmap)
İhtiyaç halinde bu spec dosyasına bakılarak şu özellikler eklenebilir:
1. **Basit / Bileşik Faiz veya Kredi Hesaplama Sekmesi:** Yeni bir `QWidget` sınıfı olarak sekmeler arasına eklenebilir.
2. **Kripto Kurları:** `ExchangeRateProvider` haricinde bir kripto API'si (`CoinGeckoProvider` gibi) yaratılıp sisteme dahil edilebilir.
3. **Çevrimdışı Çalışma Mimarisi (Offline Mode):** API çekilemediğinde önbellek dosyasından veya yerel veritabanından kur çekilmesi sağlanabilir.
4. **Veri Değişimi:** Kullanıcının girmiş olduğu verileri / sonuçları (özellikle vergi raporunu) PDF veya TXT dosyası olarak dışa aktarması sağlanabilir (`QPushButton` -> Dışa Aktar).
