#pragma once
#include <QJsonObject>

namespace settings {
// settings.json'u yükle; yoksa/bozuksa varsayılanla oluştur. Eksik anahtarları tamamlar.
QJsonObject load();
void save(const QJsonObject &s);
QJsonObject defaults();
}
