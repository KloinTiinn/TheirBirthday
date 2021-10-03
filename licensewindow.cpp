/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "licensewindow.h"
#include "ui_licensewindow.h"

#include <QTextStream>

LicenseWindow::LicenseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseWindow)
{
    ui->setupUi(this);

    QFile fl(":/new/files/COPYING");
    if (fl.open(QIODevice::ReadOnly))
    {
        QTextStream in(&fl);
        QString sFileBody = "";
        // Считываем файл строка за строкой
        while (!in.atEnd())
        { // метод atEnd() возвращает true, если в потоке больше нет данных для чтения
            sFileBody += in.readLine() + "\n"; // метод readLine() считывает одну строку из потока
        }
        //QString sFileBody = codec->toUnicode(sByteArray);
        ui->plainTextEdit->setPlainText(sFileBody);
        fl.close();
    }
}

LicenseWindow::~LicenseWindow()
{
    delete ui;
}
