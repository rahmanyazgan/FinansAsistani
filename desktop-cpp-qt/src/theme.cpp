#include "theme.h"
#include <QColor>

namespace theme {

static Theme mk(std::initializer_list<std::pair<const char *, const char *>> kv) {
    Theme t;
    for (auto &p : kv) t.insert(p.first, p.second);
    return t;
}

static const QMap<QString, Theme> &all() {
    static const QMap<QString, Theme> m = {
        {"Sade", mk({{"bg_main", "#f5f5f5"}, {"bg_secondary", "#e8e8e8"}, {"bg_input", "#ffffff"},
                     {"border", "#cccccc"}, {"accent", "#2563eb"}, {"accent_hover", "#3b82f6"},
                     {"accent_pressed", "#1d4ed8"}, {"text", "#1a1a1a"}, {"text_muted", "#555555"},
                     {"text_tab", "#444444"}, {"result", "#16a34a"}, {"textedit", "#1a1a1a"}})},
        {"Koyu Mavi", mk({{"bg_main", "#1a1a2e"}, {"bg_secondary", "#16213e"}, {"bg_input", "#0f3460"},
                          {"border", "#1a3a6e"}, {"accent", "#f59e0b"}, {"accent_hover", "#fbbf24"},
                          {"accent_pressed", "#d97706"}, {"text", "#ffffff"}, {"text_muted", "#e0e0e0"},
                          {"text_tab", "#a8b2d1"}, {"result", "#10b981"}, {"textedit", "#ccd6f6"}})},
        {"Mor Gece", mk({{"bg_main", "#1b1030"}, {"bg_secondary", "#2d1b69"}, {"bg_input", "#3b2380"},
                         {"border", "#5533a0"}, {"accent", "#a855f7"}, {"accent_hover", "#c084fc"},
                         {"accent_pressed", "#7c3aed"}, {"text", "#ffffff"}, {"text_muted", "#e0d4f5"},
                         {"text_tab", "#c4b5fd"}, {"result", "#4ade80"}, {"textedit", "#e2d9f3"}})},
        {"Yeşil Orman", mk({{"bg_main", "#0d1f0d"}, {"bg_secondary", "#1a3a1a"}, {"bg_input", "#254d25"},
                            {"border", "#2d6a2d"}, {"accent", "#22c55e"}, {"accent_hover", "#4ade80"},
                            {"accent_pressed", "#16a34a"}, {"text", "#ffffff"}, {"text_muted", "#d4edda"},
                            {"text_tab", "#a3d9a5"}, {"result", "#fbbf24"}, {"textedit", "#d1e7dd"}})},
        {"Okyanus", mk({{"bg_main", "#050b14"}, {"bg_secondary", "#0d1b2a"}, {"bg_input", "#1b263b"},
                        {"border", "#1e3a5f"}, {"accent", "#00d4ff"}, {"accent_hover", "#33e0ff"},
                        {"accent_pressed", "#00a3cc"}, {"text", "#ffffff"}, {"text_muted", "#a9d6e5"},
                        {"text_tab", "#89c2d9"}, {"result", "#00f5d4"}, {"textedit", "#e0f1f4"}})},
        {"Gün Batımı", mk({{"bg_main", "#1f1115"}, {"bg_secondary", "#3b1c27"}, {"bg_input", "#55283a"},
                           {"border", "#7a3a52"}, {"accent", "#f97316"}, {"accent_hover", "#fb923c"},
                           {"accent_pressed", "#ea580c"}, {"text", "#ffffff"}, {"text_muted", "#f5ddd3"},
                           {"text_tab", "#e8a990"}, {"result", "#a3e635"}, {"textedit", "#fde8d8"}})},
    };
    return m;
}

const QStringList &names() {
    static const QStringList n = {"Sade", "Koyu Mavi", "Mor Gece", "Yeşil Orman", "Okyanus", "Gün Batımı"};
    return n;
}

Theme get(const QString &name) { return all().value(name); }

QString generateStyle(const Theme &t) {
    QString css = R"CSS(
QMainWindow { background-color: {bg_main}; }
QTabWidget::pane { border: 1px solid {bg_secondary}; background-color: {bg_main}; border-radius: 8px; }
QTabBar::tab {
    background-color: {bg_secondary}; color: {text_tab};
    padding: 8px 24px; margin: 2px 1px;
    border-top-left-radius: 8px; border-top-right-radius: 8px;
    font-size: 13px; font-weight: 600; min-width: 140px; min-height: 24px;
}
QTabBar::tab:selected { background-color: {bg_input}; color: {accent}; border-bottom: 3px solid {accent}; }
QTabBar::tab:hover { background-color: {bg_input}; color: {accent_hover}; }
QGroupBox {
    background-color: {bg_secondary}; border: 1px solid {bg_input};
    border-radius: 10px; margin-top: 14px; padding: 18px 14px 14px 14px;
    font-size: 13px; font-weight: bold; color: {accent};
}
QGroupBox::title {
    subcontrol-origin: margin; subcontrol-position: top left; padding: 4px 12px;
    background-color: {bg_input}; border-radius: 6px; color: {text};
}
QLabel { color: {text}; font-size: 12px; }
QLineEdit, QDoubleSpinBox, QSpinBox {
    background-color: {bg_input}; color: {text};
    border: 1px solid {border}; border-radius: 6px;
    padding: 8px 12px; font-size: 13px; min-height: 20px;
    selection-background-color: {accent};
}
QLineEdit:focus, QDoubleSpinBox:focus, QSpinBox:focus { border: 2px solid {accent}; }
QComboBox {
    background-color: {bg_input}; color: {text};
    border: 1px solid {border}; border-radius: 6px;
    padding: 8px 30px 8px 12px; font-size: 13px; min-width: 100px; min-height: 20px;
}
QComboBox QAbstractItemView {
    background-color: {bg_secondary}; color: {text};
    selection-background-color: {accent}; border: 1px solid {bg_input}; border-radius: 4px;
}
QPushButton {
    background-color: {accent}; color: #ffffff; border: none; border-radius: 8px;
    padding: 10px 24px; font-size: 13px; font-weight: bold;
}
QPushButton:hover { background-color: {accent_hover}; }
QPushButton:pressed { background-color: {accent_pressed}; }
QPushButton:disabled { background-color: #4a4a6a; color: #888; }
QRadioButton { color: {text_tab}; font-size: 12px; spacing: 8px; }
QRadioButton::indicator {
    width: 16px; height: 16px; border-radius: 8px;
    border: 2px solid {text_tab}; background-color: transparent;
}
QRadioButton::indicator:checked { background-color: {accent}; border-color: {accent}; }
QTextEdit {
    background-color: {bg_input}; color: {textedit};
    border: 1px solid {border}; border-radius: 8px; padding: 10px; font-size: 12px;
    font-family: 'Consolas', 'Courier New', monospace;
}
QStatusBar { background-color: {bg_secondary}; color: {text_tab}; font-size: 11px; border-top: 1px solid {bg_input}; }
QMenuBar { background-color: {bg_secondary}; color: {text}; font-size: 12px; border-bottom: 1px solid {bg_input}; }
QMenuBar::item:selected { background-color: {bg_input}; }
QMenu { background-color: {bg_secondary}; color: {text}; border: 1px solid {bg_input}; }
QMenu::item:selected { background-color: {accent}; color: #ffffff; }
QLabel#resultLabel {
    background-color: {bg_input}; color: {result};
    border: 2px solid {border}; border-radius: 10px;
    padding: 14px; font-size: 18px; font-weight: bold; qproperty-alignment: AlignCenter;
}
QLabel#resultLabelSmall {
    background-color: {bg_input}; color: {result};
    border: 1px solid {border}; border-radius: 8px;
    padding: 10px; font-size: 14px; font-weight: bold; qproperty-alignment: AlignCenter;
}
QLabel#infoLabel { color: {text_muted}; font-size: 11px; font-style: italic; }
QLabel#headerLabel { color: {text}; font-size: 15px; font-weight: bold; }
QLabel#subHeaderLabel { color: {accent}; font-size: 13px; font-weight: bold; }
QPushButton#toggleBtn {
    background-color: {bg_input}; color: {text_muted}; border: 1px solid {border};
    padding: 4px 12px; font-size: 11px; font-weight: normal;
}
QPushButton#toggleBtn:hover { color: {accent}; background-color: {bg_secondary}; }
QPushButton#removeBtn {
    background-color: #991b1b; color: #ffffff; border: none; border-radius: 6px;
    font-size: 14px; font-weight: bold; padding: 0px; text-align: center;
}
QPushButton#removeBtn:hover { background-color: #dc2626; }
QScrollArea { border: none; background-color: transparent; }
QScrollArea > QWidget > QWidget { background-color: transparent; }
QWidget#contentWidget { background-color: {bg_main}; }
)CSS";
    for (auto it = t.constBegin(); it != t.constEnd(); ++it)
        css.replace("{" + it.key() + "}", it.value());
    return css;
}

QPalette palette(const Theme &t) {
    QPalette p;
    p.setColor(QPalette::Window, QColor(t["bg_main"]));
    p.setColor(QPalette::WindowText, QColor(t["text_tab"]));
    p.setColor(QPalette::Base, QColor(t["bg_input"]));
    p.setColor(QPalette::AlternateBase, QColor(t["bg_secondary"]));
    p.setColor(QPalette::ToolTipBase, QColor(t["bg_secondary"]));
    p.setColor(QPalette::ToolTipText, QColor(t["text"]));
    p.setColor(QPalette::Text, QColor(t["text"]));
    p.setColor(QPalette::Button, QColor(t["bg_secondary"]));
    p.setColor(QPalette::ButtonText, QColor(t["text"]));
    p.setColor(QPalette::Highlight, QColor(t["accent"]));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    return p;
}

}
