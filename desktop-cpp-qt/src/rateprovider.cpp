#include "rateprovider.h"
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

const CurrencyNames &currencyNames() {
    static const CurrencyNames m = {
        {"TRY", "Türk Lirası"}, {"USD", "ABD Doları"}, {"EUR", "Euro"},
        {"GBP", "İngiliz Sterlini"}, {"JPY", "Japon Yeni"}, {"CHF", "İsviçre Frangı"},
        {"CAD", "Kanada Doları"}, {"AUD", "Avustralya Doları"}, {"CNY", "Çin Yuanı"},
        {"RUB", "Rus Rublesi"}, {"KRW", "Güney Kore Wonu"}, {"INR", "Hindistan Rupisi"},
        {"BRL", "Brezilya Reali"}, {"MXN", "Meksika Pesosu"}, {"ZAR", "Güney Afrika Randı"},
        {"SEK", "İsveç Kronu"}, {"NOK", "Norveç Kronu"}, {"DKK", "Danimarka Kronu"},
        {"PLN", "Polonya Zlotisi"}, {"CZK", "Çek Korunası"}, {"HUF", "Macar Forinti"},
        {"RON", "Romen Leyi"}, {"BGN", "Bulgar Levası"}, {"HRK", "Hırvat Kunası"},
        {"SAR", "Suudi Riyali"}, {"AED", "BAE Dirhemi"}, {"QAR", "Katar Riyali"},
        {"KWD", "Kuveyt Dinarı"}, {"EGP", "Mısır Lirası"}, {"AZN", "Azerbaycan Manatı"},
        {"GEL", "Gürcistan Larisi"}, {"SGD", "Singapur Doları"}, {"HKD", "Hong Kong Doları"},
        {"TWD", "Tayvan Doları"}, {"THB", "Tayland Bahtı"}, {"MYR", "Malezya Ringgiti"},
        {"IDR", "Endonezya Rupisi"}, {"PHP", "Filipin Pesosu"}, {"NZD", "Yeni Zelanda Doları"},
        {"ILS", "İsrail Şekeli"}, {"PKR", "Pakistan Rupisi"}, {"NGN", "Nijerya Nairası"},
        {"ARS", "Arjantin Pesosu"}, {"CLP", "Şili Pesosu"}, {"COP", "Kolombiya Pesosu"},
        {"PEN", "Peru Solü"}, {"UAH", "Ukrayna Grivnası"},
    };
    return m;
}

ExchangeRateProvider::ExchangeRateProvider(const QString &baseUrl, QObject *parent)
    : RateProvider(parent), m_baseUrl(baseUrl), m_nam(new QNetworkAccessManager(this)) {
    while (m_baseUrl.endsWith('/')) m_baseUrl.chop(1);
}

void ExchangeRateProvider::fetchRates(const QString &base) {
    QNetworkRequest req(QUrl(m_baseUrl + "/latest/" + base));
    req.setTransferTimeout(15000);
    QNetworkReply *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, base]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit error(reply->errorString());
            return;
        }
        const QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
        if (data.value("result").toString() != "success") {
            emit error(data.value("error-type").toString("Bilinmeyen hata"));
            return;
        }
        Rates rates;
        const QJsonObject r = data.value("rates").toObject();
        for (auto it = r.begin(); it != r.end(); ++it) rates.insert(it.key(), it.value().toDouble());
        emit ratesFetched(base, rates);
    });
}

CurrencyNames ExchangeRateProvider::currencies() const { return currencyNames(); }

RateProvider *createProvider(const QJsonObject &settings, QObject *parent) {
    const QString name = settings.value("active_provider").toString("exchangerate");
    const QJsonObject cfg = settings.value("providers").toObject().value(name).toObject();
    if (name == "exchangerate")
        return new ExchangeRateProvider(cfg.value("base_url").toString("https://open.er-api.com/v6"), parent);
    // Gelecekte başka sağlayıcılar buraya eklenir
    return new ExchangeRateProvider("https://open.er-api.com/v6", parent);
}
