#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("OMR");
    a.setOrganizationName("PouyaApps");
    a.setOrganizationDomain("PouyaApps.com");
    MainWindow w;

    w.show();
    a.exec();
}
