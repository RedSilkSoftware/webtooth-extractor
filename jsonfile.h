#ifndef JSONFILE_H
#define JSONFILE_H
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

#include <QObject>
#include <QFileInfo>
#include <QListWidget>

class JSONFile : public QObject
{
    Q_OBJECT
public:
    explicit JSONFile(QObject *parent = 0);
    ~JSONFile();

    typedef QHash<QString, QVector<QString>*> t_filterdata;
    bool readJsonFile(t_filterdata& filterlist, QHash<QString, QString>& projectdata, const QString &prjfilepath);
    bool writeJsonFile(const t_filterdata& filterlist, const QHash<QString, QString>& projectdata, const QString& prjfilepath);
    bool openJsonFile();
    QString getJsonFilePath(void); // Get Path
    qint64  getJsonFileSize(void); // Size in Bytes

private:
    bool checkJsonFile(); // Different QFileInfo checks on the file before write/creation


signals:

public slots:
};

#endif // JSONFILE_H
