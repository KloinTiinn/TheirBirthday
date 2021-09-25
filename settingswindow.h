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
    explicit SettingsWindow(QWidget *parent = nullptr, int pdays = 14);
    int getDays();//получаем выбранное значение "Напоминать за Х дней"
    ~SettingsWindow();

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
