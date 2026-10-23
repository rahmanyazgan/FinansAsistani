#pragma once
#include <QWidget>
#include <vector>
#include "compound.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QTableWidget;
class QHBoxLayout;
namespace QCharts {
class QChartView;
}
#include <QChartView>

class CompoundInterestTab : public QWidget {
    Q_OBJECT
public:
    explicit CompoundInterestTab(QWidget *parent = nullptr);

private slots:
    void calculate();
    void reset();
    void showTablePopup();

private:
    void initUi();
    void showResults(const std::vector<compound::YearRow> &mainRes,
                     const std::vector<compound::YearRow> &lowRes,
                     const std::vector<compound::YearRow> &highRes,
                     double rate, double var);

    QLineEdit *m_principalInput;
    QLineEdit *m_monthlyInput;
    QSpinBox *m_yearsSpin;
    QDoubleSpinBox *m_rateSpin;
    QDoubleSpinBox *m_varianceSpin;
    QComboBox *m_freqCombo;

    QWidget *m_resultSection;
    QHBoxLayout *m_cardsH;
    QChartView *m_chartView;
    QPushButton *m_tblToggle;
    QTableWidget *m_yearTable;
};
