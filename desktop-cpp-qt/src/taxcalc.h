#pragma once
#include <utility>
#include <vector>

namespace tax {

// (üst sınır, oran)
using Dilim = std::pair<double, double>;
using Dilimler = std::vector<Dilim>;

struct DilimDetay {
    double alt, ust, oran, matrah, vergi;
};

struct VergiSonuc {
    double toplam = 0;
    std::vector<DilimDetay> detaylar;
};

// Artan oranlı gelir vergisi hesapla.
VergiSonuc hesaplaVergi(double yillikBrut, const Dilimler &dilimler);
// Kişinin bulunduğu en üst vergi dilim oranı.
double marjinalVergiOrani(double yillikBrut, const Dilimler &dilimler);

}
