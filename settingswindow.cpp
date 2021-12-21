/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent, int pdays, QString pdelimiter, bool ptray) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    //задаём значения по умолчанию
    ui->spinBox->setValue(pdays);
    ui->lineEdit->setText(pdelimiter);
    ui->checkBox->setChecked(ptray);
}
//получаем выбранное значение "Напоминать за Х дней"
int SettingsWindow::getDays()
{
    return ui->spinBox->value();
}

//получаем выбранное значение "Разделитель для отображения"
QString SettingsWindow::getDelimiter()
{
    return ui->lineEdit->text();
}
//получаем "Сворачивать в трей"
bool SettingsWindow::getTray()
{
    return ui->checkBox->isChecked();
}
SettingsWindow::~SettingsWindow()
{
    delete ui;
}
