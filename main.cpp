/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

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
    QCoreApplication::setApplicationName("TheirBirthday");
    MainWindow w;
    w.show();

    return a.exec();
}
