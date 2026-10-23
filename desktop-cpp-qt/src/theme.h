#pragma once
#include <QMap>
#include <QPalette>
#include <QString>
#include <QStringList>

using Theme = QMap<QString, QString>;

namespace theme {
// Temalar Python sürümündeki sırayla
const QStringList &names();
Theme get(const QString &name);  // bulunamazsa boş
QString generateStyle(const Theme &t);
QPalette palette(const Theme &t);
}
