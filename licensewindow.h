/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef LICENSEWINDOW_H
#define LICENSEWINDOW_H

#include <QDialog>
#include <QTextCursor>

namespace Ui {
class LicenseWindow;
}

class LicenseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseWindow(QWidget *parent = nullptr);
    ~LicenseWindow();

private:
    Ui::LicenseWindow *ui;
};

#endif // LICENSEWINDOW_H
