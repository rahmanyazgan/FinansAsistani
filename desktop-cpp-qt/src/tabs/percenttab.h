#pragma once
#include <QWidget>

class QButtonGroup;
class QLabel;
class QLineEdit;
class QComboBox;

class PercentTab : public QWidget {
    Q_OBJECT
public:
    explicit PercentTab(QWidget *parent = nullptr);

private:
    void onModeChanged(int mode);
    void calculate();
    void showResult(const QString &result, const QString &detail);
    void showError(const QString &msg);

    QButtonGroup *m_modes;
    QLabel *m_lblA, *m_lblB, *m_lblKdv, *m_result, *m_detail;
    QLineEdit *m_a, *m_b;
    QComboBox *m_kdvRate, *m_kdvDir;
};
