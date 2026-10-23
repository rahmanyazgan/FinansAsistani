#include "compoundinteresttab.h"
#include "util.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QDialog>
#include <QMessageBox>
#include <QDoubleValidator>

#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

static const QList<QPair<QString, int>> FREQ_LIST = {
    {"Günlük", 365},
    {"Haftalık", 52},
    {"Aylık", 12},
    {"3 Aylık", 4},
    {"6 Aylık", 2},
    {"Yıllık", 1}
};

CompoundInterestTab::CompoundInterestTab(QWidget *parent) : QWidget(parent) {
    initUi();
}

void CompoundInterestTab::initUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto *formWidget = new QWidget(this);
    formWidget->setObjectName("contentWidget");
    formWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *fbox = new QVBoxLayout(formWidget);
    fbox->setSpacing(10);
    fbox->setContentsMargins(16, 14, 16, 10);

    auto *hdr = new QLabel("📈 Bileşik Kar Hesaplayıcısı", this);
    hdr->setObjectName("headerLabel");
    fbox->addWidget(hdr);

    auto *groupsH = new QHBoxLayout();
    groupsH->setSpacing(12);

    // Grup 1
    auto *grp1 = new QGroupBox("Yatırım & Katkı", this);
    auto *g1 = new QGridLayout(grp1);
    g1->setContentsMargins(12, 16, 12, 12);
    g1->setVerticalSpacing(8);
    g1->setHorizontalSpacing(16);
    g1->setColumnStretch(0, 2);
    g1->setColumnStretch(1, 1);

    g1->addWidget(new QLabel("İlk Yatırım", this), 0, 0);
    m_principalInput = new QLineEdit("0", this);
    m_principalInput->setValidator(new QDoubleValidator(0, 999999999999.0, 2, this));
    g1->addWidget(m_principalInput, 0, 1);

    g1->addWidget(new QLabel("Aylık Katkı", this), 1, 0);
    m_monthlyInput = new QLineEdit("5000", this);
    m_monthlyInput->setValidator(new QDoubleValidator(-999999999.0, 999999999.0, 2, this));
    g1->addWidget(m_monthlyInput, 1, 1);

    g1->addWidget(new QLabel("Süre (Yıl)", this), 2, 0);
    m_yearsSpin = new QSpinBox(this);
    m_yearsSpin->setRange(1, 100);
    m_yearsSpin->setValue(10);
    g1->addWidget(m_yearsSpin, 2, 1);

    groupsH->addWidget(grp1);

    // Grup 2
    auto *grp2 = new QGroupBox("Kar Oranı & Frekans", this);
    auto *g2 = new QGridLayout(grp2);
    g2->setContentsMargins(12, 16, 12, 12);
    g2->setVerticalSpacing(8);
    g2->setHorizontalSpacing(16);
    g2->setColumnStretch(0, 2);
    g2->setColumnStretch(1, 1);

    g2->addWidget(new QLabel("Tahmini Kar Oranı", this), 0, 0);
    m_rateSpin = new QDoubleSpinBox(this);
    m_rateSpin->setRange(0, 200);
    m_rateSpin->setValue(10);
    m_rateSpin->setSuffix("  %");
    m_rateSpin->setDecimals(2);
    g2->addWidget(m_rateSpin, 0, 1);

    g2->addWidget(new QLabel("Varyans Aralığı", this), 1, 0);
    m_varianceSpin = new QDoubleSpinBox(this);
    m_varianceSpin->setRange(0, 100);
    m_varianceSpin->setValue(3);
    m_varianceSpin->setSuffix("  %");
    m_varianceSpin->setDecimals(2);
    g2->addWidget(m_varianceSpin, 1, 1);

    g2->addWidget(new QLabel("Bileşik Frekans", this), 2, 0);
    m_freqCombo = new QComboBox(this);
    for (const auto &p : FREQ_LIST) {
        m_freqCombo->addItem(p.first, p.second);
    }
    m_freqCombo->setCurrentIndex(0); // Günlük
    g2->addWidget(m_freqCombo, 2, 1);

    groupsH->addWidget(grp2);
    fbox->addLayout(groupsH);

    auto *btnH = new QHBoxLayout();
    btnH->addStretch();
    auto *calcBtn = new QPushButton("Hesapla", this);
    calcBtn->setMinimumWidth(130);
    connect(calcBtn, &QPushButton::clicked, this, &CompoundInterestTab::calculate);

    auto *resetBtn = new QPushButton("Sıfırla", this);
    resetBtn->setMinimumWidth(100);
    connect(resetBtn, &QPushButton::clicked, this, &CompoundInterestTab::reset);

    btnH->addWidget(calcBtn);
    btnH->addWidget(resetBtn);
    fbox->addLayout(btnH);

    mainLayout->addWidget(formWidget);

    // Sonuç bölümü
    m_resultSection = new QWidget(this);
    m_resultSection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *resV = new QVBoxLayout(m_resultSection);
    resV->setContentsMargins(16, 8, 16, 10);
    resV->setSpacing(8);

    auto *cardsW = new QWidget(m_resultSection);
    m_cardsH = new QHBoxLayout(cardsW);
    m_cardsH->setSpacing(10);
    cardsW->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    resV->addWidget(cardsW);

    m_chartView = new QChartView(m_resultSection);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(260);
    m_chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_chartView->setStyleSheet("border: 1px solid #1e3a5f; border-radius: 12px;");
    resV->addWidget(m_chartView, 1);

    auto *tblHdr = new QHBoxLayout();
    auto *tblTitle = new QLabel("Yıllık Büyüme Dökümü", this);
    tblTitle->setObjectName("subHeaderLabel");
    m_tblToggle = new QPushButton("📊 Tabloyu Göster", this);
    m_tblToggle->setObjectName("toggleBtn");
    connect(m_tblToggle, &QPushButton::clicked, this, &CompoundInterestTab::showTablePopup);
    tblHdr->addWidget(tblTitle);
    tblHdr->addStretch();
    tblHdr->addWidget(m_tblToggle);
    resV->addLayout(tblHdr);

    m_yearTable = new QTableWidget(this);
    m_yearTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_yearTable->setAlternatingRowColors(true);
    m_yearTable->hide();

    m_resultSection->hide();
    mainLayout->addWidget(m_resultSection, 1);
}

void CompoundInterestTab::reset() {
    m_principalInput->setText("0");
    m_monthlyInput->setText("5000");
    m_yearsSpin->setValue(10);
    m_rateSpin->setValue(10);
    m_varianceSpin->setValue(3);
    m_freqCombo->setCurrentIndex(0);
    m_resultSection->hide();
}

void CompoundInterestTab::calculate() {
    bool ok = false;
    double principal = util::parse(m_principalInput->text(), &ok);
    if (!ok) {
        QMessageBox::warning(this, "Hata", "Geçerli bir başlangıç yatırımı girin.");
        return;
    }
    double monthly = util::parse(m_monthlyInput->text(), &ok);
    if (!ok) monthly = 0.0;

    int years = m_yearsSpin->value();
    double rate = m_rateSpin->value();
    double var = m_varianceSpin->value();
    int n = m_freqCombo->currentData().toInt();
    if (n <= 0) n = 365;

    auto mainRes = compound::futureValue(principal, monthly, years, rate, n);
    std::vector<compound::YearRow> lowRes;
    std::vector<compound::YearRow> highRes;

    if (var > 0) {
        lowRes = compound::futureValue(principal, monthly, years, std::max(0.0, rate - var), n);
        highRes = compound::futureValue(principal, monthly, years, rate + var, n);
    }

    showResults(mainRes, lowRes, highRes, rate, var);
}

void CompoundInterestTab::showResults(const std::vector<compound::YearRow> &mainRes,
                                     const std::vector<compound::YearRow> &lowRes,
                                     const std::vector<compound::YearRow> &highRes,
                                     double rate, double var) {
    if (mainRes.empty()) return;

    QLayoutItem *child;
    while ((child = m_cardsH->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    double finalBal = mainRes.back().balance;
    double totalContrib = mainRes.back().contributed;
    double totalInt = mainRes.back().interest;

    auto makeCard = [](const QString &title, double amount, const QString &color) -> QFrame* {
        auto *frame = new QFrame();
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setStyleSheet(QString("QFrame { border: 2px solid %1; border-radius: 14px; }").arg(color));
        auto *cv = new QVBoxLayout(frame);
        cv->setContentsMargins(16, 14, 16, 14);
        cv->setSpacing(4);

        auto *tLbl = new QLabel(title);
        tLbl->setStyleSheet(QString("color:%1; font-size:10px; font-weight:bold; letter-spacing:1px;").arg(color));
        auto *vLbl = new QLabel(util::num(amount, 2));
        vLbl->setStyleSheet("font-size:20px; font-weight:bold;");

        cv->addWidget(tLbl);
        cv->addWidget(vLbl);
        return frame;
    };

    m_cardsH->addWidget(makeCard("💰  NİHAİ BAKİYE", finalBal, "#00d4ff"));
    m_cardsH->addWidget(makeCard("💵  TOPLAM KATKI", totalContrib, "#22c55e"));
    m_cardsH->addWidget(makeCard("📈  KAR", totalInt, "#f59e0b"));

    if (var > 0 && !lowRes.empty() && !highRes.empty()) {
        m_cardsH->addWidget(makeCard(QString("📉  DÜŞÜK (−%1%)").arg(util::num(var, 1)), lowRes.back().balance, "#ef4444"));
        m_cardsH->addWidget(makeCard(QString("🚀  YÜKSEK (+%1%)").arg(util::num(var, 1)), highRes.back().balance, "#a855f7"));
    }

    // Grafik
    auto *chart = new QChart();
    chart->setBackgroundVisible(false);
    chart->setTitle(QString("YILLARA GÖRE BAKİYE GELİŞİMİ  —  Kar Oranı: %%1").arg(util::num(rate, 1)));
    chart->setTitleFont(QFont("Arial", 10, QFont::Bold));
    chart->setTitleBrush(QColor("#a9d6e5"));

    auto addLine = [&](const std::vector<compound::YearRow> &res, const QString &name, const QString &hexColor) {
        auto *s = new QLineSeries();
        s->setName(name);
        s->setColor(QColor(hexColor));
        for (const auto &row : res) {
            s->append(row.year, row.balance);
        }
        chart->addSeries(s);
        return s;
    };

    addLine(mainRes, QString("Bakiye (Kar %%1)").arg(util::num(rate, 1)), "#00d4ff");

    auto *contribS = new QLineSeries();
    contribS->setName("Toplam Katkı");
    contribS->setColor(QColor("#22c55e"));
    for (const auto &row : mainRes) {
        contribS->append(row.year, row.contributed);
    }
    chart->addSeries(contribS);

    if (var > 0 && !lowRes.empty() && !highRes.empty()) {
        addLine(lowRes, QString("Düşük (−%1%)").arg(util::num(var, 1)), "#ef4444");
        addLine(highRes, QString("Yüksek (+%1%)").arg(util::num(var, 1)), "#a855f7");
    }

    chart->createDefaultAxes();
    for (auto *ax : chart->axes(Qt::Horizontal)) {
        ax->setLabelsBrush(QColor("#a9d6e5"));
        ax->setTitleBrush(QColor("#a9d6e5"));
        ax->setTitleText("Yıl");
    }
    for (auto *ax : chart->axes(Qt::Vertical)) {
        ax->setLabelsBrush(QColor("#a9d6e5"));
        ax->setTitleBrush(QColor("#a9d6e5"));
        ax->setTitleText("Değer");
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelBrush(QColor("#a9d6e5"));
    m_chartView->setChart(chart);

    // Tablo verisi
    int years = static_cast<int>(mainRes.size());
    m_yearTable->clear();
    m_yearTable->setRowCount(years);
    m_yearTable->setColumnCount(4);
    m_yearTable->setHorizontalHeaderLabels({"Yıl", "Bakiye", "Toplam Katkı", "Kar"});
    m_yearTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < years; ++i) {
        const auto &row = mainRes[i];
        QStringList vals = {QString::number(row.year),
                            util::num(row.balance, 2),
                            util::num(row.contributed, 2),
                            util::num(row.interest, 2)};
        for (int col = 0; col < 4; ++col) {
            auto *item = new QTableWidgetItem(vals[col]);
            item->setTextAlignment(Qt::AlignCenter);
            m_yearTable->setItem(i, col, item);
        }
    }

    m_resultSection->show();
}

void CompoundInterestTab::showTablePopup() {
    if (m_yearTable->rowCount() == 0) return;

    QDialog dlg(this);
    dlg.setWindowTitle("Yıllık Büyüme Dökümü");
    dlg.resize(560, 480);
    auto *lay = new QVBoxLayout(&dlg);
    lay->setContentsMargins(16, 16, 16, 16);
    lay->setSpacing(10);

    auto *lbl = new QLabel("Yıllık Büyüme Dökümü", &dlg);
    lbl->setObjectName("subHeaderLabel");
    lay->addWidget(lbl);

    auto *popupTable = new QTableWidget(m_yearTable->rowCount(), m_yearTable->columnCount(), &dlg);
    popupTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    popupTable->setAlternatingRowColors(true);

    QStringList headers;
    for (int c = 0; c < m_yearTable->columnCount(); ++c) {
        headers << m_yearTable->horizontalHeaderItem(c)->text();
    }
    popupTable->setHorizontalHeaderLabels(headers);
    popupTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    popupTable->verticalHeader()->setVisible(false);

    for (int r = 0; r < m_yearTable->rowCount(); ++r) {
        for (int c = 0; c < m_yearTable->columnCount(); ++c) {
            auto *src = m_yearTable->item(r, c);
            if (src) {
                auto *itm = new QTableWidgetItem(src->text());
                itm->setTextAlignment(Qt::AlignCenter);
                popupTable->setItem(r, c, itm);
            }
        }
    }
    lay->addWidget(popupTable);

    auto *btnClose = new QPushButton("Kapat", &dlg);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    auto *btnH = new QHBoxLayout();
    btnH->addStretch();
    btnH->addWidget(btnClose);
    lay->addLayout(btnH);

    dlg.exec();
}
