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

#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include "csvfile.h"

CSVFile::CSVFile(QObject *parent) : QObject(parent)
{

}

CSVFile::~CSVFile()
{

}

bool CSVFile::WriteResultCSV(const QMap<QString, QString> &tbl_result, const QString& csvfilepath, const QString& csvSeparator)
{
    bool bRes = false;
    QFile csvFile(csvfilepath);
    if (csvFile.exists())
    {
        qDebug() << "Error: File" << csvfilepath << "does already exist!";
    }
    //else
    {
        //qDebug() << "File" << prjfilepath << "does not yet exists";
        if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Couldn't open the CSV file.";
        }
        else
        {
            if (!tbl_result.size() == 0)
            {
                 QTextStream outstream(&csvFile);
                 // Write header
                 outstream << "Data Field" << csvSeparator << "Value" << endline;
                 // Write all the content
                 QMap<QString, QString>::const_iterator it = tbl_result.constBegin();
                 QString tmpKey = "";
                 QString tmpValue = "";
                 bool isSpecialCase = false;
                 while (it != tbl_result.constEnd())
                 {
                     tmpKey = it.key();
                     tmpValue = it.value();
                     // KEY: Check if we need to enclose the text of each field with extra "
                     if (tmpKey.contains("\"")) // " in the field?
                     {
                         isSpecialCase = true;
                         // Search for all occurrences of "
                         for (int i=0; i < tmpKey.size(); i++)
                         {
                             if (tmpKey.indexOf('"', i) > -1) // -1 = not found
                             {
                                 i = tmpKey.indexOf('"', i);
                                tmpKey.insert(i+1, '"');
                             }
                         }
                     }
                     if (tmpKey.contains(",") || tmpKey.contains(endline)) // Comma or CRLF in the field?
                     {
                         isSpecialCase = true;
                         tmpKey = "\"" + tmpKey + "\"";
                     }
                     // VALUE: Check if we need to enclose the text of each field with extra "
                     if (tmpValue.contains("\"")) // " in the field?
                     {
                         isSpecialCase = true;
                         // Search for all occurrences of "
                         for (int i=0; i < tmpValue.size(); i++)
                         {
                             if (tmpValue.indexOf('"', i) > -1) // -1 = not found
                             {
                                 i = tmpValue.indexOf('"', i);
                                tmpValue.insert(i+1, '"');
                             }
                         }
                     }
                     if (tmpValue.contains(",") || tmpValue.contains(endline)) // Comma or CRLF in the field?
                     {
                         isSpecialCase = true;
                         tmpValue = "\"" + tmpValue + "\"";
                     }

                     // Special cases?
                     if (isSpecialCase)
                     {
                         outstream << tmpKey << csvSeparator << tmpValue << endline;
                     }
                     else
                     {
                        outstream << it.key() << csvSeparator << it.value() << endline;
                     }

                     tmpKey = "";
                     tmpValue = "";
                     isSpecialCase = false;
                     it++;
                 }
                 outstream << "Data from the WebTooth Extractor (c) by Patrick Scheller"  << csvSeparator << "2015" << endline;
                 outstream.flush(); // Write the data to the CSV file
                 bRes = true;
            }
            csvFile.close();
        }
    }
    return (bRes);
}

