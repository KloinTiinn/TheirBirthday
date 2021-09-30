/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent, int pdays) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->spinBox->setValue(pdays);
}
//получаем выбранное значение "Напоминать за Х дней"
int SettingsWindow::getDays()
{
    return ui->spinBox->value();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
