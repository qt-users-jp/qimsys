#include "mainwindow.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    qputenv("GTK_IM_MODULE", QByteArray("qimsys"));
    qputenv("QT_IM_MODULE", QByteArray("qimsys"));
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
    mainWindow.showExpanded();

    return app.exec();
}
