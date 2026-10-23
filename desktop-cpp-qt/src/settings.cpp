#include "settings.h"
#include "util.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

namespace settings {

static QJsonArray dilim(std::initializer_list<std::pair<double, double>> v) {
    QJsonArray a;
    for (auto &p : v) a.append(QJsonArray{p.first, p.second});
    return a;
}

QJsonObject defaults() {
    QJsonObject provider{{"name", "ExchangeRate API (Ücretsiz)"},
                         {"base_url", "https://open.er-api.com/v6"},
                         {"api_key", ""}};
    QJsonObject tax{
        {"asgari_ucret_yillik", 396360.0},
        {"ucret_dilimleri", dilim({{190000.0, 0.15}, {400000.0, 0.20}, {1500000.0, 0.27},
                                   {5300000.0, 0.35}, {9999999999.0, 0.40}})},
        {"ucret_disi_dilimleri", dilim({{190000.0, 0.15}, {400000.0, 0.20}, {1000000.0, 0.27},
                                        {5300000.0, 0.35}, {9999999999.0, 0.40}})}};
    return QJsonObject{{"active_provider", "exchangerate"},
                       {"providers", QJsonObject{{"exchangerate", provider}}},
                       {"cache_ttl_seconds", 600},
                       {"tax_settings", tax}};
}

QJsonObject load() {
    const QJsonObject def = defaults();
    QFile f(util::settingsFile());
    if (f.open(QIODevice::ReadOnly)) {
        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(f.readAll(), &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject data = doc.object();
            for (auto it = def.begin(); it != def.end(); ++it)
                if (!data.contains(it.key())) data.insert(it.key(), it.value());
            return data;
        }
    }
    save(def);
    return def;
}

void save(const QJsonObject &s) {
    QFile f(util::settingsFile());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        f.write(QJsonDocument(s).toJson(QJsonDocument::Indented));
}

}
