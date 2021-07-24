#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;

    QString sLocale = QLocale::system().name();

    if(sLocale == "ru_RU")
        translator.load(":/new/files/main_ru.qm");
    else
        translator.load(":/new/files/main_en.qm");

    a.installTranslator(&translator);

    QCoreApplication::setOrganizationName("Datasoft");
    QCoreApplication::setApplicationName("TheirBurthday");
    MainWindow w;
    w.show();

    return a.exec();
}
