// Python calculators.pyw referans çıktılarıyla karşılaştırma testi
#include "compound.h"
#include "taxcalc.h"
#include <cmath>
#include <cstdio>

static int failures = 0;
static void check(const char *name, double got, double want, double eps = 1e-6) {
    bool ok = std::fabs(got - want) <= eps * std::max(1.0, std::fabs(want));
    std::printf("[%s] %-32s got=%.6f want=%.6f\n", ok ? " OK " : "FAIL", name, got, want);
    if (!ok) ++failures;
}

int main() {
    const tax::Dilimler U = {{190000, .15}, {400000, .20}, {1500000, .27},
                             {5300000, .35}, {9999999999.0, .40}};
    check("vergi(0)", tax::hesaplaVergi(0, U).toplam, 0);
    check("vergi(150000)", tax::hesaplaVergi(150000, U).toplam, 22500.0);
    check("vergi(396360)", tax::hesaplaVergi(396360, U).toplam, 69772.0);
    check("vergi(1000000)", tax::hesaplaVergi(1000000, U).toplam, 232500.0);
    check("vergi(7000000)", tax::hesaplaVergi(7000000, U).toplam, 2377500.0);
    check("marjinal(396360)", tax::marjinalVergiOrani(396360, U), 0.20);
    check("marjinal(7000000)", tax::marjinalVergiOrani(7000000, U), 0.40);
    check("fv(0,5000,10y,10%,gunluk)", compound::futureValue(0, 5000, 10, 10, 365).back().balance,
          1035187.6623718548);
    check("fv(1000,0,5y,0%)", compound::futureValue(1000, 0, 5, 0, 12).back().balance, 1000);
    std::printf(failures ? "\n%d test BASARISIZ\n" : "\nTum testler gecti\n", failures);
    return failures ? 1 : 0;
}
