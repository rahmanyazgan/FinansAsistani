#include "compound.h"
#include <cmath>

namespace compound {

std::vector<YearRow> futureValue(double principal, double monthly, int years,
                                 double annualRatePct, int n) {
    const double r = annualRatePct / 100.0;
    // Efektif aylık faiz oranı (nominal n-dönemli faizden türetilir)
    const double rMo = r > 0 ? std::pow(1 + r / n, double(n) / 12.0) - 1 : 0.0;
    double balance = principal, cum = principal;
    std::vector<YearRow> out;
    for (int yr = 1; yr <= years; ++yr) {
        for (int m = 0; m < 12; ++m) {
            balance += monthly;
            cum += monthly;
            balance *= (1 + rMo);
        }
        out.push_back({yr, balance, cum, balance - cum});
    }
    return out;
}

}
