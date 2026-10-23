#include "taxtab.h"
#include "util.h"
#include "dialogs/resultdetailsdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QTextEdit>
#include <QScrollArea>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QJsonArray>
#include <QDate>
#include <algorithm>
#include <cmath>

InsuranceItemWidget::InsuranceItemWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 5);

    m_ayCombo = new QComboBox(this);
    m_ayCombo->addItems({"Ocak", "Şubat", "Mart", "Nisan", "Mayıs", "Haziran",
                         "Temmuz", "Ağustos", "Eylül", "Ekim", "Kasım", "Aralık"});
    m_ayCombo->setFixedWidth(100);

    m_tipCombo = new QComboBox(this);
    m_tipCombo->addItems({"Birikimli Hayat (%50)", "Sağlık/Vefat (%100)"});
    m_tipCombo->setFixedWidth(190);
    m_tipCombo->setCurrentIndex(1); // Varsayılan: Vefat

    m_primInput = new QLineEdit("100", this);
    m_primInput->setPlaceholderText("Prim");
    m_primInput->setValidator(new QDoubleValidator(0, 999999999, 2, this));

    m_removeBtn = new QPushButton(QString::fromUtf8("✕"), this);
    m_removeBtn->setFixedSize(34, 34);
    m_removeBtn->setObjectName("removeBtn");
    m_removeBtn->setToolTip("Poliçeyi Sil");
    connect(m_removeBtn, &QPushButton::clicked, this, [this]() {
        emit removed(this);
    });

    layout->addWidget(new QLabel("Başlangıç:", this));
    layout->addWidget(m_ayCombo);
    layout->addWidget(new QLabel("Tür:", this));
    layout->addWidget(m_tipCombo);
    layout->addWidget(new QLabel("Prim:", this));
    layout->addWidget(m_primInput);
    layout->addWidget(m_removeBtn);
}

InsuranceItemWidget::Data InsuranceItemWidget::getData() const {
    bool ok = false;
    double val = util::parse(m_primInput->text(), &ok);
    return {
        m_ayCombo->currentIndex(),
        m_tipCombo->currentIndex() == 0,
        ok ? val : 0.0
    };
}

TaxTab::TaxTab(const QJsonObject &settings, QWidget *parent)
    : QWidget(parent), m_settings(settings) {
    initUi();
}

void TaxTab::initUi() {
    int currentYear = util::currentYear();

    auto *mainLayout = new QVBoxLayout(this);
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *contentWidget = new QWidget(scroll);
    contentWidget->setObjectName("contentWidget");
    auto *layout = new QVBoxLayout(contentWidget);
    scroll->setWidget(contentWidget);
    mainLayout->addWidget(scroll);

    layout->setSpacing(12);

    auto *header = new QLabel(QString("🏛️ Gelir Vergisi & Sigorta İadesi Hesaplama (%1)").arg(currentYear), this);
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    auto *info = new QLabel(QString("%1 yılı gelir vergisi dilimleri ve hayat sigortası prim indirimi hesabı. "
                                    "Vergi dilimleri Hazine ve Maliye Bakanlığı tebliğine göre güncellenmiştir.").arg(currentYear), this);
    info->setObjectName("infoLabel");
    info->setWordWrap(true);
    layout->addWidget(info);

    // — Gelir Bilgileri —
    auto *incomeGroup = new QGroupBox("Gelir Bilgileri", this);
    auto *ig = new QGridLayout(incomeGroup);
    ig->setContentsMargins(10, 20, 10, 10);
    ig->setHorizontalSpacing(15);
    ig->setVerticalSpacing(20);

    ig->addWidget(new QLabel("Gelir Türü:", this), 0, 0);
    m_incomeType = new QComboBox(this);
    m_incomeType->addItems({"Ücret Geliri (maaşlı çalışan)", "Ücret Dışı Gelir (serbest meslek vb.)"});
    ig->addWidget(m_incomeType, 0, 1);

    ig->addWidget(new QLabel("Brüt Gelir (₺):", this), 1, 0);
    m_brutInput = new QLineEdit(this);
    m_brutInput->setPlaceholderText("Örn: 50000");
    m_brutInput->setValidator(new QDoubleValidator(0, 999999999999.0, 2, this));
    ig->addWidget(m_brutInput, 1, 1);

    ig->setRowStretch(2, 1);
    layout->addWidget(incomeGroup);

    // — Sigorta Primleri —
    m_insGroup = new QGroupBox("Sigorta Prim Bilgileri", this);

    auto *toggleLayout = new QHBoxLayout();
    toggleLayout->addStretch();
    m_toggleBtn = new QPushButton("➖ Sigorta Gizle/Daralt", this);
    m_toggleBtn->setObjectName("toggleBtn");
    m_toggleBtn->setFixedWidth(150);
    connect(m_toggleBtn, &QPushButton::clicked, this, &TaxTab::toggleInsurance);
    toggleLayout->addWidget(m_toggleBtn);
    layout->addLayout(toggleLayout);

    m_insGroup->setMinimumHeight(120);
    m_insContainer = new QWidget(m_insGroup);
    auto *insVLayout = new QVBoxLayout(m_insContainer);

    auto *topH = new QHBoxLayout();
    topH->addWidget(new QLabel("Para Birimi:", this));
    m_kurCombo = new QComboBox(this);
    m_kurCombo->addItems({"TRY (₺)", "USD ($)"});
    m_kurCombo->setCurrentIndex(1); // Varsayılan: USD
    connect(m_kurCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TaxTab::onCurrChanged);
    topH->addWidget(m_kurCombo);

    m_usdWidget = new QWidget(this);
    auto *usdLay = new QHBoxLayout(m_usdWidget);
    usdLay->setContentsMargins(0, 0, 0, 0);
    usdLay->addWidget(new QLabel("Kur:", this));
    m_usdInput = new QLineEdit("43.85", this);
    m_usdInput->setValidator(new QDoubleValidator(0, 999.0, 4, this));
    usdLay->addWidget(m_usdInput);
    topH->addWidget(m_usdWidget);

    topH->addStretch();
    m_addPoliBtn = new QPushButton("➕ Poliçe Ekle", this);
    m_addPoliBtn->setFixedWidth(135);
    connect(m_addPoliBtn, &QPushButton::clicked, this, &TaxTab::addPolicy);
    topH->addWidget(m_addPoliBtn);
    insVLayout->addLayout(topH);

    m_poliListLayout = new QVBoxLayout();
    insVLayout->addLayout(m_poliListLayout);

    addPolicy();

    auto *note = new QLabel("ℹ Seçilen aydan itibaren ödenen primler üzerinden net iadeler hesaplanır.\n"
                            "Üst sınır Yıllık Brüt Asgari Ücrettir.", this);
    note->setObjectName("infoLabel");
    note->setWordWrap(true);
    insVLayout->addWidget(note);

    auto *innerV = new QVBoxLayout(m_insGroup);
    innerV->setContentsMargins(10, 20, 10, 10);
    innerV->addWidget(m_insContainer);
    layout->addWidget(m_insGroup);

    auto *btnLayout = new QHBoxLayout();
    auto *calcBtn = new QPushButton("Hesapla", this);
    connect(calcBtn, &QPushButton::clicked, this, &TaxTab::calculate);
    btnLayout->addWidget(calcBtn);

    m_detailsBtn = new QPushButton("📋 Sonuç Detayları", this);
    m_detailsBtn->setObjectName("toggleBtn");
    m_detailsBtn->setVisible(false);
    m_detailsBtn->setFixedWidth(160);
    m_detailsBtn->setToolTip("Tüm hesaplama dökümünü yeni pencerede gör ve kopyala");
    connect(m_detailsBtn, &QPushButton::clicked, this, &TaxTab::showDetailsPopup);
    btnLayout->addWidget(m_detailsBtn);

    layout->addLayout(btnLayout);

    m_resultText = new QTextEdit(this);
    m_resultText->setReadOnly(true);
    m_resultText->setMinimumHeight(200);
    m_resultText->hide();
    layout->addWidget(m_resultText);
    layout->addStretch(1);
}

void TaxTab::addPolicy() {
    auto *w = new InsuranceItemWidget(this);
    connect(w, &InsuranceItemWidget::removed, this, &TaxTab::removePolicy);
    m_poliListLayout->addWidget(w);
    m_poliWidgets.push_back(w);
}

void TaxTab::removePolicy(InsuranceItemWidget *widget) {
    if (m_poliWidgets.size() > 1) {
        auto it = std::find(m_poliWidgets.begin(), m_poliWidgets.end(), widget);
        if (it != m_poliWidgets.end()) {
            m_poliWidgets.erase(it);
        }
        widget->deleteLater();
    } else {
        QMessageBox::warning(this, "Uyarı", "En az bir poliçe satırı bulunmalıdır.");
    }
}

void TaxTab::toggleInsurance() {
    bool isVis = m_insGroup->isVisible();
    m_insGroup->setVisible(!isVis);
    m_toggleBtn->setText(isVis ? "➕ Sigorta Ekle/Genişlet" : "➖ Sigorta Gizle/Daralt");
}

void TaxTab::onCurrChanged(int idx) {
    bool isUsd = (idx == 1);
    m_usdWidget->setVisible(isUsd);
}

static tax::Dilimler parseDilimler(const QJsonArray &arr) {
    tax::Dilimler res;
    for (const auto &v : arr) {
        if (v.isArray()) {
            QJsonArray pair = v.toArray();
            if (pair.size() >= 2) {
                res.push_back({pair[0].toDouble(), pair[1].toDouble()});
            }
        }
    }
    return res;
}

void TaxTab::calculate() {
    QJsonObject ts = m_settings.value("tax_settings").toObject();
    double asgariUcret = ts.value("asgari_ucret_yillik").toDouble(396360.0);

    tax::Dilimler ucretDilimleri = parseDilimler(ts.value("ucret_dilimleri").toArray());
    if (ucretDilimleri.empty()) {
        ucretDilimleri = {{190000.0, 0.15}, {400000.0, 0.20}, {1500000.0, 0.27},
                          {5300000.0, 0.35}, {9999999999.0, 0.40}};
    }
    tax::Dilimler ucretDisiDilimleri = parseDilimler(ts.value("ucret_disi_dilimleri").toArray());
    if (ucretDisiDilimleri.empty()) {
        ucretDisiDilimleri = {{190000.0, 0.15}, {400000.0, 0.20}, {1000000.0, 0.27},
                              {5300000.0, 0.35}, {9999999999.0, 0.40}};
    }

    bool ok = false;
    double brutAylik = util::parse(m_brutInput->text(), &ok);
    if (!ok || brutAylik <= 0) {
        brutAylik = asgariUcret / 12.0;
        m_brutInput->setText(QString::number(brutAylik, 'f', 2));
    }

    bool isUsd = (m_kurCombo->currentIndex() == 1);
    double usdKuru = isUsd ? util::parse(m_usdInput->text(), &ok) : 1.0;
    if (isUsd && (!ok || usdKuru <= 0)) usdKuru = 1.0;

    std::vector<double> poliAylikMatrahlar(12, 0.0);
    double toplamIndirilecekYillik = 0.0;

    QStringList policyTypeNames;
    for (auto *w : m_poliWidgets) {
        auto data = w->getData();
        int ayIdx = data.ayIdx;
        double primTry = data.prim * usdKuru;
        double oran = data.isHayat ? 0.5 : 1.0;
        double aylikIndirilecek = primTry * oran;

        for (int i = ayIdx; i < 12; ++i) {
            poliAylikMatrahlar[i] += aylikIndirilecek;
        }
        toplamIndirilecekYillik += aylikIndirilecek * (12 - ayIdx);

        QString tName = data.isHayat ? "Birikimli Hayat" : "Sağlık/Vefat";
        if (!policyTypeNames.contains(tName)) policyTypeNames << tName;
    }

    double brut = brutAylik * 12.0;
    bool isUcret = (m_incomeType->currentIndex() == 0);
    const auto &dilimler = isUcret ? ucretDilimleri : ucretDisiDilimleri;
    QString gelirTuru = isUcret ? "Ücret Geliri" : "Ücret Dışı Gelir";

    double yillikMatrah = brut;
    if (isUcret) {
        double tavanAylik = (asgariUcret / 12.0) * 7.5;
        double primeEsasAylik = std::min(brutAylik, tavanAylik);
        double sgkIssizlik = primeEsasAylik * 0.15;
        yillikMatrah = (brutAylik - sgkIssizlik) * 12.0;
    }

    auto oncesi = tax::hesaplaVergi(yillikMatrah, dilimler);
    double vergiOncesi = oncesi.toplam;

    double ustSinirGelir = brut * 0.15;
    double ustSinirAsgari = asgariUcret;
    double ustSinir = std::min(ustSinirGelir, ustSinirAsgari);

    double indirimUygulanacakYillik = std::min(toplamIndirilecekYillik, ustSinir);
    double matrahSonrasi = std::max(yillikMatrah - indirimUygulanacakYillik, 0.0);
    auto sonrasi = tax::hesaplaVergi(matrahSonrasi, dilimler);
    double vergiSonrasi = sonrasi.toplam;

    double vergiIadesi = vergiOncesi - vergiSonrasi;
    double kalanLimitYillik = indirimUygulanacakYillik;
    double marjinal = tax::marjinalVergiOrani(yillikMatrah, dilimler);

    QStringList lines;
    lines << QString("=").repeated(65);
    lines << QString("  VERGİ & SİGORTA İADESİ HESAPLAMA RAPORU (%1)").arg(util::currentYear());
    lines << QString("=").repeated(65);
    lines << "";
    lines << QString("  Gelir Türü       : %1").arg(gelirTuru);
    lines << QString("  Aylık Brüt Gelir : %1 ₺").arg(util::num(brutAylik, 2), 15);
    if (isUcret) {
        lines << QString("  Ayl. Vergi Matrahı: %1 ₺").arg(util::num(yillikMatrah / 12.0, 2), 14);
    }
    lines << QString("  Marjinal Dilim   : %%1").arg(qRound(marjinal * 100));
    lines << "";

    lines << "─── YILLIK VERGİ DİLİMLERİ (İndirim Öncesi) ───";
    for (const auto &d : oncesi.detaylar) {
        QString ustStr = d.ust < 1e11 ? util::num(d.ust, 0) : "∞";
        lines << QString("  %1 – %2 ₺ %%%3 → Vergi: %4 ₺")
                     .arg(util::num(d.alt, 0), 10)
                     .arg(ustStr, 10)
                     .arg(qRound(d.oran * 100), 2)
                     .arg(util::num(d.vergi, 2), 10);
    }
    lines << QString("  %1 TOPLAM: %2 ₺").arg("", 30).arg(util::num(vergiOncesi, 2), 10);
    lines << "";

    QJsonArray aylikIadeArr;
    QStringList aylar = {"Ocak", "Şubat", "Mart", "Nisan", "Mayıs", "Haziran",
                         "Temmuz", "Ağustos", "Eylül", "Ekim", "Kasım", "Aralık"};

    if (toplamIndirilecekYillik > 0) {
        lines << "─── SİGORTA PRİM İNDİRİMİ ───";
        if (isUsd) {
            lines << QString("  Para Birimi: USD $ (Kur: %1)").arg(util::num(usdKuru, 2));
        }
        lines << QString("  Yıllık Toplam İndirilebilir: %1 ₺").arg(util::num(toplamIndirilecekYillik, 2), 12);
        lines << QString("  Yıllık Uygulanan İndirim   : %1 ₺").arg(util::num(indirimUygulanacakYillik, 2), 12);
        lines << "";
        lines << "─── AYLARA GÖRE BORDROYA YANSIYAN İADE (NET MAAŞA ETKİSİ) ───";

        double kumMEski = 0;
        for (int ay = 1; ay <= 12; ++ay) {
            double primeEsas = isUcret ? std::min(brutAylik, (asgariUcret / 12.0) * 7.5) : 0;
            double sgkVIssizlik = isUcret ? primeEsas * 0.15 : 0;
            double matrahAylik = brutAylik - sgkVIssizlik;

            double ayP = poliAylikMatrahlar[ay - 1];
            double uygAylik = std::min(ayP, kalanLimitYillik);
            kalanLimitYillik -= uygAylik;

            double yeniKumEski = kumMEski + matrahAylik;
            double aylikOran = tax::marjinalVergiOrani(yeniKumEski, dilimler);
            double aylikIade = uygAylik * aylikOran;

            QJsonObject item;
            item["iade"] = aylikIade;
            item["oran"] = aylikOran;
            aylikIadeArr.append(item);

            kumMEski += matrahAylik;
        }

        auto addMonthlyTable = [&](const std::vector<int> &sub) {
            QString top = "  ";
            for (size_t i = 0; i < sub.size(); ++i) top += "┌───────────────";
            top += "┐";
            lines << top;

            QString names = "  ";
            for (int idx : sub) {
                QString n = QString(" %1 ").arg(aylar[idx]);
                names += QString("│%1").arg(n, -15);
            }
            names += "│";
            lines << names;

            QString mid = "  ";
            for (size_t i = 0; i < sub.size(); ++i) mid += "├───────────────";
            mid += "┤";
            lines << mid;

            QString vals = "  ";
            for (int idx : sub) {
                QJsonObject item = aylikIadeArr[idx].toObject();
                double iade = item["iade"].toDouble();
                double oran = item["oran"].toDouble();
                QString vStr = QString(" %1 ₺ (%%%2) ").arg(util::num(iade, 0)).arg(qRound(oran * 100));
                vals += QString("│%1").arg(vStr, -15);
            }
            vals += "│";
            lines << vals;

            QString bot = "  ";
            for (size_t i = 0; i < sub.size(); ++i) bot += "└───────────────";
            bot += "┘";
            lines << bot;
        };

        addMonthlyTable({0, 1, 2, 3});
        lines << "";
        addMonthlyTable({4, 5, 6, 7});
        lines << "";
        addMonthlyTable({8, 9, 10, 11});
        lines << "";
    }

    QJsonObject resData;
    resData["toplam_prim"] = toplamIndirilecekYillik;
    resData["vergi_iadesi"] = vergiIadesi;
    resData["matrah_dusulen"] = indirimUygulanacakYillik;
    resData["brut_aylik"] = brutAylik;
    resData["aylik_iade_listesi"] = aylikIadeArr;

    QJsonArray aylarArr;
    for (const auto &a : aylar) aylarArr.append(a);
    resData["ay_isimleri"] = aylarArr;
    resData["sigorta_turu"] = policyTypeNames.join(" / ");
    resData["vergi_dilimi"] = QString("%%%1").arg(qRound(marjinal * 100));
    resData["son_hesaplama"] = QDate::currentDate().toString("dd/MM/yyyy");
    resData["yil"] = util::currentYear();

    m_lastResultData = resData;
    m_lastFullResult = lines.join("\n");

    m_resultText->setVisible(true);
    m_resultText->setPlainText(m_lastFullResult);
    m_detailsBtn->setVisible(true);

    showDetailsPopup();
}

void TaxTab::showDetailsPopup() {
    if (!m_lastResultData.isEmpty()) {
        ResultDetailsDialog dlg(this, m_lastResultData);
        dlg.exec();
    }
}
