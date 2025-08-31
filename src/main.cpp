#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include <QMessageBox>

static QString readQss(const QString& path) {
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return {};
    QTextStream ts(&f);
    return ts.readAll();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 用 Fusion 作为基样式，避免平台原生控件“顶掉”QSS 的部分外观
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();
    return app.exec();
}
