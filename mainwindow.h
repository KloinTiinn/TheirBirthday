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

private:
    Ui::MainWindow *ui;
    void refreshWindows();
    void setLst(const QString& path);
    void setLstEvents();
    void setLstDates();
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
    QSettings gSettings;
    PathManager pathMan;
};

#endif // MAINWINDOW_H
