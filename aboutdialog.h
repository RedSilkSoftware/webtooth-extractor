#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

/*
This file is part of WebTooth-Extractor and for personal use the
GPL3 license applies. For any kind of commercial usage, you have to
acquire a commercial license before use. The copyright of the WebTooth
application and source code remains with Patrick Scheller and any
contributor to the project agrees with his commits to renounce to the
copyright of his parts completely.

WebTooth-Extractor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License.

WebTooth-Extractor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WebTooth-Extractor. If not, see http://www.gnu.org/licenses/ .
*/

#include <QDialog>

const QString qt_Version = QT_VERSION_STR;
const QString wte_Version = "0.5 alpha";

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    QString getSys_CPU() const;
    QString getSys_KernelType() const;
    QString getSys_KernelVersion() const;
    QString getSys_prettyProductName() const;
    QString getSys_productType() const;
    QString getSys_productVersion() const;

    void setQtWebKitVersion(const QString &value);

private:
    void format_WTE(void);
    void format_QT(void);
    void format_SYS(void);

    Ui::AboutDialog *ui;
    QString sys_CPU;
    QString sys_KernelType;
    QString sys_KernelVersion;
    QString sys_prettyProductName;
    QString sys_productType;
    QString sys_productVersion;

    // Text fields to fill
    QString wte_Begin;
    QString wte_End;
    QString qt_Begin;
    QString qt_End;
    QString sys_Partial;
    QString qtWebKitVersion;

};

#endif // ABOUTDIALOG_H
