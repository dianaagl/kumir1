#include <QApplication>
#include <QtCore>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("TyulbashevSoft");
    a.setOrganizationDomain("Tyulbashev.ru");
    a.setApplicationName("Proga3");
    a.setApplicationVersion("3.0.0");

    const QString localeName = QLocale::system().name();
    QTranslator translator;
    if (translator.load(localeName, QApplication::applicationDirPath()+"/../share/proga3/translations/")) {
         QApplication::installTranslator(&translator);
    }


    CrissCrossWindow w;
    w.show();

    return a.exec();
}
