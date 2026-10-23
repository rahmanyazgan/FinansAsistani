#pragma once
#include <QString>

class QLineEdit;

namespace util {
// Python "{x:,.Nf}" eşdeğeri: 1,234.56
QString num(double v, int decimals = 2);
// Python "{x:,.Ng}" eşdeğeri
QString gnum(double v, int precision = 4);
// Python "{x:g}" eşdeğeri
QString g(double v);
// "1,5" / "1 000" gibi girdileri ayrıştırır. ok=false ise geçersiz.
double parse(const QString &text, bool *ok = nullptr);
// Uygulama dizini (exe'nin yanı)
QString appDir();
QString settingsFile();
QString cacheFile();
int currentYear();
}
