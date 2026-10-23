#include "resultdetailsdialog.h"
#include "util.h"
#include "tabs/taxtab.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QPrinter>
#include <QJsonArray>
#include <QDateTime>

#include <QChart>
#include <QBarSet>
#include <QBarSeries>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

ResultDetailsDialog::ResultDetailsDialog(QWidget *parent, const QJsonObject &data)
    : QDialog(parent), m_data(data) {
    setWindowTitle("Sigorta Primi Vergi İade Hesaplama");
    resize(900, 850);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    int yil = m_data.value("yil").toInt(util::currentYear());
    auto *titleLbl = new QLabel(QString("%1 YILI ÖZETİ").arg(yil), this);
    titleLbl->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLbl);

    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(15);

    // 1. Kolon: Bilgi Kartları
    auto *cardsLayout = new QVBoxLayout();
    cardsLayout->setSpacing(10);

    auto createCard = [](const QString &title, double value) -> QFrame* {
        auto *card = new QFrame();
        card->setFrameShape(QFrame::StyledPanel);
        card->setMaximumHeight(100);
        card->setStyleSheet(
            "QFrame {"
            "    background-color: #122d4f;"
            "    border: 1px solid #1e3a5f;"
            "    border-radius: 10px;"
            "}"
        );
        auto *cardV = new QVBoxLayout(card);
        cardV->setContentsMargins(10, 10, 10, 10);
        cardV->setSpacing(4);

        auto *tLbl = new QLabel(title.toUpper());
        tLbl->setStyleSheet("font-size: 10px; color: #a9d6e5; font-weight: bold;");
        auto *vLbl = new QLabel(QString("₺ %1").arg(util::num(value, 2)));
        vLbl->setStyleSheet("font-size: 19px; color: #ffffff; font-weight: bold;");

        cardV->addWidget(tLbl);
        cardV->addWidget(vLbl);
        return card;
    };

    cardsLayout->addWidget(createCard("Toplam Prim Ödemesi", m_data.value("toplam_prim").toDouble()));
    cardsLayout->addWidget(createCard("Hak Edilen Vergi İadesi", m_data.value("vergi_iadesi").toDouble()));
    cardsLayout->addWidget(createCard("Matrahtan Düşülen Tutar", m_data.value("matrah_dusulen").toDouble()));
    topLayout->addLayout(cardsLayout, 1);

    // 2. Kolon: Grafik Alanı
    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setStyleSheet("background-color: #0c1929; border: 1px solid #1e3a5f; border-radius: 12px;");
    m_chartView->setMaximumHeight(320);

    auto *chart = new QChart();
    chart->setBackgroundVisible(false);
    chart->setTitle("AYLIK VERGİ İADESİ VE PRİM DAĞILIMI");
    chart->setTitleFont(QFont("Arial", 10, QFont::Bold));
    chart->setTitleBrush(QColor("#a9d6e5"));

    auto *setIade = new QBarSet("Vergi İadesi (₺)");
    setIade->setColor(QColor("#1b263b"));
    setIade->setBorderColor(QColor("#00d4ff"));

    auto *lineSeries = new QLineSeries();
    lineSeries->setName("İade Trendi (₺)");
    lineSeries->setColor(QColor("#00f5d4"));
    lineSeries->setPointsVisible(true);

    QJsonArray aylikArr = m_data.value("aylik_iade_listesi").toArray();
    double maxV = 1000.0;
    for (int i = 0; i < aylikArr.size(); ++i) {
        QJsonObject item = aylikArr[i].toObject();
        double iade = item.value("iade").toDouble();
        setIade->append(iade);
        lineSeries->append(i, iade);
        if (iade > maxV) maxV = iade;
    }

    auto *series = new QBarSeries();
    series->append(setIade);
    chart->addSeries(series);
    chart->addSeries(lineSeries);

    auto *axisX = new QBarCategoryAxis();
    QJsonArray aylarArr = m_data.value("ay_isimleri").toArray();
    QStringList shortMonths;
    for (const auto &m : aylarArr) {
        shortMonths << m.toString().left(3);
    }
    axisX->append(shortMonths);
    axisX->setLabelsBrush(QColor("#a9d6e5"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    lineSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, maxV * 1.3);
    axisY->setLabelsBrush(QColor("#a9d6e5"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    lineSeries->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelBrush(QColor("#a9d6e5"));
    m_chartView->setChart(chart);
    topLayout->addWidget(m_chartView, 2);

    // 3. Kolon: Özet Bilgiler Paneli
    auto *infoPanel = new QFrame(this);
    infoPanel->setStyleSheet("background-color: #122d4f; border: 1px solid #1e3a5f; border-radius: 10px;");
    infoPanel->setMaximumHeight(320);
    auto *infoV = new QVBoxLayout(infoPanel);
    infoV->setContentsMargins(15, 15, 15, 15);
    infoV->setSpacing(6);

    auto addInfoRow = [infoV](const QString &title, const QString &value) {
        auto *t = new QLabel(title.toUpper());
        t->setStyleSheet("font-size: 10px; color: #a9d6e5; font-weight: normal;");
        auto *v = new QLabel(value);
        v->setStyleSheet("font-size: 15px; color: #ffffff; font-weight: bold; margin-bottom: 4px;");
        infoV->addWidget(t);
        infoV->addWidget(v);
    };

    addInfoRow("Sigorta Türü", m_data.value("sigorta_turu").toString());
    addInfoRow("Vergi Dilimi", m_data.value("vergi_dilimi").toString());
    addInfoRow("Son Hesaplama", m_data.value("son_hesaplama").toString());
    infoV->addStretch();
    topLayout->addWidget(infoPanel, 1);

    mainLayout->addLayout(topLayout);

    // Orta Bölüm: Detay Tablosu
    auto *tableLbl = new QLabel("AYLIK PRİM VE VERGİ İADE DETAYLARI", this);
    tableLbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(tableLbl);

    m_table = new QTableWidget(12, 6, this);
    m_table->setHorizontalHeaderLabels({"AY", "ÖDENEN PRİM (₺)", "MATRAH İNDİRİMİ (₺)", "VERGİ DİLİMİ (%)", "VERGİ İADESİ (₺)", "DURUM"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setMinimumHeight(350);
    m_table->setStyleSheet(
        "QTableWidget {"
        "    background-color: #0c1929;"
        "    color: #ffffff;"
        "    border: 1px solid #1e3a5f;"
        "    gridline-color: #1b263b;"
        "    border-radius: 5px;"
        "}"
        "QHeaderView::section {"
        "    background-color: #1b263b;"
        "    color: #a9d6e5;"
        "    padding: 6px;"
        "    font-weight: bold;"
        "    border: none;"
        "}"
        "QTableWidget::item { padding: 5px; }"
    );

    int currentMonth = QDate::currentDate().month();
    double toplamPrim = m_data.value("toplam_prim").toDouble();

    for (int i = 0; i < 12; ++i) {
        QString ayIsim = (i < aylarArr.size()) ? aylarArr[i].toString().toUpper() : "";
        double iade = 0.0, oran = 0.0;
        if (i < aylikArr.size()) {
            QJsonObject item = aylikArr[i].toObject();
            iade = item.value("iade").toDouble();
            oran = item.value("oran").toDouble();
        }

        m_table->setItem(i, 0, new QTableWidgetItem(ayIsim));
        m_table->setItem(i, 1, new QTableWidgetItem(util::num(toplamPrim / 12.0, 0)));
        m_table->setItem(i, 2, new QTableWidgetItem(util::num(iade / std::max(0.01, oran), 0)));
        m_table->setItem(i, 3, new QTableWidgetItem(QString("%%%1").arg(qRound(oran * 100))));
        m_table->setItem(i, 4, new QTableWidgetItem(util::num(iade, 0)));

        QString durum = (i + 1 < currentMonth) ? "Ödendi" : ((i + 1 == currentMonth) ? "Bu Ay" : "Bekliyor");
        m_table->setItem(i, 5, new QTableWidgetItem(durum));
    }

    mainLayout->addWidget(m_table, 1);

    // Alt Bölüm
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    auto *btnStart = new QPushButton("YENİ HESAPLAMA BAŞLAT", this);
    btnStart->setMinimumSize(250, 45);
    btnStart->setStyleSheet("background-color: #1a3f6f; color: #00d4ff; border: 1px solid #00d4ff;");
    connect(btnStart, &QPushButton::clicked, this, &QDialog::close);

    auto *btnExport = new QPushButton("📄 RAPORU DIŞA AKTAR", this);
    btnExport->setMinimumSize(250, 45);
    connect(btnExport, &QPushButton::clicked, this, &ResultDetailsDialog::showExportMenu);

    bottomLayout->addWidget(btnStart);
    bottomLayout->addWidget(btnExport);
    mainLayout->addLayout(bottomLayout);
}

void ResultDetailsDialog::showExportMenu() {
    auto *menu = new QMenu(this);
    menu->setStyleSheet(
        "QMenu { background-color: #1b263b; color: #ffffff; border: 1px solid #1e3a5f; }"
        "QMenu::item:selected { background-color: #00d4ff; color: #000000; }"
    );

    auto *act1 = menu->addAction("🖼️ Resim Olarak Kaydet (PNG)");
    connect(act1, &QAction::triggered, this, &ResultDetailsDialog::saveAsImage);

    auto *act2 = menu->addAction("📄 PDF Belgesi Olarak Kaydet");
    connect(act2, &QAction::triggered, this, &ResultDetailsDialog::saveAsPdf);

    auto *act3 = menu->addAction("📋 Metin Olarak Kopyala");
    connect(act3, &QAction::triggered, this, &ResultDetailsDialog::copyAll);

    auto *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        QPoint pos = btn->mapToGlobal(btn->rect().topLeft());
        pos.setY(pos.y() - menu->sizeHint().height());
        menu->popup(pos);
    } else {
        menu->exec(QCursor::pos());
    }
}

void ResultDetailsDialog::saveAsImage() {
    QPixmap pixmap = grab();
    QString defaultName = QString("Vergi_Iadesi_Raporu_%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmm"));
    QString path = QFileDialog::getSaveFileName(this, "Raporu Kaydet", defaultName, "Resim Dosyası (*.png)");
    if (!path.isEmpty()) {
        pixmap.save(path, "PNG");
        QMessageBox::information(this, "Başarılı", "Rapor resim olarak kaydedildi.");
    }
}

void ResultDetailsDialog::saveAsPdf() {
    QString defaultName = QString("Vergi_Iadesi_Raporu_%1.pdf").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmm"));
    QString path = QFileDialog::getSaveFileName(this, "Raporu Kaydet", defaultName, "PDF Dosyası (*.pdf)");
    if (!path.isEmpty()) {
        QPixmap pixmap = grab();
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(path);
        printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

        QPainter painter(&printer);
        QRectF rect = painter.viewport();
        QSizeF size = pixmap.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(pixmap.rect());
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        QMessageBox::information(this, "Başarılı", "Rapor PDF olarak kaydedildi.");
    }
}

void ResultDetailsDialog::copyAll() {
    auto *parentTab = qobject_cast<TaxTab*>(parentWidget());
    QString txt = parentTab ? parentTab->lastFullResult() : "Veri bulunamadı.";
    QApplication::clipboard()->setText(txt);
    QMessageBox::information(this, "Bilgi", "Hesaplama dökümü panoya kopyalandı.");
}
