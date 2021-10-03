/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSettings>
#include "pathmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *);
    void resizeEvent(QResizeEvent *);

private slots:
    void on_actionColor_triggered();

    void on_actionFont_triggered();

    void on_actionExit_triggered();

    void on_actionEdit_triggered();

    void on_actionSettings_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionLicense_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    void refreshWindows();
    void setLst(const QString& path);
    void setLstEvents();
    void setLstDates();
    QString getDaysStr(int pDays);
    QString getResultStr(QList<QString>, int pdays);
    QString getResultTodayStr(QList<QString>);
    QString getResultYesterdayStr(QList<QString>);
    QString getResultTomorrowStr(QList<QString>);
    void findTodayStrs(QPlainTextEdit *);
    void setWindowFont();
    void setWindowSize();
    void setGColor();
    QList<QString> qlEvents;
    QList<QString> qlDates;
    QList<QString> qlToday;
    QColor gColor;
    int gDays;
    QSettings gSettings;
    PathManager pathMan;
};

#endif // MAINWINDOW_H
