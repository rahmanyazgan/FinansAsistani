#pragma once
#include <vector>

namespace compound {

struct YearRow {
    int year;
    double balance, contributed, interest;
};

// Yıl-yıl (bakiye, toplam katkı, toplam kar) hesaplar.
std::vector<YearRow> futureValue(double principal, double monthly, int years,
                                 double annualRatePct, int n);

}
