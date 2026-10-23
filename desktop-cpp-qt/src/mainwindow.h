#pragma once
#include <QMainWindow>
#include <QJsonObject>

class QTabWidget;
class QPushButton;
class QMenu;
class CurrencyTab;
class PercentTab;
class TaxTab;
class CompoundInterestTab;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onTabChanged(int index);
    void showThemeMenu();
    void applyTheme(const QString &name);
    void openTaxSettings();

private:
    void initUi();

    QJsonObject m_settings;
    QString m_currentTheme;

    QTabWidget *m_tabs;
    CurrencyTab *m_currencyTab;
    PercentTab *m_percentTab;
    TaxTab *m_taxTab;
    CompoundInterestTab *m_compoundTab;

    QPushButton *m_btnTaxSettings;
    QPushButton *m_themeBtn;
    QMenu *m_themeMenu;
};
