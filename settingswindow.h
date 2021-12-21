/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr, int pdays = 14, QString pdelimiter = "/", bool ptray = false);
    int getDays();//получаем выбранное значение "Напоминать за Х дней"
    QString getDelimiter();//получаем "Разделитель для отображения"
    bool getTray();//получаем "Сворачивать в трей"
    ~SettingsWindow();

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
