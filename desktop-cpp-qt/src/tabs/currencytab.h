#pragma once
#include "rateprovider.h"
#include <QJsonObject>
#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

class CurrencyTab : public QWidget {
    Q_OBJECT
public:
    explicit CurrencyTab(const QJsonObject &settings, QWidget *parent = nullptr);

private:
    void initUi();
    void loadCurrencies();
    void onCurrenciesLoaded(const CurrencyNames &currencies);
    void swapCurrencies();
    void convert();
    void onRatesFetched(const QString &base, const Rates &rates);
    void onRatesError(const QString &err);
    void doConvert(double amount, const QString &base, const QString &target, const Rates &rates);
    void resetButton();

    QJsonObject m_settings;
    RateProvider *m_provider;
    QMap<QString, Rates> m_ratesCache;
    qint64 m_cacheTime = 0;
    int m_cacheTtl = 600;

    double m_amountPending = 0;
    QString m_basePending, m_targetPending;

    QLineEdit *m_amount;
    QComboBox *m_from, *m_to;
    QPushButton *m_convertBtn;
    QLabel *m_result, *m_update;
};
