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
