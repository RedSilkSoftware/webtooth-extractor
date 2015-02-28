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

#include <QDebug>
#include <QWebPage>
#include "aboutdialog.h"
#include "ui_aboutdialog.h"


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog),
    qtWebKitVersion("")
{
    ui->setupUi(this);
    sys_CPU = QSysInfo::currentCpuArchitecture();
    sys_KernelType = QSysInfo::kernelType();
    sys_KernelVersion = QSysInfo::kernelVersion();
    sys_prettyProductName = QSysInfo::prettyProductName();
    sys_productType = QSysInfo::productType();
    sys_productVersion = QSysInfo::productVersion();

    // Fill the fields with versions and system informations
    format_WTE();
    format_QT();
    format_SYS();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
QString AboutDialog::getSys_CPU() const
{
    return sys_CPU;
}
QString AboutDialog::getSys_KernelType() const
{
    return sys_KernelType;
}
QString AboutDialog::getSys_KernelVersion() const
{
    return sys_KernelVersion;
}
QString AboutDialog::getSys_prettyProductName() const
{
    return sys_prettyProductName;
}
QString AboutDialog::getSys_productType() const
{
    return sys_productType;
}
QString AboutDialog::getSys_productVersion() const
{
    return sys_productVersion;
}

void AboutDialog::format_WTE()
{
    wte_Begin = "<html><head/><body><p>This is version: " + wte_Version;
    wte_End = "</p><p><span style=\" font-weight:600;\">WebTooth-Extractor</span> is the baby of<span style=\" font-weight:600;\"> Patrick Scheller</span> and was created out of a necessity and has quickly grown (beyond proportions) into a generic web-field scrapper. I hope it is useful to you and that you may either contribute to the project in some way, make a donation or buy the commercial license so you can use it for your business. <a href=\"http://redsilksoftware.github.io/webtooth-extractor/\"><span style=\" text-decoration: underline; color:#0000ff;\">Home of WebTooth</span></a></p></body></html>";
    ui->label_WebTooth->setText(wte_Begin + wte_End);
}

void AboutDialog::format_QT()
{    qt_Begin = "<html><head/><body><p>This software was written with the Qt library Version " + qt_Version;
    qt_End = ". <a href=\"http://qt-project.org/\"><span style=\" text-decoration: underline; color:#0000ff;\">Link to the Qt-Project website</span></a></p></body></html>";
    ui->label_QT->setText(qt_Begin + qt_End);
}

void AboutDialog::format_SYS()
{
    //qDebug() << "sys_productVersion" << sys_productVersion;
    //qDebug() << "sys_productType" << sys_productType;
//    qDebug() << "sys_prettyProductName" << sys_prettyProductName;
//    qDebug() << "sys_KernelVersion" << sys_KernelVersion;
//    qDebug() << "sys_KernelType" << sys_KernelType;
//    qDebug() << "sys_CPU" << sys_CPU;
    sys_Partial = "<html><head/><body><p><span style=\" text-decoration: underline;\">Information about your system that you can use for feedback and bug-reports:</span></p><p>Operating System Version: " + sys_prettyProductName;
    sys_Partial += "</p><p>Kernel Type + Version: " + sys_KernelType + ", " + sys_KernelVersion;
    sys_Partial += "</p><p>CPU Architecture: " + sys_CPU;
    sys_Partial += "<br/></p></body></html>";
    ui->label_SysInfos->setText(sys_Partial);
}

// The QWebKit Version-String won't display in the About Dialog :(
void AboutDialog::setQtWebKitVersion(const QString &value)
{
    qtWebKitVersion = value;
    //qDebug() << "qtWebKitVersion:" << qtWebKitVersion;
}




