/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editwindow.h"
#include "settingswindow.h"

#include <QTextCodec>
#include <QDate>
#include <QDebug>
#include <QFileInfo>
#include <QColorDialog>
#include <QFontDialog>
#include <QSettings>
#include <QFont>
#include <QDesktopServices>
#include <QProcess>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), gSettings("Datasoft","TheirBirthday"),
    pathMan()
{
    ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    //цвет выделения по умолчанию
    gColor = QColor(Qt::green).lighter(125);
    //Напоминать за 14 дней по умолчанию
    gDays = gSettings.value("/Days", 14).toInt();
    //мало ли чего там с сеттингов считалось...
    if (gDays < 1 || gDays > 364) gDays = 14;
    // Прочтены ли файлы
    if (!pathMan.ok()) {
        QMessageBox::critical(0, tr("Ошибка"), pathMan.errString());
        return;
    }
    //запоминаем наши даты и события
    setLstEvents();
    setLstDates();

    setWindowFont();
    setGColor();
    setWindowSize();

    refreshWindows();
    startTimer(60000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setWindowFont()
{
    //считываем данные шрифта
    QString fntFamily = gSettings.value("/Font", "Arial").toString();
    int fntSize = gSettings.value("/FontSize", 8).toInt();
    bool fntItalic = gSettings.value("/FontItalic", false).toBool();
    int fntBold = gSettings.value("/FontBold", -1).toInt();

    //устанавливаем шрифт в окнах
    QFont fnt(fntFamily, fntSize, fntBold, fntItalic);
    ui->plainTEditDates->setFont(fnt);
    ui->plainTEditEvents->setFont(fnt);
}

void MainWindow::setGColor()
{
    //считываем данные цвета
    int r = gSettings.value("/Red", 0).toInt();
    int g = gSettings.value("/Green", 0).toInt();
    int b = gSettings.value("/Blue", 0).toInt();

    //устанавливаем цвет выделения
    if (r != 0 || g != 0 || b != 0)
        gColor = QColor::fromRgb(r, g, b);
}

void MainWindow::setWindowSize()
{
    //считываем размеры окна
    int frmWidth = gSettings.value("/Width", 578).toInt();
    int frmHeight = gSettings.value("/Height", 363).toInt();

    //устанавливаем размеры окна
    this->setGeometry(0, 0, frmWidth, frmHeight);
}
//заполняем полуокно по переданному имени файла
void MainWindow::setLst(const QString& path)
{
    QRegExp regexp("^[0-9][0-9]/[0-9][0-9]/[0-9][0-9][0-9][0-9]");
    QFile fl(path);
    if (!QFile::exists(path))
    {
        fl.open(QIODevice::WriteOnly | QIODevice::Append);
        fl.close();
    }

    if (fl.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("CP1251");
        while(!fl.atEnd())
        {
            QByteArray sBStr = fl.readLine();
            QString sStr = codec->toUnicode(sBStr);

            if (sStr.contains(regexp))
            {
                if (path.contains("events.txt"))
                    qlEvents << sStr;
                else
                    qlDates << sStr;
            }
        }
        fl.close();
    }
}
//Заполняем События (нижнее окно)
void MainWindow::setLstEvents()
{
    setLst(pathMan.eventsFilePath());
}
//Заполняем Даты (верхнее окно)
void MainWindow::setLstDates()
{
    setLst(pathMan.datesFilePath());
}
//формируем строки "Вчера"
QString MainWindow::getResultYesterdayStr(QList<QString> pql)
{
    QString sb = "";
    foreach(QString fs, pql)
    {
        QString sDate = fs.left(10);
        QDate dDate = QDate::fromString(sDate, "dd/MM/yyyy");
        if (dDate.day() == QDate::currentDate().addDays(-1).day() && dDate.month() == QDate::currentDate().addDays(-1).month())
            sb += tr("Вчера") + fs.replace(sDate, "");// + "\n";
    }
    return sb;
}
//формируем строки "Сегодня"
QString MainWindow::getResultTodayStr(QList<QString> pql)
{
    QString sb = "";
    foreach(QString fs, pql)
    {
        QString sDate = fs.left(10);
        QDate dDate = QDate::fromString(sDate, "dd/MM/yyyy");
        if (dDate.day() == QDate::currentDate().day() && dDate.month() == QDate::currentDate().month())
        {
            int iy = QDate::currentDate().year() - dDate.year();
            QString st = tr("Сегодня ") + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина") + ")";
            sb += st + "\n";
            qlToday.append(st);
        }
    }
    return sb;
}
//формируем строки "Завтра"
QString MainWindow::getResultTomorrowStr(QList<QString> pql)
{
    QString sb = "";
    foreach(QString fs, pql)
    {
        QString sDate = fs.left(10);
        QDate dDate = QDate::fromString(sDate, "dd/MM/yyyy");
        if (dDate.day() == QDate::currentDate().addDays(1).day() && dDate.month() == QDate::currentDate().addDays(1).month())
        {
            int iy = QDate::currentDate().year() - dDate.year();
            if (iy > 0)
                sb += tr("Завтра ") + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина") + ")\n";
            else
                sb += "Завтра " + fs.replace(sDate, "");
        }
    }
    return sb;
}
//разбираем, "день", "дня" или "дней", в зависимости от количества pdays
QString MainWindow::getDaysStr(int pdays)
{
    int remains1 = pdays;
    if (pdays >= 100)
        remains1 = pdays % 100;//остаток от деления на 100

    if (remains1 >= 11 && remains1 <=14) return tr("дней");

    int remains2 = remains1;

    if (remains1 >= 10)
        remains2 = remains1 % 10;//остаток от деления на 10

    switch (remains2)
    {
        case 1: return tr("день");
        case 2: case 3: case 4: return tr("дня");
        default: return tr("дней");
    }
}
//формируем строки "Через N дней"
QString MainWindow::getResultStr(QList<QString> pql, int pdays)
{
    if (pdays == -1)
        return getResultYesterdayStr(pql);
    if (pdays == 0)
        return getResultTodayStr(pql);
    if (pdays == 1)
        return getResultTomorrowStr(pql);
    QString sb = "";
    foreach(QString fs, pql)
    {
        QString sDate = fs.left(10);
        QDate dDate = QDate::fromString(sDate, "dd/MM/yyyy");
        if (dDate.day() == QDate::currentDate().addDays(pdays).day() && dDate.month() == QDate::currentDate().addDays(pdays).month())
        {
            QStringList slDayMonth = sDate.left(5).split("/");
            QString sLocale = QLocale::system().name();
            int iy = QDate::currentDate().year() - dDate.year();
            if (iy > 0)
            {
                if (sLocale == "en_US")
                {
                    sb += tr("Через ") + QString::number(pdays) + tr(" дней (") + slDayMonth[1] + "/" + slDayMonth[0] + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")\n";
                }
                else
                    sb += tr("Через ") + QString::number(pdays) + " " + getDaysStr(pdays) + " (" + sDate.left(5).replace("/", ".") + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")\n";
            }
            else
            {
                if (sLocale == "en_US")
                {
                    sb += tr("Через ") + QString::number(pdays) + tr(" дней (") + slDayMonth[1] + "/" + slDayMonth[0] + ") " + fs.replace(sDate, "");
                }
                else
                    sb += tr("Через ") + QString::number(pdays) + " " + getDaysStr(pdays) + " (" + sDate.left(5).replace("/", ".") + ") " + fs.replace(sDate, "");
            }
        }
    }
    return sb;
}
//Подсвечиваем цветом сегодняшнее
void MainWindow::findTodayStrs(QPlainTextEdit *pte)
{
    pte->moveCursor(QTextCursor::Start);

    QTextCursor cur = pte->textCursor();
    QList<QTextEdit::ExtraSelection> lSel;

    QTextCursor findCur;

    foreach(QString fs, qlToday)
    {
        findCur = pte->document()->find(fs, cur);
        if(findCur != cur)
        {
            QTextEdit::ExtraSelection xtra;
            xtra.format.setBackground(gColor);
            xtra.cursor = findCur;
            lSel.append(xtra);
            pte->setExtraSelections(lSel);
        }
        cur = findCur;
    }
}
//Обновляем главное окно
void MainWindow::refreshWindows()
{
    ui->plainTEditEvents->setPlainText("");
    ui->plainTEditDates->setPlainText("");

    QString sbEv = "", sbDt = "";

    for(int i = -1; i < gDays; i++)
        sbDt += getResultStr(qlDates, i);

    for(int i = -1; i < gDays; i++)
        sbEv += getResultStr(qlEvents, i);

    ui->plainTEditEvents->setPlainText(sbEv);
    ui->plainTEditDates->setPlainText(sbDt);
    //подсвечиваем строки "сегодня"
    findTodayStrs(ui->plainTEditEvents);
    findTodayStrs(ui->plainTEditDates);
}
//Выбор цвета
void MainWindow::on_actionColor_triggered()
{
    QColor tempColor = QColorDialog::getColor(gColor);
    if (tempColor.isValid())
    {
        gColor = tempColor;
        gSettings.setValue("/Red", tempColor.red());
        gSettings.setValue("/Green", tempColor.green());
        gSettings.setValue("/Blue", tempColor.blue());

        refreshWindows();
    }
}
//Выбор шрифта
void MainWindow::on_actionFont_triggered()
{
    bool bOk;
    QFont fnt = QFontDialog::getFont(&bOk);
    if (bOk)
    {//Сохраняем данные
        gSettings.setValue("/Font", fnt.family());
        gSettings.setValue("/FontSize", fnt.pointSize());
        gSettings.setValue("/FontItalic", fnt.italic());
        gSettings.setValue("/FontBold", fnt.Bold);
        ui->plainTEditDates->setFont(fnt);
        ui->plainTEditEvents->setFont(fnt);
    }
}
//Обновляем окно каждую минуту
void MainWindow::timerEvent(QTimerEvent *)
{
    refreshWindows();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    gSettings.setValue("/Width", event->size().width());
    gSettings.setValue("/Height", event->size().height());
}

void MainWindow::on_actionEdit_triggered()
{
    //QProcess prc;
    //prc.start("xdg-open", QStringList() << pathMan.eventsFilePath());
    //prc.waitForFinished();
    EditWindow *pew = new EditWindow(0, pathMan.eventsFilePath());
    pew->exec();
    delete pew;

    qlEvents.clear();
    setLstEvents();
    refreshWindows();
}

void MainWindow::on_actionExit_triggered()
{

}
//Выбор параметра "Напоминать за Х дней"
void MainWindow::on_actionSettings_triggered()
{
    SettingsWindow *psw = new SettingsWindow(0, gDays);
    if(psw->exec() == QDialog::Rejected)
    {
        delete psw;
        return;
    }

    gDays = psw->getDays();
    if (gDays < 1 || gDays > 364) gDays = 14;
    //Сохраняем наш параметр в глобальных настройках
    gSettings.setValue("/Days", gDays);
    delete psw;

    qlDates.clear();
    setLstDates();

    qlEvents.clear();
    setLstEvents();

    refreshWindows();
}
