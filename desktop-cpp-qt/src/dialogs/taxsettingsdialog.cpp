#include "taxsettingsdialog.h"
#include "util.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonArray>

TaxSettingsDialog::TaxSettingsDialog(QWidget *parent, const QJsonObject &taxSettings)
    : QDialog(parent), m_taxSettings(taxSettings) {
    setWindowTitle(QString("Vergi Ayarları (%1)").arg(util::currentYear()));
    resize(550, 600);

    auto *layout = new QVBoxLayout(this);

    auto *hl = new QHBoxLayout();
    hl->addWidget(new QLabel("Yıllık Brüt Asgari Ücret (₺):", this));
    double asgariUcret = m_taxSettings.value("asgari_ucret_yillik").toDouble(396360.0);
    m_asgariUcretInput = new QLineEdit(QString::number(asgariUcret, 'f', 2), this);
    hl->addWidget(m_asgariUcretInput);
    layout->addLayout(hl);

    m_tableUcret = createTable("ÜCRET Gelirleri İçin Vergi Dilimleri", "ucret_dilimleri");
    m_tableUcretDisi = createTable("ÜCRET DIŞI Gelirler İçin Vergi Dilimleri", "ucret_disi_dilimleri");

    auto *bl = new QHBoxLayout();
    auto *btnSave = new QPushButton("Kaydet", this);
    connect(btnSave, &QPushButton::clicked, this, &QDialog::accept);
    auto *btnCancel = new QPushButton("İptal", this);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    bl->addWidget(btnSave);
    bl->addWidget(btnCancel);
    layout->addLayout(bl);
}

QTableWidget *TaxSettingsDialog::createTable(const QString &title, const QString &dilimKey) {
    auto *layout = qobject_cast<QVBoxLayout*>(this->layout());
    layout->addWidget(new QLabel(title, this));

    auto *table = new QTableWidget(5, 2, this);
    table->setHorizontalHeaderLabels({"Üst Sınır (₺)", "Vergi Oranı (%)"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonArray arr = m_taxSettings.value(dilimKey).toArray();
    for (int i = 0; i < arr.size() && i < 5; ++i) {
        QJsonArray pair = arr[i].toArray();
        if (pair.size() >= 2) {
            double ust = pair[0].toDouble();
            double oran = pair[1].toDouble();
            QString ustTxt = ust > 1e11 ? "Limitsiz" : QString::number(ust, 'f', 2);
            table->setItem(i, 0, new QTableWidgetItem(ustTxt));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(oran * 100.0, 'f', 2)));
        }
    }
    layout->addWidget(table);
    return table;
}

QJsonObject TaxSettingsDialog::getSettings() const {
    QJsonObject newS;
    bool ok = false;
    double asgari = util::parse(m_asgariUcretInput->text(), &ok);
    newS["asgari_ucret_yillik"] = ok ? asgari : 396360.0;

    auto readTable = [](QTableWidget *table, const QJsonArray &fallback) -> QJsonArray {
        QJsonArray arr;
        for (int i = 0; i < table->rowCount(); ++i) {
            auto *itemUst = table->item(i, 0);
            auto *itemOran = table->item(i, 1);
            if (!itemUst || !itemOran) continue;

            QString ustTxt = itemUst->text().trimmed().toLower();
            double ust = 9999999999.0;
            if (ustTxt != "inf" && ustTxt != "limitsiz" && ustTxt != "sınırsız" && !ustTxt.isEmpty() && ustTxt != "0") {
                bool pOk = false;
                double v = util::parse(ustTxt, &pOk);
                if (pOk) ust = v;
            }

            bool oOk = false;
            double oran = util::parse(itemOran->text(), &oOk) / 100.0;
            if (oOk) {
                QJsonArray pair;
                pair.append(ust);
                pair.append(oran);
                arr.append(pair);
            }
        }
        return arr.isEmpty() ? fallback : arr;
    };

    newS["ucret_dilimleri"] = readTable(m_tableUcret, m_taxSettings.value("ucret_dilimleri").toArray());
    newS["ucret_disi_dilimleri"] = readTable(m_tableUcretDisi, m_taxSettings.value("ucret_disi_dilimleri").toArray());

    return newS;
}
