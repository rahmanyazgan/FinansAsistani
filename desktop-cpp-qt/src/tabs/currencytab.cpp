#include "currencytab.h"
#include "util.h"
#include <QComboBox>
#include <QDateTime>
#include <QDoubleValidator>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

static qint64 nowSecs() { return QDateTime::currentSecsSinceEpoch(); }

CurrencyTab::CurrencyTab(const QJsonObject &settings, QWidget *parent)
    : QWidget(parent), m_settings(settings) {
    m_provider = createProvider(settings, this);
    m_cacheTtl = settings.value("cache_ttl_seconds").toInt(600);
    connect(m_provider, &RateProvider::ratesFetched, this, &CurrencyTab::onRatesFetched);
    connect(m_provider, &RateProvider::error, this, &CurrencyTab::onRatesError);
    initUi();
    loadCurrencies();
}

void CurrencyTab::initUi() {
    auto *mainLayout = new QVBoxLayout(this);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    content->setObjectName("contentWidget");
    auto *layout = new QVBoxLayout(content);
    scroll->setWidget(content);
    mainLayout->addWidget(scroll);
    layout->setSpacing(14);

    auto *header = new QLabel("💱 Döviz Kuru Dönüştürücü");
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    auto *info = new QLabel("Kurlar ExchangeRate API üzerinden anlık olarak alınır. "
                            "Ayarlar settings.json dosyasından yapılandırılabilir.");
    info->setObjectName("infoLabel");
    info->setWordWrap(true);
    layout->addWidget(info);

    auto *group = new QGroupBox("Dönüşüm");
    auto *grid = new QGridLayout;
    grid->setSpacing(10);

    grid->addWidget(new QLabel("Miktar:"), 0, 0);
    m_amount = new QLineEdit("1");
    m_amount->setValidator(new QDoubleValidator(0, 999999999999, 6, m_amount));
    m_amount->setPlaceholderText("Miktar girin...");
    grid->addWidget(m_amount, 0, 1);

    grid->addWidget(new QLabel("Kaynak:"), 1, 0);
    m_from = new QComboBox;
    grid->addWidget(m_from, 1, 1);

    grid->addWidget(new QLabel("Hedef:"), 2, 0);
    m_to = new QComboBox;
    grid->addWidget(m_to, 2, 1);

    auto *swapBtn = new QPushButton("⇅ Değiştir");
    swapBtn->setFixedWidth(120);
    connect(swapBtn, &QPushButton::clicked, this, &CurrencyTab::swapCurrencies);
    grid->addWidget(swapBtn, 1, 2, 2, 1);

    m_convertBtn = new QPushButton("Dönüştür");
    connect(m_convertBtn, &QPushButton::clicked, this, &CurrencyTab::convert);
    grid->addWidget(m_convertBtn, 3, 0, 1, 3);

    group->setLayout(grid);
    layout->addWidget(group);

    m_result = new QLabel;
    m_result->setObjectName("resultLabel");
    m_result->setMinimumHeight(60);
    m_result->hide();
    layout->addWidget(m_result);

    m_update = new QLabel;
    m_update->setObjectName("infoLabel");
    m_update->setAlignment(Qt::AlignRight);
    layout->addWidget(m_update);

    layout->addStretch();
}

void CurrencyTab::loadCurrencies() { onCurrenciesLoaded(m_provider->currencies()); }

void CurrencyTab::onCurrenciesLoaded(const CurrencyNames &currencies) {
    m_from->clear();
    m_to->clear();
    const QStringList priority = {"TRY", "USD", "EUR", "GBP"};
    QStringList ordered;
    for (const auto &c : priority)
        if (currencies.contains(c)) ordered << c;
    for (const auto &c : currencies.keys())  // QMap anahtarları sıralıdır
        if (!priority.contains(c)) ordered << c;

    for (const auto &code : ordered) {
        const QString label = code + " – " + currencies.value(code);
        m_from->addItem(label, code);
        m_to->addItem(label, code);
    }
    int iUsd = ordered.indexOf("USD"), iTry = ordered.indexOf("TRY");
    m_from->setCurrentIndex(iUsd >= 0 ? iUsd : 0);
    m_to->setCurrentIndex(iTry >= 0 ? iTry : 1);
}

void CurrencyTab::swapCurrencies() {
    int i = m_from->currentIndex(), j = m_to->currentIndex();
    m_from->setCurrentIndex(j);
    m_to->setCurrentIndex(i);
}

void CurrencyTab::convert() {
    bool ok = false;
    double amount = util::parse(m_amount->text(), &ok);
    if (!ok) {
        m_result->setVisible(true);
        m_result->setText("⚠ Geçerli bir miktar girin");
        return;
    }
    const QString base = m_from->currentData().toString();
    const QString target = m_to->currentData().toString();
    if (base.isEmpty() || target.isEmpty()) return;

    if (base == target) {
        m_result->setVisible(true);
        m_result->setText(QString("%1 %2 = %1 %3").arg(util::gnum(amount, 6), base, target));
        return;
    }
    if (m_ratesCache.contains(base) && (nowSecs() - m_cacheTime) < m_cacheTtl) {
        doConvert(amount, base, target, m_ratesCache[base]);
        return;
    }
    m_convertBtn->setEnabled(false);
    m_convertBtn->setText("Yükleniyor...");
    m_amountPending = amount;
    m_basePending = base;
    m_targetPending = target;
    m_provider->fetchRates(base);
}

void CurrencyTab::resetButton() {
    m_convertBtn->setEnabled(true);
    m_convertBtn->setText("Dönüştür");
}

void CurrencyTab::onRatesFetched(const QString &base, const Rates &rates) {
    m_ratesCache[base] = rates;
    m_cacheTime = nowSecs();
    doConvert(m_amountPending, m_basePending, m_targetPending, rates);
    resetButton();
    m_update->setText("✓ Kurlar güncellendi: " + QTime::currentTime().toString("HH:mm:ss"));

    // Diske kaydet (Python sürümüyle aynı format)
    QJsonObject cache;
    QFile f(util::cacheFile());
    if (f.open(QIODevice::ReadOnly)) {
        cache = QJsonDocument::fromJson(f.readAll()).object();
        f.close();
    }
    QJsonObject r;
    for (auto it = rates.begin(); it != rates.end(); ++it) r.insert(it.key(), it.value());
    cache.insert(base, QJsonObject{{"rates", r}, {"timestamp", double(m_cacheTime)}});
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        f.write(QJsonDocument(cache).toJson(QJsonDocument::Compact));
}

void CurrencyTab::onRatesError(const QString &err) {
    // İnternet yoksa diske bak
    QFile f(util::cacheFile());
    if (f.open(QIODevice::ReadOnly)) {
        const QJsonObject cache = QJsonDocument::fromJson(f.readAll()).object();
        if (cache.contains(m_basePending)) {
            const QJsonObject stored = cache.value(m_basePending).toObject();
            Rates rates;
            const QJsonObject r = stored.value("rates").toObject();
            for (auto it = r.begin(); it != r.end(); ++it) rates.insert(it.key(), it.value().toDouble());
            const auto ts = qint64(stored.value("timestamp").toDouble());
            const QString dt = QDateTime::fromSecsSinceEpoch(ts).toString("dd.MM.yyyy HH:mm");
            m_ratesCache[m_basePending] = rates;
            doConvert(m_amountPending, m_basePending, m_targetPending, rates);
            resetButton();
            m_update->setText(QString("⚠ Çevrimdışı (Son güncel: %1)").arg(dt));
            return;
        }
    }
    m_result->setVisible(true);
    m_result->setText("⚠ Hata (İnternet Yok): " + err);
    resetButton();
}

void CurrencyTab::doConvert(double amount, const QString &base, const QString &target,
                            const Rates &rates) {
    m_result->setVisible(true);
    if (!rates.contains(target)) {
        m_result->setText(QString("⚠ %1 kuru bulunamadı").arg(target));
        return;
    }
    const double rate = rates.value(target);
    const double result = amount * rate;
    m_result->setText(QString("%1 %2  =  %3 %4\n1 %2 = %5 %4")
                          .arg(util::num(amount), base, util::num(result), target, util::num(rate, 6)));
}
