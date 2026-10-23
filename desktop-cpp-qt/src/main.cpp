#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"
#include "settings.h"
#include "theme.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    QJsonObject s = settings::load();
    QString themeName = s.value("theme").toString("Okyanus");
    Theme t = theme::get(themeName);
    if (t.isEmpty()) {
        t = theme::get("Okyanus");
    }

    app.setStyleSheet(theme::generateStyle(t));
    app.setPalette(theme::palette(t));

    MainWindow window;
    window.show();

    return app.exec();
}
