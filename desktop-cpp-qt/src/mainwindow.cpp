#include "mainwindow.h"
#include "settings.h"
#include "theme.h"
#include "tabs/currencytab.h"
#include "tabs/percenttab.h"
#include "tabs/taxtab.h"
#include "tabs/compoundinteresttab.h"
#include "dialogs/taxsettingsdialog.h"

#include <QTabWidget>
#include <QStatusBar>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Finans Asistanı");
    setWindowIcon(QIcon(":/icon.png"));
    setMinimumSize(720, 680);
    resize(800, 750);

    m_settings = settings::load();
    m_currentTheme = m_settings.value("theme").toString("Okyanus");

    initUi();
}

void MainWindow::initUi() {
    int lastTab = m_settings.value("last_tab").toInt(0);

    m_tabs = new QTabWidget(this);
    m_currencyTab = new CurrencyTab(m_settings, this);
    m_percentTab = new PercentTab(this);
    m_taxTab = new TaxTab(m_settings, this);
    m_compoundTab = new CompoundInterestTab(this);

    m_tabs->addTab(m_currencyTab, "💱 Kur Dönüşümü");
    m_tabs->addTab(m_percentTab, "📊 Yüzde Hesabı");
    m_tabs->addTab(m_taxTab, "🏛️ Vergi İadesi");
    m_tabs->addTab(m_compoundTab, "📈 Bileşik Kar");

    m_tabs->setCurrentIndex(lastTab);
    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    setCentralWidget(m_tabs);

    // Statusbar
    auto *sb = statusBar();
    sb->showMessage("Hazır");

    m_btnTaxSettings = new QPushButton("⚙️ Vergi Ayarları", this);
    m_btnTaxSettings->setObjectName("statusThemeBtn");
    connect(m_btnTaxSettings, &QPushButton::clicked, this, &MainWindow::openTaxSettings);
    m_btnTaxSettings->setVisible(lastTab == 2);
    sb->addPermanentWidget(m_btnTaxSettings);

    auto *themeLabel = new QLabel("🎨 Tema:", this);
    themeLabel->setStyleSheet("font-size: 11px; margin-left: 8px;");
    sb->addPermanentWidget(themeLabel);

    m_themeBtn = new QPushButton(m_currentTheme, this);
    m_themeBtn->setFixedWidth(130);
    m_themeBtn->setStyleSheet("font-size: 11px; padding: 2px 6px; max-height: 22px; text-align: left;");
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::showThemeMenu);
    sb->addPermanentWidget(m_themeBtn);

    m_themeMenu = new QMenu(this);
    for (const auto &name : theme::names()) {
        auto *action = m_themeMenu->addAction(name);
        connect(action, &QAction::triggered, this, [this, name]() {
            applyTheme(name);
        });
    }
}

void MainWindow::onTabChanged(int index) {
    m_btnTaxSettings->setVisible(index == 2);

    m_settings["last_tab"] = index;
    settings::save(m_settings);
}

void MainWindow::showThemeMenu() {
    m_themeMenu->adjustSize();
    QPoint pos = m_themeBtn->mapToGlobal(m_themeBtn->rect().topLeft());
    pos.setY(pos.y() - m_themeMenu->sizeHint().height());
    m_themeMenu->popup(pos);
}

void MainWindow::applyTheme(const QString &name) {
    auto t = theme::get(name);
    if (t.isEmpty()) return;

    m_currentTheme = name;
    m_themeBtn->setText(name);

    auto *app = qobject_cast<QApplication*>(QApplication::instance());
    if (app) {
        app->setStyleSheet(theme::generateStyle(t));
        app->setPalette(theme::palette(t));
    }

    m_settings["theme"] = name;
    settings::save(m_settings);
}

void MainWindow::openTaxSettings() {
    TaxSettingsDialog dlg(this, m_settings.value("tax_settings").toObject());
    if (dlg.exec() == QDialog::Accepted) {
        m_settings["tax_settings"] = dlg.getSettings();
        settings::save(m_settings);
        m_taxTab->updateSettings(m_settings);
        QMessageBox::information(this, "Bilgi", "Vergi ayarları güncellendi. Yeni hesaplamalar bu verilere göre yapılacaktır.");
    }
}
