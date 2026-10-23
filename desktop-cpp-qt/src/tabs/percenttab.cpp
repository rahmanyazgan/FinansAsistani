#include "percenttab.h"
#include "util.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <cmath>

PercentTab::PercentTab(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    content->setObjectName("contentWidget");
    auto *layout = new QVBoxLayout(content);
    scroll->setWidget(content);
    mainLayout->addWidget(scroll);
    layout->setSpacing(12);

    auto *header = new QLabel("📊 Yüzde Hesaplayıcı");
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    auto *modeGroup = new QGroupBox("Hesaplama Modu");
    auto *modeLayout = new QVBoxLayout;
    m_modes = new QButtonGroup(this);
    const QList<QPair<QString, QString>> modes = {
        {"Bir sayının %X'i kaçtır?", "X sayısının %Y'si = ?"},
        {"X, Y'nin yüzde kaçıdır?", "X değeri, Y değerinin yüzde kaçı?"},
        {"Yüzde değişim (artış/azalış)", "Eski → Yeni, % kaç değişim?"},
        {"İki değer arası yüzdelik fark", "A ile B arasındaki % fark"},
        {"KDV Hesaplama", "KDV dahil ↔ hariç dönüşüm"},
    };
    for (int i = 0; i < modes.size(); ++i) {
        auto *rb = new QRadioButton(QString("%1   (%2)").arg(modes[i].first, modes[i].second));
        rb->setStyleSheet("font-size: 12px;");
        m_modes->addButton(rb, i);
        modeLayout->addWidget(rb);
    }
    m_modes->button(0)->setChecked(true);
    connect(m_modes, &QButtonGroup::idClicked, this, &PercentTab::onModeChanged);
    modeGroup->setLayout(modeLayout);
    layout->addWidget(modeGroup);

    auto *inputGroup = new QGroupBox("Değerler");
    auto *grid = new QGridLayout;
    grid->setSpacing(10);

    m_lblA = new QLabel("Sayı (X):");
    m_a = new QLineEdit;
    m_a->setPlaceholderText("Değer girin...");
    m_a->setValidator(new QDoubleValidator(m_a));
    grid->addWidget(m_lblA, 0, 0);
    grid->addWidget(m_a, 0, 1);

    m_lblB = new QLabel("Yüzde (%):");
    m_b = new QLineEdit;
    m_b->setPlaceholderText("Değer girin...");
    m_b->setValidator(new QDoubleValidator(m_b));
    grid->addWidget(m_lblB, 1, 0);
    grid->addWidget(m_b, 1, 1);

    m_lblKdv = new QLabel("KDV Oranı (%):");
    m_kdvRate = new QComboBox;
    m_kdvRate->addItems({"1", "10", "20"});
    m_kdvRate->setCurrentIndex(2);
    grid->addWidget(m_lblKdv, 2, 0);
    grid->addWidget(m_kdvRate, 2, 1);

    m_kdvDir = new QComboBox;
    m_kdvDir->addItems({"KDV Hariç → KDV Dahil", "KDV Dahil → KDV Hariç"});
    grid->addWidget(m_kdvDir, 3, 0, 1, 2);

    inputGroup->setLayout(grid);
    layout->addWidget(inputGroup);

    auto *calcBtn = new QPushButton("Hesapla");
    connect(calcBtn, &QPushButton::clicked, this, &PercentTab::calculate);
    layout->addWidget(calcBtn);

    m_result = new QLabel;
    m_result->setObjectName("resultLabel");
    m_result->setMinimumHeight(60);
    m_result->setWordWrap(true);
    layout->addWidget(m_result);

    m_detail = new QLabel;
    m_detail->setObjectName("infoLabel");
    m_detail->setWordWrap(true);
    layout->addWidget(m_detail);

    layout->addStretch();
    onModeChanged(0);
}

void PercentTab::onModeChanged(int mode) {
    static const QStringList la = {"Sayı (X):", "Parça (X):", "Eski Değer:", "Değer A:", "Tutar:"};
    static const QStringList lb = {"Yüzde (%):", "Bütün (Y):", "Yeni Değer:", "Değer B:", "—"};
    m_lblA->setText(la[mode]);
    m_lblB->setText(lb[mode]);
    const bool kdv = mode == 4;
    m_lblB->setVisible(!kdv);
    m_b->setVisible(!kdv);
    m_lblKdv->setVisible(kdv);
    m_kdvRate->setVisible(kdv);
    m_kdvDir->setVisible(kdv);
    m_a->clear();
    m_b->clear();
    m_result->hide();
    m_detail->hide();
    m_result->clear();
    m_detail->clear();
}

void PercentTab::showResult(const QString &result, const QString &detail) {
    m_result->setVisible(true);
    m_detail->setVisible(true);
    m_result->setText(result);
    m_detail->setText(detail);
}

void PercentTab::showError(const QString &msg) {
    m_result->setVisible(true);
    m_detail->hide();
    m_result->setText(msg);
}

void PercentTab::calculate() {
    using util::g;
    using util::gnum;
    const int mode = m_modes->checkedId();
    bool ok = false;
    const double a = util::parse(m_a->text(), &ok);
    if (!ok) return showError("⚠ Geçerli bir sayı girin (ilk kutu)");
    double b = 0;
    if (mode != 4) {
        b = util::parse(m_b->text(), &ok);
        if (!ok) return showError("⚠ Geçerli bir sayı girin (ikinci kutu)");
    }

    switch (mode) {
    case 0: {
        const double r = a * b / 100;
        showResult(QString("%1 sayısının %%2'%3i = %4").arg(gnum(a), g(b), b != 1 ? "s" : "", gnum(r)),
                   QString("Formül: %1 × %2 / 100 = %3").arg(g(a), g(b), gnum(r, 6)));
        break;
    }
    case 1: {
        if (b == 0) return showError("⚠ Bütün (Y) değeri sıfır olamaz");
        const double pct = a / b * 100;
        showResult(QString("%1,  %2 değerinin %%3'%4idir").arg(gnum(a), gnum(b), gnum(pct), pct != 1 ? "s" : ""),
                   QString("Formül: (%1 / %2) × 100 = %%3").arg(g(a), g(b), gnum(pct, 6)));
        break;
    }
    case 2: {
        if (a == 0) return showError("⚠ Eski değer sıfır olamaz");
        const double change = (b - a) / std::fabs(a) * 100;
        const QString dir = change >= 0 ? "artış 📈" : "azalış 📉";
        showResult(QString("%%1 %2").arg(gnum(std::fabs(change)), dir),
                   QString("Formül: ((%1 - %2) / |%2|) × 100 = %%3\nFark: %4")
                       .arg(g(b), g(a), gnum(change, 6), gnum(b - a)));
        break;
    }
    case 3: {
        const double avg = (std::fabs(a) + std::fabs(b)) / 2;
        if (avg == 0) return showError("⚠ Her iki değer de sıfır olamaz");
        const double diff = std::fabs(a - b) / avg * 100;
        showResult(QString("Yüzdelik fark: %%1").arg(gnum(diff)),
                   QString("Formül: |%1 - %2| / ((|%1| + |%2|) / 2) × 100 = %%3\n"
                           "Mutlak fark: %4  |  Ortalama: %5")
                       .arg(g(a), g(b), gnum(diff, 6), gnum(std::fabs(a - b)), gnum(avg)));
        break;
    }
    case 4: {
        const double rate = m_kdvRate->currentText().toDouble();
        if (m_kdvDir->currentIndex() == 0) {
            const double kdv = a * rate / 100, total = a + kdv;
            showResult(QString("KDV Dahil: %1").arg(util::num(total)),
                       QString("KDV Hariç Tutar: %1\nKDV Tutarı (%%2): %3\nKDV Dahil Toplam: %4")
                           .arg(util::num(a), g(rate), util::num(kdv), util::num(total)));
        } else {
            const double base = a / (1 + rate / 100), kdv = a - base;
            showResult(QString("KDV Hariç: %1").arg(util::num(base)),
                       QString("KDV Dahil Tutar: %1\nKDV Tutarı (%%2): %3\nKDV Hariç Tutar: %4")
                           .arg(util::num(a), g(rate), util::num(kdv), util::num(base)));
        }
        break;
    }
    }
}
