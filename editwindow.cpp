/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "editwindow.h"
#include "ui_editwindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTextCodec>
#include <QSettings>
#include <QTextStream>
#include <Qt>
#include <QInputDialog>

EditWindow::EditWindow(QWidget *parent, const QString& fPath) :
    QDialog(parent),
    ui(new Ui::EditWindow),
    gFileName(fPath)
{
    ui->setupUi(this);
    keyCtrlPlus = new QShortcut(this); // Инициализируем объект
    keyCtrlPlus->setKey(Qt::CTRL + Qt::Key_Plus); // Устанавливаем сочетание клавиш
    connect(keyCtrlPlus, SIGNAL(activated()), ui->plainTextEdit, SLOT(zoomIn()));
    keyCtrlMinus = new QShortcut(this); // Инициализируем объект
    keyCtrlMinus->setKey(Qt::CTRL + Qt::Key_Minus); // Устанавливаем сочетание клавиш
    connect(keyCtrlMinus, SIGNAL(activated()), ui->plainTextEdit, SLOT(zoomOut()));

    loadFile();
    setWindowFont();
}

EditWindow::~EditWindow()
{
    delete ui;
}
//загружаем файл на редактирование
void EditWindow::loadFile()
{
    QFile fl(gFileName);
    if (fl.open(QIODevice::ReadOnly))
    {
        QByteArray sByteArray = fl.readAll();
        QTextCodec *codec = QTextCodec::codecForName("CP1251");
        QString sFileBody = codec->toUnicode(sByteArray);
        ui->plainTextEdit->setPlainText(sFileBody);
        fl.close();
    }
}
//Устанавливаем шрифт и его размер
void EditWindow::setWindowFont()
{
    QSettings settings("TheirBirthdaySoft","TheirBirthday");
    QString fntFamily = settings.value("/Font", "Arial").toString();
    int fntSize = settings.value("/FontSize", 8).toInt();
    //устанавливаем шрифт в окне
    QFont fnt(fntFamily, fntSize, -1, false);
    ui->plainTextEdit->setFont(fnt);
}
//На нажатие ОК
void EditWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    //сохраняем файл
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
    {
        QFile fl(gFileName);
        if (fl.open(QIODevice::WriteOnly))
        {
            QTextStream fileStream(&fl);
            fileStream.setCodec("CP1251");
            fileStream << ui->plainTextEdit->toPlainText();
            if (fileStream.status() != QTextStream::Ok)
                qDebug() << tr("Ошибка записи файла");
            fl.close();
        }
    }
}
//перемещение курсора на найденное
void EditWindow::moveCursor(int numBlock, int numCol)
{
    QTextCursor findCur = ui->plainTextEdit->textCursor();

    findCur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, numBlock);
    findCur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, numCol);

    ui->plainTextEdit->setTextCursor(findCur);
}

void EditWindow::setSelection(QTextCursor &findCur)
{
    QList<QTextEdit::ExtraSelection> lSel;

    QTextEdit::ExtraSelection xtra;
    xtra.format.setBackground(QColor(Qt::green).lighter(125));
    xtra.cursor = findCur;
    lSel.append(xtra);
    ui->plainTextEdit->setExtraSelections(lSel);
}
//Найти
QTextCursor EditWindow::findWord()
{
    curPos = ui->plainTextEdit->document()->find(sWord, curPos);

    if (curPos.isNull()) return curPos;

    int numBlock = curPos.blockNumber();
    int numCol = curPos.columnNumber() - sWord.length();

    setSelection(curPos);
    moveCursor(numBlock, numCol);
    return curPos;
}

void EditWindow::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_F:
        if (ev->modifiers() & Qt::ControlModifier)
        {
            bool bOk;

            ui->plainTextEdit->moveCursor(QTextCursor::Start);
            curPos = ui->plainTextEdit->textCursor();

            sWord = QInputDialog::getText(NULL, tr("Поиск"), tr("Найти:"), QLineEdit::Normal, "", &bOk);
            if(!bOk)
                return;

            findWord();
        }
        break;
      case Qt::Key_F3:
        findWord();
      break;
    }
}

