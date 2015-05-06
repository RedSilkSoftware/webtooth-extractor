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

#include <QStandardPaths>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

#include "properties.h"
#include "ui_properties.h"

Properties::Properties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Properties),
    sUrlPath(""),
    bCropThumb(true),
    thumbHeight(500),
    thumbWidth(300),
    sAuthor(""),
    sProjectNotes(""),
    uProjectUrl(""),
    sLastModificationDate(""),
    csvSeparator(",")
{
    ui->setupUi(this);

    QDateTime now = QDateTime::currentDateTime();
    //sLastModificationDate = now.toString(Qt::ISODate);
    //ui->lineEditLastModificationDate->setText(sLastModificationDate);
    hProjectData.insert("projectapp", "WebTooth-Extractor"); // Name of this application
    hProjectData.insert("projectname", ""); // Project-File-Name
    hProjectData.insert("projectauthor", "");
    hProjectData.insert("projectmodified", ""); // Date of last saved modification
    hProjectData.insert("projecnotes", "");
    hProjectData.insert("projecturl", "");
    hProjectData.insert("cropthumb", "true"); // true or falsse
    hProjectData.insert("thumbwidth", "300"); // in pixels
    hProjectData.insert("thumbheight", "500"); // in pixels
    hProjectData.insert("csvseparator", ","); // in pixels
}

Properties::~Properties()
{
    delete ui;
}

// Save-Button
void Properties::on_buttonBox_accepted()
{
    // SetDirty if user has entered/modified properties
    if (this->sUrlPath != ui->lineEdit_URL->text())
    {
        this->sUrlPath = ui->lineEdit_URL->text();
        this->isDirtyPropData = true;
    }
    if (bCropThumb != ui->checkBoxCropThumb->isChecked())
    {
        bCropThumb = ui->checkBoxCropThumb->isChecked();
        this->isDirtyPropData = true;
    }
    if (thumbHeight != ui->spinBoxThumbHeight->value())
    {
        thumbHeight = ui->spinBoxThumbHeight->value();
        this->isDirtyPropData = true;
    }
    if (thumbWidth != ui->spinBoxThumbWidth->value())
    {
        thumbWidth = ui->spinBoxThumbWidth->value();
        this->isDirtyPropData = true;
    }
    if (sAuthor != ui->lineEditAuthorName->text())
    {
        sAuthor = ui->lineEditAuthorName->text();
        this->isDirtyPropData = true;
    }
    if (sProjectNotes != ui->plainTextEditUserNotes->toPlainText())
    {
        sProjectNotes = ui->plainTextEditUserNotes->toPlainText();
        this->isDirtyPropData = true;
    }
    if (csvSeparator != ui->lineEditCSVSeparator->text())
    {
        csvSeparator = ui->lineEditCSVSeparator->text();
        this->isDirtyPropData = true;
    }

    if (this->isDirtyPropData)
    {
        hProjectData["projectauthor"] = sAuthor;
        //qDebug() << "Author has been modified:" << hProjectData["projectauthor"];
        hProjectData["projecnotes"] = sProjectNotes;
        hProjectData["projecturl"] = sUrlPath;
        hProjectData["cropthumb"] = (bCropThumb ? "true" : "false");
        hProjectData["thumbwidth"] = QString::number(thumbWidth);
        hProjectData["thumbheight"] = QString::number(thumbHeight);
        hProjectData["csvseparator"] = csvSeparator;
    }
}

// Load after an OpenFile
void Properties::LoadData(void)
{
    sAuthor = hProjectData["projectauthor"];
    sProjectNotes = hProjectData["projecnotes"];
    sUrlPath = hProjectData["projecturl"];
    bCropThumb = (hProjectData["cropthumb"] == "true" ? true : false);
    thumbWidth = hProjectData["thumbwidth"].toShort();
    thumbHeight = hProjectData["thumbheight"].toShort();
    csvSeparator = hProjectData["csvseparator"];
    sLastModificationDate = hProjectData["projectmodified"];
    //qDebug() << "Properties:" << sAuthor << ", " << sProjectNotes << ", " << sUrlPath << ", " << (bCropThumb ? "true" : "false") << ", "
    //         << QString::number(thumbWidth) << ", " << QString::number(thumbHeight);

    ui->lineEdit_URL->setText(this->sUrlPath);
    ui->checkBoxCropThumb->setChecked(this->bCropThumb);
    ui->spinBoxThumbHeight->setValue(this->thumbHeight);
    ui->spinBoxThumbWidth->setValue(this->thumbWidth);
    ui->lineEditAuthorName->setText(this->sAuthor);
    ui->plainTextEditUserNotes->setPlainText(this->sProjectNotes);
    ui->lineEditCSVSeparator->setText(this->csvSeparator);
    ui->lineEditLastModificationDate->setText(this->sLastModificationDate);
}

void Properties::on_toolButton_clicked()
{
    // Call the URL-FileDialog
    QUrl homePath(QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory));
    QUrl urlpath = QFileDialog::getOpenFileUrl(this, "Open URL Or Local HTML File", homePath, "HTML File (*.html *.htm);;Text Files (*.txt);;All Files (*.*)");

    if (!urlpath.isEmpty())
        ui->lineEdit_URL->setText(urlpath.toString());

}
qint16 Properties::getThumbWidth() const
{
    return thumbWidth;
}

void Properties::setThumbWidth(const qint16 &value)
{
    thumbWidth = value;
}

qint16 Properties::getThumbHeight() const
{
    return thumbHeight;
}

void Properties::setThumbHeight(const qint16 &value)
{
    thumbHeight = value;
}

bool Properties::getBCropThumb() const
{
    return bCropThumb;
}

void Properties::setBCropThumb(bool value)
{
    bCropThumb = value;
}

QString Properties::getSLastModificationDate() const
{
    return sLastModificationDate;
}

void Properties::setSLastModificationDate(const QString &value)
{
    sLastModificationDate = value;
    ui->lineEditLastModificationDate->setText(sLastModificationDate);
}

QString Properties::getSUserNotes() const
{
    return sUserNotes;
}

void Properties::setSUserNotes(const QString &value)
{
    sUserNotes = value;
}

QString Properties::getSAuthorName() const
{
    return sAuthorName;
}

void Properties::setSAuthorName(const QString &value)
{
    sAuthorName = value;
}

QString Properties::getSUrlPath() const
{
    return sUrlPath;
}

void Properties::setSUrlPath(const QString &value)
{
    sUrlPath = value;
}


void Properties::on_checkBoxCropThumb_toggled(bool checked)
{
    if (checked)
    {
        //ui->spinBoxThumbHeight->setReadOnly(false);
        ui->spinBoxThumbHeight->setEnabled(true);
    }
    else
    {
        //ui->spinBoxThumbHeight->setReadOnly(true);
        ui->spinBoxThumbHeight->setEnabled(false);
    }
}
QString Properties::getCsvSeparator() const
{
    return csvSeparator;
}

void Properties::setCsvSeparator(const QString &value)
{
    csvSeparator = value;
}

bool Properties::isDirtyProp()
{
    return (this->isDirtyPropData);
}

bool Properties::unsetDirtyProp()
{
    this->isDirtyPropData = false;
    return (this->isDirtyPropData);
}

void Properties::ResetProperties()
{
    this->sUrlPath = "";
    this->bCropThumb = true;
    this->thumbHeight = 500;
    this->thumbWidth = 300;
    this->sAuthor = "";
    this->sProjectNotes = "";
    this->csvSeparator = ",";
    this->sLastModificationDate = "";

    this->on_buttonBox_rejected();
}

// If Cancel/Rejected then reset the previous values in the GUI
void Properties::on_buttonBox_rejected()
{
    ui->lineEdit_URL->setText(this->sUrlPath);
    ui->checkBoxCropThumb->setChecked(this->bCropThumb);
    ui->spinBoxThumbHeight->setValue(this->thumbHeight);
    ui->spinBoxThumbWidth->setValue(this->thumbWidth);
    ui->lineEditAuthorName->setText(this->sAuthor);
    ui->plainTextEditUserNotes->setPlainText(this->sProjectNotes);
    ui->lineEditCSVSeparator->setText(this->csvSeparator);
}


