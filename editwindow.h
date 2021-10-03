/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QDialog>
#include <QMenuBar>
#include <QAbstractButton>
#include <QShortcut>
#include <QTextCursor>

namespace Ui {
class EditWindow;
}

class EditWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent, const QString& fPath);
    ~EditWindow();
protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
signals:

private:
    Ui::EditWindow *ui;
    void loadFile();
    void setWindowFont();
    QString gFileName;
    QShortcut       *keyCtrlPlus;    // объект горячей клавиши Ctrl + +
    QShortcut       *keyCtrlMinus;  // объект сочетания клавиш Ctrl + -
    QTextCursor findWord();
    QTextCursor curPos;
    QString sWord;
    void moveCursor(int, int);
    void setSelection(QTextCursor &findCur);
};

#endif // EDITWINDOW_H
