#pragma once
#include <QDialog>
#include <QJsonObject>

class QLineEdit;
class QTableWidget;

class TaxSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit TaxSettingsDialog(QWidget *parent, const QJsonObject &taxSettings);

    QJsonObject getSettings() const;

private:
    QTableWidget *createTable(const QString &title, const QString &dilimKey);

    QJsonObject m_taxSettings;
    QLineEdit *m_asgariUcretInput;
    QTableWidget *m_tableUcret;
    QTableWidget *m_tableUcretDisi;
};
