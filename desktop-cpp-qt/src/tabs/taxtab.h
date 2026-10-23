#pragma once
#include <QWidget>
#include <QJsonObject>
#include <vector>
#include "taxcalc.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QTextEdit;
class QVBoxLayout;

class InsuranceItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit InsuranceItemWidget(QWidget *parent = nullptr);

    struct Data {
        int ayIdx;
        bool isHayat;
        double prim;
    };

    Data getData() const;

signals:
    void removed(InsuranceItemWidget *widget);

private:
    QComboBox *m_ayCombo;
    QComboBox *m_tipCombo;
    QLineEdit *m_primInput;
    QPushButton *m_removeBtn;
};

class TaxTab : public QWidget {
    Q_OBJECT
public:
    explicit TaxTab(const QJsonObject &settings, QWidget *parent = nullptr);

    QString lastFullResult() const { return m_lastFullResult; }
    QJsonObject lastResultData() const { return m_lastResultData; }

    void updateSettings(const QJsonObject &settings) { m_settings = settings; }

private slots:
    void addPolicy();
    void removePolicy(InsuranceItemWidget *widget);
    void toggleInsurance();
    void onCurrChanged(int idx);
    void calculate();
    void showDetailsPopup();

private:
    void initUi();

    QJsonObject m_settings;

    QComboBox *m_incomeType;
    QLineEdit *m_brutInput;

    QGroupBox *m_insGroup;
    QPushButton *m_toggleBtn;
    QWidget *m_insContainer;
    QVBoxLayout *m_poliListLayout;
    QComboBox *m_kurCombo;
    QWidget *m_usdWidget;
    QLineEdit *m_usdInput;
    QPushButton *m_addPoliBtn;
    QPushButton *m_detailsBtn;
    QTextEdit *m_resultText;

    std::vector<InsuranceItemWidget*> m_poliWidgets;
    QString m_lastFullResult;
    QJsonObject m_lastResultData;
};
