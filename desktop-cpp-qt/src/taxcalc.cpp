#include "taxcalc.h"
#include <algorithm>

namespace tax {

VergiSonuc hesaplaVergi(double yillikBrut, const Dilimler &dilimler) {
    VergiSonuc s;
    double kalan = yillikBrut;
    double altSinir = 0;
    for (const auto &[ustSinir, oran] : dilimler) {
        if (kalan <= 0) break;
        double genislik = ustSinir - altSinir;
        double vergilenecek = std::min(kalan, genislik);
        double dv = vergilenecek * oran;
        s.toplam += dv;
        s.detaylar.push_back({altSinir, std::min(ustSinir, yillikBrut), oran, vergilenecek, dv});
        kalan -= vergilenecek;
        altSinir = ustSinir;
    }
    return s;
}

double marjinalVergiOrani(double yillikBrut, const Dilimler &dilimler) {
    for (const auto &[ust, oran] : dilimler)
        if (yillikBrut <= ust) return oran;
    return dilimler.empty() ? 0.0 : dilimler.back().second;
}

}
