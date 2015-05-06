#ifndef PROPERTIES_H
#define PROPERTIES_H

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
#include <QUrl>

namespace Ui {
class Properties;
}

class Properties : public QDialog
{
    Q_OBJECT

public:
    QHash<QString, QString> hProjectData;
    explicit Properties(QWidget *parent = 0);
    ~Properties();

    void LoadData(void);
    bool isDirtyProp();
    bool unsetDirtyProp(); // Reset to not dirty
    void ResetProperties(); // Reset all properties to default values

    QString getSUrlPath() const;
    void setSUrlPath(const QString &value);

    QString getSAuthorName() const;
    void setSAuthorName(const QString &value);

    QString getSUserNotes() const;
    void setSUserNotes(const QString &value);

    QString getSLastModificationDate() const;
    void setSLastModificationDate(const QString &value);

    bool getBCropThumb() const;
    void setBCropThumb(bool value);

    qint16 getThumbHeight() const;
    void setThumbHeight(const qint16 &value);

    qint16 getThumbWidth() const;
    void setThumbWidth(const qint16 &value);

    QString getCsvSeparator() const;
    void setCsvSeparator(const QString &value);


private slots:
    void on_buttonBox_accepted();

    void on_toolButton_clicked();

    void on_checkBoxCropThumb_toggled(bool checked);

    void on_buttonBox_rejected();

private:
    Ui::Properties *ui;
    QString sUrlPath;
    QString sAuthorName;
    QString sUserNotes;
    QString sLastModificationDate;
    qint16 thumbWidth;
    qint16 thumbHeight;
    bool bCropThumb;
    QString sAuthor;
    QString sProjectNotes;
    QUrl uProjectUrl;
    QString csvSeparator;
    bool isDirtyPropData;
};

#endif // PROPERTIES_H
