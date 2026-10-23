#pragma once
#include <QDialog>
#include <QJsonObject>

class QTableWidget;
namespace QCharts {
class QChartView;
}
#include <QChartView>

class ResultDetailsDialog : public QDialog {
    Q_OBJECT
public:
    explicit ResultDetailsDialog(QWidget *parent, const QJsonObject &data);

private slots:
    void showExportMenu();
    void saveAsImage();
    void saveAsPdf();
    void copyAll();

private:
    QJsonObject m_data;
    QTableWidget *m_table;
    QChartView *m_chartView;
};
