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
#include "licensewindow.h"
#include "aboutwindow.h"

#include <QTextCodec>
#include <QDate>
#include <QDebug>
#include <QFileInfo>
#include <QColorDialog>
#include <QFontDialog>
#include <QSettings>
#include <QScreen>
#include <QFont>
#include <QDesktopServices>
#include <QProcess>
#include <QMessageBox>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), gSettings("Datasoft","TheirBirthday"),
    pathMan()
{
    ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    //задаём кастомное контекстное меню в полуокнах
    ui->plainTEditEvents->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plainTEditEvents, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuEvents(const QPoint&)));

    ui->plainTEditDates->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plainTEditDates, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuDates(const QPoint&)));
    //обрабатываем иконку трея
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/new/files/TheirBirthday.ico"));
    trayIcon->setToolTip("TheirBirthday");
    //контекстное меню трея
    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction(tr("Развернуть окно"), this);
    QAction * quitAction = new QAction(tr("Выход"), this);

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    //цвет выделения по умолчанию
    gColor = QColor(Qt::green).lighter(125);
    //цвет для 3 дней
    gColor3 = QColor(Qt::blue).lighter(175);
    //Напоминать за 14 дней по умолчанию
    gDays = gSettings.value("/Days", 14).toInt();
    //Разделитель для отображения
    gDelimiter = gSettings.value("/Delimiter", "/").toString();
    //Сворачивать в трей
    gTray = gSettings.value("/Tray", false).toBool();
    //мало ли чего там с сеттингов считалось...
    if (gDays < 1 || gDays > 364) gDays = 14;
    if (gDelimiter.length() != 1) gDelimiter = "/";
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

    int r3 = gSettings.value("/Red3", 0).toInt();
    int g3 = gSettings.value("/Green3", 0).toInt();
    int b3 = gSettings.value("/Blue3", 0).toInt();

    //устанавливаем цвет выделения
    if (r3 != 0 || g3 != 0 || b3 != 0)
        gColor3 = QColor::fromRgb(r3, g3, b3);
}

void MainWindow::setWindowSize()
{
    //считываем размеры окна
    int frmWidth = gSettings.value("/Width", 578).toInt();
    int frmHeight = gSettings.value("/Height", 363).toInt();

    //определяем размеры экрана
    QScreen* screen = QApplication::screens().at(0);
    QSize size = screen->availableSize();

    int iX = size.width()/2 - frmWidth/2;
    int iY = size.height()/2 - frmHeight/2;
    //проверяем на допустимость значений
    if (iX < 0) iX = 0;
    if (iY < 0) iY = 0;
    //устанавливаем размеры окна, в центре экрана
    this->setGeometry(iX , iY, frmWidth, frmHeight);
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
            QString st = "";
            if (iy > 0)
            {
                st = tr("Завтра ") + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина") + ")";
                sb += st + "\n";
            }
            else
            {
                st = tr("Завтра ") + fs.replace(sDate, "");
                sb += st;
            }

            ql3.append(st);
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
            QString st = "";
            QStringList slDayMonth = sDate.left(5).split("/");
            QString sLocale = QLocale::system().name();
            int iy = QDate::currentDate().year() - dDate.year();
            if (iy > 0)
            {
                if (sLocale == "en_US")
                {
                    //sb += tr("Через ") + QString::number(pdays) + tr(" дней (") + slDayMonth[1] + "/" + slDayMonth[0] + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")\n";
                    st = tr("Через ") + QString::number(pdays) + tr(" дней (") + slDayMonth[1] + gDelimiter + slDayMonth[0] + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")";
                }
                else
                    //sb += tr("Через ") + QString::number(pdays) + " " + getDaysStr(pdays) + " (" + sDate.left(5).replace("/", ".") + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")\n";
                    st = tr("Через ") + QString::number(pdays) + " " + getDaysStr(pdays) + " (" + sDate.left(5).replace("/", gDelimiter) + ") " + fs.replace(sDate, "").trimmed() + " (" + QString::number(iy) + tr(" годовщина")+")";
                sb += st + "\n";
            }
            else
            {
                if (sLocale == "en_US")
                {
                    st = tr("Через ") + QString::number(pdays) + tr(" дней (") + slDayMonth[1] + gDelimiter + slDayMonth[0] + ") " + fs.replace(sDate, "");
                }
                else
                    st = tr("Через ") + QString::number(pdays) + " " + getDaysStr(pdays) + " (" + sDate.left(5).replace("/", gDelimiter) + ") " + fs.replace(sDate, "");
                sb += st;
            }
            //заполняем список ql3
            if (pdays == 2 || pdays == 3)
                ql3.append(st);
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
    //подсвечиваем сегодняшние
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
    //подсвечиваем на 3 дня вперёд
    foreach(QString fs, ql3)
    {
        findCur = pte->document()->find(fs, cur);
        if(findCur != cur)
        {
            QTextEdit::ExtraSelection xtra;
            xtra.format.setBackground(gColor3);
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
        gSettings.setValue("/FontBold", fnt.bold()?fnt.Bold:-1);//без таких ухищрений Bold не работает
        ui->plainTEditDates->setFont(fnt);
        ui->plainTEditEvents->setFont(fnt);
    }
}
//Обновляем окно каждую минуту
void MainWindow::timerEvent(QTimerEvent *)
{
    refreshWindows();
}
//обрабатываем изменение размера окна
void MainWindow::resizeEvent(QResizeEvent *event)
{
    gSettings.setValue("/Width", event->size().width());
    gSettings.setValue("/Height", event->size().height());
}
//обрабатываем закрытие окна
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->isVisible() && gTray)
    {
        event->ignore();
        this->hide();
    }
}
//суть редактирования для обоих файлов, и Dates, и Events
void MainWindow::callDatesEventsFile(QList<QString>& pLst, QString pFilePath)
{
    ui->menuBar->hide();

    EditWindow *pew = new EditWindow(0, pFilePath);
    pew->exec();
    delete pew;

    ui->menuBar->show();

    pLst.clear();
}
//Редактировать
void MainWindow::on_actionEdit_triggered()
{
    //QProcess prc;
    //prc.start("xdg-open", QStringList() << pathMan.eventsFilePath());
    //prc.waitForFinished();
    callDatesEventsFile(qlEvents, pathMan.eventsFilePath());

    setLstEvents();
    refreshWindows();
}

void MainWindow::on_actionExit_triggered()
{

}
//Выбор параметров "Напоминать за Х дней" и "Разделитель"
void MainWindow::on_actionSettings_triggered()
{
    SettingsWindow *psw = new SettingsWindow(0, gDays, gDelimiter, gTray);
    if(psw->exec() == QDialog::Rejected)
    {
        delete psw;
        return;
    }

    gDays = psw->getDays();
    if (gDays < 1 || gDays > 364) gDays = 14;
    //Сохраняем наш параметр в глобальных настройках
    gSettings.setValue("/Days", gDays);

    gDelimiter = psw->getDelimiter();
    if (gDelimiter.length() != 1) gDelimiter = "/";
    gSettings.setValue("/Delimiter", gDelimiter);

    gTray = psw->getTray();
    gSettings.setValue("/Tray", gTray);
    delete psw;

    qlDates.clear();
    setLstDates();

    qlEvents.clear();
    setLstEvents();

    refreshWindows();
}

//пункт меню "О Qt"
void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0);
}
//Выводим нашу лицензию
void MainWindow::on_actionLicense_triggered()
{
    LicenseWindow *plw = new LicenseWindow;

    plw->exec();

    delete plw;
}
//Окно "О программе"
void MainWindow::on_actionAbout_triggered()
{
    AboutWindow * paw = new AboutWindow;

    paw->exec();

    delete paw;
}
//Выбираем цвет 3
void MainWindow::on_actionColor3_triggered()
{
    QColor tempColor = QColorDialog::getColor(gColor3);
    if (tempColor.isValid())
    {
        gColor3 = tempColor;
        gSettings.setValue("/Red3", tempColor.red());
        gSettings.setValue("/Green3", tempColor.green());
        gSettings.setValue("/Blue3", tempColor.blue());

        refreshWindows();
    }
}
//Показываем кастомное контекстное меню для событий
void MainWindow::showContextMenuEvents(const QPoint& pos)
{
    QPoint globalPos = ui->plainTEditEvents->mapToGlobal(pos);
    QMenu myMenu;

    QAction* copyAction = new QAction(tr("Копировать"), this);
    QAction* selectAllAction = new QAction(tr("Выделить всё"), this);
    QAction* editAction = new QAction(tr("Редактировать..."), this);

    myMenu.addAction(copyAction);
    myMenu.addAction(selectAllAction);
    myMenu.addAction(editAction);

    connect(copyAction, &QAction::triggered, this, &MainWindow::showContextMenuEventsCopy);
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::showContextMenuEventsSelectAll);
    connect(editAction, &QAction::triggered, this, &MainWindow::editContextMenuEvents);

    QAction* selectedItem = myMenu.exec(globalPos);
}
//Показываем кастомное контекстное меню для дат
void MainWindow::showContextMenuDates(const QPoint& pos)
{
    QPoint globalPos = ui->plainTEditDates->mapToGlobal(pos);
    QMenu myMenu;

    QAction* copyAction = new QAction(tr("Копировать"), this);
    QAction* selectAllAction = new QAction(tr("Выделить всё"), this);
    QAction* editAction  = new QAction(tr("Редактировать..."), this);

    myMenu.addAction(copyAction);
    myMenu.addAction(selectAllAction);
    myMenu.addAction(editAction);

    connect(copyAction, &QAction::triggered, this, &MainWindow::showContextMenuDatesCopy);
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::showContextMenuDatesSelectAll);
    connect(editAction, &QAction::triggered, this, &MainWindow::editContextMenuDates);

    QAction* selectedItem = myMenu.exec(globalPos);
}
//Обрабатываем редактирование событий
void MainWindow::editContextMenuEvents()
{
    on_actionEdit_triggered();
}
//Обрабатываем редактирование дат
void MainWindow::editContextMenuDates()
{
    callDatesEventsFile(qlDates, pathMan.datesFilePath());
    setLstDates();
    refreshWindows();
}
//пользовательские контекстные меню
void MainWindow::showContextMenuDatesCopy()
{
    ui->plainTEditDates->copy();
}

void MainWindow::showContextMenuDatesSelectAll()
{
    ui->plainTEditDates->selectAll();
}

void MainWindow::showContextMenuEventsCopy()
{
    ui->plainTEditEvents->copy();
}

void MainWindow::showContextMenuEventsSelectAll()
{
    ui->plainTEditEvents->selectAll();
}
//обрабатываем нажатие на иконку в трее
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        if(gTray)
        {
            if(!this->isVisible())
                this->show();
            else
                this->hide();
        }
        break;
    default:
        break;
    }
}
