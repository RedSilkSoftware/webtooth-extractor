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

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QtDebug>
#include <QDir>
#include <QListWidgetItem>
#include <QDateTime>

#include <qtpropertymanager.h>
#include <qttreepropertybrowser.h>


#include "jsonfile.h"

JSONFile::JSONFile(QObject *parent) : QObject(parent)
{

}

JSONFile::~JSONFile()
{

}

bool JSONFile::readJsonFile(t_filterdata& filterlist, QHash<QString, QString>& projectdata, const QString &prjfilepath)
{
    bool bRes = false;

    QJsonParseError jerror;
    QFile projFile(prjfilepath);
    if (!projFile.exists())
    {
        qDebug() << "Error: File" << prjfilepath << "does not exist!";
        return(bRes);
    }
    else
    {
        //qDebug() << "File" << prjfilepath << "exists";
    }
    QJsonDocument jdoc;
    if (projFile.open(QIODevice::ReadOnly))
    {
        //qDebug() << "JSON-File could be opened";
        jdoc = QJsonDocument::fromJson(projFile.readAll(),&jerror);
        if(jerror.error != QJsonParseError::NoError)
        {
            qDebug() << "ERROR-JSON-Read:" << jerror.errorString() << "at position:" << jerror.offset;
            projFile.close();
            return (bRes);
        }
        else
        {
            //qDebug() << "QJsonDocument::fromJson() succeeded";
        }
    }
    else
    {
        qDebug() << "ERROR-JSON: File could not be opened";
        projFile.close();
        return(bRes);
    }
    QJsonObject rootObj = jdoc.object();
    projFile.close();

    //QHashIterator<QString, QString> itPrjData(projectdata);
    projectdata.insert("projectapp", "WebTooth-Extractor");
    projectdata.insert("projectname", rootObj["projectname"].toString());
    projectdata.insert("projectauthor", rootObj["projectauthor"].toString());
    projectdata.insert("projectmodified", rootObj["projectmodified"].toString());
    projectdata.insert("cropthumb", rootObj["cropthumb"].toString());
    projectdata.insert("projecnotes", rootObj["projecnotes"].toString());
    projectdata.insert("projecturl", rootObj["projecturl"].toString());
    projectdata.insert("thumbheight", rootObj["thumbheight"].toString());
    projectdata.insert("thumbwidth", rootObj["thumbwidth"].toString());

    // typedef QHash<QString, QVector<QString>*> t_filterdata;
    QJsonArray filters = rootObj["projectdata"].toArray();
    for(auto&& filterItem: filters)
    {
        //qDebug() << "XXX - filterItem";
        const QJsonObject& filter = filterItem.toObject();
        //filterlist.insert("filtername", new QVector<QString>(){filter["filtername"].toString()} );
        //filterlist.insert("filtertype", new QVector<QString>(){filter["filtertype"].toString()} );

        QJsonArray arrProperties = filter["filterdata"].toArray();
        QVector<QString>* vattrlist= new QVector<QString>();
        for (auto&& item: arrProperties)
        {
            const QJsonObject& attrProp = item.toObject();
            if (filter["filtertype"].toString() == "tag")
            {
                //qDebug() << "Filtertype:" << filter["filtertype"].toString();
                vattrlist->push_back(filter["filtertype"].toString());
                vattrlist->push_back(attrProp["H Tag Name"].toString());
                vattrlist->push_back(attrProp["H Attribute Name"].toString());
                vattrlist->push_back(attrProp["H ID Name"].toString());
                vattrlist->push_back(attrProp["H Class Name"].toString());
                vattrlist->push_back(attrProp["P Tag Name"].toString());
                vattrlist->push_back(attrProp["P Attribute Name"].toString());
                vattrlist->push_back(attrProp["P ID Name"].toString());
                vattrlist->push_back(attrProp["P Class Name"].toString());
                vattrlist->push_back(attrProp["Occurrence Amount"].toString());
                vattrlist->push_back(attrProp["Occurrence Function"].toString());
            }
            else if (filter["filtertype"].toString() == "reg")
            {
                //qDebug() << "XXX - RegExpr";
                vattrlist->push_back(filter["filtertype"].toString());
                vattrlist->push_back(attrProp["Regular Expression"].toString());
            }
        }
        filterlist.insert(filter["filtername"].toString(), vattrlist);
        //qDebug() << "YYY - End of JSON-Read";
        bRes = true;
    }
    return(bRes);
}

bool JSONFile::writeJsonFile(const t_filterdata& filterlist, const QHash<QString, QString>& projectdata, const QString &prjfilepath)
{
    qDebug() << "Entering writeJsonFile";
    bool bRes = false;
    QDir dir;
    qDebug() << "Current Path: " << dir.currentPath() << flush;
    //QFileInfo myfileInfo;
    QFile projFile(prjfilepath);
    if (!projFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open the project file.");
        return(bRes);
    }

    // *******************************
    QJsonObject rootObj;//root object
    QHashIterator<QString, QString> itPrjData(projectdata);
    while (itPrjData.hasNext())
    {
        itPrjData.next();
        rootObj[itPrjData.key()] = itPrjData.value();
    }
    //rootObj["projectapp"] = "WebTooth-Extractor";
    //rootObj["projectname"] = "usertest";
    //rootObj["projectauthor"] =  "Lunix Sdlavrot";
    //QDateTime now = QDateTime::currentDateTime();
    //rootObj["projectmodified"] = now.toString(Qt::ISODate);

    QJsonArray filters;
    auto itFList = filterlist.constBegin();
    while (itFList != filterlist.constEnd())
    {
        QJsonObject filter;
        filter["filtername"] = itFList.key();
        auto myvec = itFList.value();
        filter["filtertype"] = myvec->at(0);

        QJsonArray arrProperties;
        QJsonObject objProperties;
        if (myvec->at(0) == "tag")
        {
            objProperties["H Tag Name"] = myvec->at(1);
            objProperties["H Attribute Name"] = myvec->at(2);
            objProperties["H ID Name"] = myvec->at(3);
            objProperties["H Class Name"] = myvec->at(4);

            objProperties["P Tag Name"] = myvec->at(5);
            objProperties["P Attribute Name"] = myvec->at(6);
            objProperties["P ID Name"] = myvec->at(7);
            objProperties["P Class Name"] = myvec->at(8);

            objProperties["Occurrence Amount"] = myvec->at(9);
            objProperties["Occurrence Function"] = myvec->at(10);
        }
        else if (myvec->at(0) == "reg")
        {
            objProperties["Regular Expression"] = myvec->at(1);
        }
        arrProperties.append(objProperties);
        filter["filterdata"] = arrProperties;

        filters.append(filter);
        itFList++;

    }
    rootObj["projectdata"] = filters;

    //qDebug() << "writeJsonFile: Before write to file.";
    qint64 byteswritten = projFile.write( QJsonDocument(rootObj).toJson(/*QJsonDocument::Compact*/) );
    //qDebug() << "writeJsonFile: After write to file.";
    projFile.close();

    if (byteswritten == -1)
    {
        qDebug() << "No Bytes written to the JsonFile :(";
        qWarning("Error: Could not write to the project file.");
    }
    else
    {
        qDebug() << byteswritten << "Bytes have been written to the file";
        bRes = true;
    }

    return(bRes);
}

bool JSONFile::openJsonFile()
{
    bool bRes = false;
    return(bRes);
}

QString JSONFile::getJsonFilePath(void)
{
    QString sRes = "";
    return(sRes);
}

qint64  JSONFile::getJsonFileSize(void)
{
    qint64 iRes = 0;
    return(iRes);
}

bool JSONFile::checkJsonFile()
{
    bool bRes = false;
    return(bRes);
}
