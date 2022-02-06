/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "mainwindow.h"
#include <QApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
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

    QSystemSemaphore semaphore("7b220e83-9ad0-43f1-babb-6bb7211c14b4", 1);  // создаём семафор
    semaphore.acquire(); // Поднимаем семафор, запрещая другим экземплярам работать с ра
    //для линуксов
    QSharedMemory nixFixSharedMemory("a051da4c-ea1f-4348-908f-b2810f015d20");
    if(nixFixSharedMemory.attach())
    {
        nixFixSharedMemory.detach();
    }

    QSharedMemory sharedMemory("a051da4c-ea1f-4348-908f-b2810f015d20");  // Создаём экземпляр разделяемой памяти
    bool isRunning = false;            // переменную для проверки ууже запущенного приложения
    if (sharedMemory.attach())
    { // пытаемся присоединить экземпляр разделяемой памяти к уже существующему сегменту
        isRunning = true;      // Если успешно, то определяем, что уже есть запущенный экземпляр
    }
    else
    {
        sharedMemory.create(1); // В противном случае выделяем 1 байт памяти
        isRunning = false;     // И определяем, что других экземпляров не запущено
    }

    semaphore.release();        // Отпускаем семафор

    // Если уже запущен один экземпляр приложения, то сообщаем об этом пользователю
    // и завершаем работу текущего экземпляра приложения
    if(isRunning)
    {
        //QMessageBox::warning(0, "!!!", "The application is already running!");
        return 1;
    }

    QCoreApplication::setOrganizationName("TheirBirthdaySoft");
    QCoreApplication::setApplicationName("TheirBirthday");
    MainWindow w;
    w.show();

    return a.exec();
}
