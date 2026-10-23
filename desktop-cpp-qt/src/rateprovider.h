#pragma once
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <memory>

class QNetworkAccessManager;

using Rates = QMap<QString, double>;
using CurrencyNames = QMap<QString, QString>;

// Kur sağlayıcı soyut temel sınıf. Yeni kaynak eklemek için alt-sınıflayın.
class RateProvider : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    ~RateProvider() override = default;
    // Asenkron: sonuç ratesFetched / error sinyalleriyle gelir
    virtual void fetchRates(const QString &baseCurrency) = 0;
    virtual CurrencyNames currencies() const = 0;
signals:
    void ratesFetched(const QString &base, const Rates &rates);
    void error(const QString &message);
};

// ExchangeRate API – ücretsiz, API key gereksiz
class ExchangeRateProvider : public RateProvider {
    Q_OBJECT
public:
    explicit ExchangeRateProvider(const QString &baseUrl = "https://open.er-api.com/v6",
                                  QObject *parent = nullptr);
    void fetchRates(const QString &baseCurrency) override;
    CurrencyNames currencies() const override;
private:
    QString m_baseUrl;
    QNetworkAccessManager *m_nam;
};

const CurrencyNames &currencyNames();

// Sağlayıcı fabrikası
RateProvider *createProvider(const QJsonObject &settings, QObject *parent);
