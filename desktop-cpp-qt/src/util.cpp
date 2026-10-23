#include "util.h"
#include <QCoreApplication>
#include <QDate>
#include <QLocale>
#include <cmath>

namespace util {

static const QLocale &en() {
    static QLocale l(QLocale::English, QLocale::UnitedStates);
    return l;
}

QString num(double v, int decimals) { return en().toString(v, 'f', decimals); }

QString gnum(double v, int precision) {
    // %g benzeri: büyük sayılarda bilimsel gösterime düşmeden gruplu yaz
    if (v != 0 && std::fabs(v) < 1e15) {
        int digits = int(std::floor(std::log10(std::fabs(v)))) + 1;
        if (digits >= precision) return en().toString(std::round(v), 'f', 0);
        int dec = precision - digits;
        QString s = en().toString(v, 'f', dec);
        if (s.contains('.')) {
            while (s.endsWith('0')) s.chop(1);
            if (s.endsWith('.')) s.chop(1);
        }
        return s;
    }
    return en().toString(v, 'g', precision);
}

QString g(double v) {
    QString s = QString::number(v, 'g', 6);
    return s;
}

double parse(const QString &text, bool *ok) {
    QString t = text;
    t.replace(',', '.').remove(' ').remove(QChar(0x20BA)).remove('$');
    bool good = false;
    double v = t.toDouble(&good);
    if (ok) *ok = good;
    return good ? v : 0.0;
}

QString appDir() { return QCoreApplication::applicationDirPath(); }
QString settingsFile() { return appDir() + "/settings.json"; }
QString cacheFile() { return appDir() + "/rates_cache.json"; }
int currentYear() { return QDate::currentDate().year(); }

}
