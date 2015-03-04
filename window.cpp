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

#include <QtWidgets>
#include <QtWebKitWidgets>
#include <thread>
#include "window.h"
#include <QStandardPaths>
#include <climits>
#include <QWebSettings>

/*
#include "qtpropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
*/
#include <qtpropertymanager.h>
#include <qteditorfactory.h>
#include <qttreepropertybrowser.h>

#include "jsonfile.h"
#include "csvfile.h"
//#include "properties.h"


Window::Window(QWidget *parent)
    : QMainWindow(parent),
      sHtml(""),
      isFirstCall(true),
      searchRegExp(""),
      strCurrentFilter(""),
      strCurrentProjectFileName(""),
      isDirtyData(false),
      propDialog(this),
      aboutDialog(this),
      webZoomFactor(0.0),
      insp(nullptr)
{
    setupUi(this);
    statusBar()->showMessage("Ready");
    setupPropertyBrowsers();
    listWidgetFilters->setSortingEnabled(true);
    insp = new QWebInspector(this);
    dlg = new QDialog(); // Dialog for the Webinspector
    webView->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    //dockWidgetInspector->setLayout(new QVBoxLayout());
    //dockWidgetInspector->layout()->addWidget(insp);
    this->verticalLayout_11->addWidget(insp);
    //dockWidgetInspector->close();

    // Configure the browser through QWebSettings!
    //QWebSettings* pwebsettings = QWebSettings::​globalSettings();
    webZoomFactor = webView->zoomFactor(); // 1 = 100%
    qDebug() << "ZoomFactor of WebView: " << webZoomFactor;
    QString tmpwebkitversion = QTWEBKIT_VERSION_STR;
    aboutDialog.setQtWebKitVersion(tmpwebkitversion);
    labelPropertyError->hide();
    tableWidgetStats->setHorizontalHeaderLabels(QStringList() << "Data Field" << "Value");

    connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(on_widgetPropertyBrowser_valueChanged(QtProperty*, const QString&)));
    connect(intManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(on_widgetPropertyBrowser_IntValueChanged(QtProperty*, int)));
    connect(enumManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(on_widgetPropertyBrowser_EnumValueChanged(QtProperty*, int)));
    //connect(stringManager, SIGNAL(​editingFinished()), this, SLOT(on_stringEdit_editingFinished()));
}


void threadLoadUrl(QWebView* mywin, QUrl url)
{
    mywin->setUrl(url);
}


void Window::setUrl(const QUrl &url)
{    
    currentUrl = url;
    // Only call at startup
    if(isFirstCall)
    {
        this->originalUrl = url;
        isFirstCall = false;
    }
    this->webView->setUrl(url);
    //std::thread worker_thread(threadLoadUrl, this->webView, currentUrl);
    //worker_thread.join();
}

void Window::examineChildElements(const QWebElement &parentElement, QTreeWidgetItem *parentItem)
{
    QWebElement element = parentElement.firstChild();
    parentItem->treeWidget()->setHeaderLabel("DOM Tree with Tags and Attributes");
    QString tmpInfo = "";
    while (!element.isNull())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        if (element.attributeNames().isEmpty())
        {
            //qDebug() << "element.attributeNames().isEmpty() = true";
            tmpInfo = "<" + element.tagName() + ">";
        }
        else
        {
            //qDebug() << "element.attributeNames().isEmpty() = false !";
            tmpInfo = "<" + element.tagName() + ">" + " (";
            tmpInfo += element.attributeNames().at(0); // First attribute doesn't need a leading comma
            for (int i = 1; i < element.attributeNames().size(); i++)
            {
                tmpInfo += ", " + element.attributeNames().at(i);
            }
            tmpInfo += ")";
        }
        item->setText(0, tmpInfo);
        QString tmpTooltip = "";
        tmpTooltip = element.toPlainText();
        if (tmpTooltip.size() > 500)
        {
            tmpTooltip.resize(500);
            tmpTooltip += " .... >> CUT >>";
        }
        if (tmpTooltip.isEmpty()) // No content
        {
            item->setBackgroundColor(0, QColor(220, 220, 230));
        }
        item->setToolTip(0, tmpTooltip);
        parentItem->addChild(item);

        examineChildElements(element, item);
        element = element.nextSibling();
    }
}


//
void Window::processWebFilters(const QWebElement& parentElement)
{
    int elemCounter = 0;
    qDebug() << "Enter processWebFilters";
    // Process all HTML-Tag-Filters with every WebElement
    this->scanAllWebElements(parentElement, elemCounter);
    qDebug() << "The Webpage has " << elemCounter << "elements";

    // Process all RegExp-Filters with the complete HTML-String
    const QString htmlsource = webView->page()->mainFrame()->toHtml();
    auto itFList = this->filterData.constBegin();
    while (itFList != filterData.constEnd())
    {
        qDebug() << "Processing RegExp-Filter: " << itFList.key();
        auto myvec = itFList.value();
        if (myvec->at(0) == "reg") // Filtertype = RegExp
        {
            if (!myvec->at(1).isEmpty())
            {
                // Search RegExp seperately within the HTML source
                qDebug() << "Searching for the RegExp: " << myvec->at(1) << " within the HTML source";
                QRegularExpression myreg;
                myreg.setPattern(myvec->at(1));
                myreg.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption/*QRegularExpression::CaseInsensitiveOption*/);
                if (myreg.isValid())
                {
                    QRegularExpressionMatch regmatch = myreg.match(htmlsource, 0, QRegularExpression::PartialPreferCompleteMatch);
                    if (regmatch.hasMatch())
                    {
                        qDebug() << "Capture Count: " << myreg.captureCount();
                        qDebug() << "Captured No. " << regmatch.lastCapturedIndex() << "of matching groups.";
                        qDebug() << "RegExp all matches: " << regmatch.captured(0); // 0 = Complete regex-result-group
                        qDebug() << "RegExp matched the following 1: " << regmatch.captured(1);
                        qDebug() << "RegExp matched the following 2: " << regmatch.captured(2);
                        qDebug() << "RegExp matched the following 3: " << regmatch.captured(3);
                        qDebug() << "RegExp matched the following 4: " << regmatch.captured(4);
                        tbl_stats.insert(itFList.key(), regmatch.captured(0));
                    }
                }
                else
                {
                    qDebug() << "ERROR: The regular expression is invalid: " <<  myvec->at(1);
                }
            }
        }
        itFList++;
    }
    this->statusBar()->showMessage("Processed " + QString::number(elemCounter) + " elements with " + QString::number(filterData.size()) +  " filters in this webpage.", this->ui_timeout);
}

// Check all HTML-Tags from the filters
void Window::scanAllWebElements(const QWebElement& parentElement, int& icounter)
{
    //qDebug() << "scanAllWebElements 1";
    QWebElement element = parentElement.firstChild();
    /* Tag-Names must be in Capitals, always!! */

    // QHash<QString, QVector<QString>*>

    while (!element.isNull()) // run through the whole webpage
    {
        ++icounter;
        // Run every webelement through all the available filter until one filter is met or the end
        // of the list is reached, then remove that filter through the list.
        auto itFList = this->filterData.constBegin();
        while (itFList != filterData.constEnd())
        {
            auto myvec = itFList.value();
            if (myvec->size() == 0)
                continue;

            int amtAttr = 0;
            // How many Search Arguments has the user entered?
            for (int i=0; i < myvec->size(); i++)
            {
                if (!myvec->at(i).isEmpty())
                    amtAttr++;
            }
            if (amtAttr > 2)
                amtAttr = amtAttr - 2; // Minus both Occurrence Attributes!
            else
                amtAttr = 0;

            // HTML-BLOCK
            if (myvec->at(0) == "tag") // Filtertype = HTML-Tag
            {
                //qDebug() << "Processing HTML-Tag-Filter: " << itFList.key();
                //qDebug() << "Searching for the HTML-Tag: " << myvec->at(1);
                QVector<int> bResVec; // Stores all the results of the searches
                QStringList argList;
                // TAG 1
                if (element.tagName() == myvec->at(1).toUpper() && !myvec->at(1).isEmpty())
                {
                    qDebug() << "Tag found:" << myvec->at(1);
                    bResVec.push_back(1);
                }
                // ATTRIBUTES multi
                if (!myvec->at(2).isEmpty())
                {
                    if (myvec->at(2).contains(" "))
                    {
                        argList = myvec->at(2).split(" ");
                    }
                    else
                    {
                        argList << myvec->at(2); // Only 1 attribute
                    }
                    for (int i=0; i < argList.size(); i++)
                    {
                        if ( element.hasAttribute(argList.at(i)) )
                        {
                            //qDebug() << "Attribute(s) found:" << myvec->at(1);
                            bResVec.push_back(1);
                        }
                    }
                }
                // ID 1
                if (!myvec->at(3).isEmpty())
                {
                    if (element.attribute("id") == myvec->at(3))
                    {
                        qDebug() << "ID found:" << myvec->at(3);
                        bResVec.push_back(1);
                    }
                }
                // CLASSES multi
                if (!myvec->at(4).isEmpty())
                {
                    if (myvec->at(4).contains(" "))
                    {
                        argList = myvec->at(4).split(" ");
                    }
                    else
                    {
                        argList << myvec->at(4); // Only 1 attribute
                    }
                    for (int i=0; i < argList.size(); i++)
                    {
                        if ( element.hasClass(argList.at(i)) )
                        {
                            qDebug() << "Class(es) found:" << myvec->at(4);
                            bResVec.push_back(1);
                        }
                    }
                }
                // OCURRENCE AMOUNT 1
                // OCCURRENCE FUNCTION 1

                // PARENT BLOCK
                // P TAG 1
                if (element.parent().tagName() == myvec->at(5).toUpper() && !myvec->at(5).isEmpty())
                {
                    qDebug() << "P Tag found:" << myvec->at(1);
                    bResVec.push_back(1);
                }
                // P ATTRIBUTES multi
                if (!myvec->at(6).isEmpty())
                {
                    if (myvec->at(6).contains(" "))
                    {
                        argList = myvec->at(6).split(" ");
                    }
                    else
                    {
                        argList << myvec->at(6); // Only 1 attribute
                    }
                    for (int i=0; i < argList.size(); i++)
                    {
                        if ( element.parent().hasAttribute(argList.at(i)) )
                        {
                            //qDebug() << "P Attribute(s) found:" << myvec->at(1);
                            bResVec.push_back(1);
                        }
                    }
                }
                // P ID 1
                if (!myvec->at(7).isEmpty())
                {
                    if (element.parent().attribute("id") == myvec->at(7))
                    {
                        qDebug() << "P ID found:" << myvec->at(7);
                        bResVec.push_back(1);
                    }
                }
                // P CLASSES multi
                if (!myvec->at(8).isEmpty())
                {
                    if (myvec->at(8).contains(" "))
                    {
                        argList = myvec->at(8).split(" ");
                    }
                    else
                    {
                        argList << myvec->at(8); // Only 1 attribute
                    }
                    for (int i=0; i < argList.size(); i++)
                    {
                        if ( element.parent().hasClass(argList.at(i)) )
                        {
                            qDebug() << "P Class(es) found:" << myvec->at(8);
                            bResVec.push_back(1);
                        }
                    }
                }

                // Do we have a full match of arguments? Then write to result table.
                if (amtAttr == bResVec.size() && amtAttr > 0 && bResVec.size() > 0)
                {
                    qDebug() << "FullMatch found :) Element:" << element.toPlainText();
                    qDebug() << "\t amtAttr:" << amtAttr << ", bResVec.size()=" << bResVec.size();
                    tbl_stats.insert(itFList.key(), element.toPlainText());
                }
                else
                {
                    //qDebug() << "No full match with amtAttr:" << amtAttr << " bResVec.size()=" << bResVec.size();
                }
            }
            else
            {
                //qDebug() << "ERROR: No Html-Tag Filter found, nothing to process";
            }
            itFList++;
        }

        // ********************************************************************
        /*
        if (element.tagName() == "H2" && element.hasClass("") && element.hasClass("") && element.attribute("id") == "" && element.hasAttribute("") && element.hasAttribute(""))
        {
            qDebug() << "HTML-Element found...";
            if (element.parent().tagName() == "ASIDE" && element.parent().hasClass("") && element.parent().hasClass("") && element.parent().hasAttribute("") && element.parent().hasAttribute("") && element.parent().attribute("id") == "")
            {
                qDebug() << "+ + + Filter with Parent passed :D";
            }
            else
            {
                qDebug() << "Error: Filter with Parent failed";
            }
        }
        else
        {
            qDebug() << "Error: Filter without Parent failed";
        }

        if (element.tagName() == "H2")
        {
            qDebug() << "+ H2 gefunden...";
            if (element.parent().tagName() == "ASIDE")
                qDebug() << "+ Einzelsuche erfolgreich :-)";
            else
                qDebug() << "+ Einzelsuche gescheitert";
        }
        */
        // ********************************************************************

        this->scanAllWebElements(element, icounter);
        element = element.nextSibling();
    }
}

void Window::on_pushButtonScan_clicked()
{
    this->setCursor(Qt::WaitCursor);

    treeWidget->clear();
    this->tableWidgetStats->clear();
    tableWidgetStats->setHorizontalHeaderLabels(QStringList() << "Data Field" << "Value");

    QWebFrame *frame = webView->page()->mainFrame();
    QWebElement document = frame->documentElement();
    treeWidget->clear(); // Remove all tree-items before refilling them
    examineChildElements(document, treeWidget->invisibleRootItem());
    this->processWebFilters(document); // Replaces the static filter processing here
    fillTable();
    getHTML();
    //statusBar()->showMessage("Current Url: " + currentUrl.toString());

    // Save a thumbnail of the current webpage
    QWebPage* tmpPage = this->webView->page();
    QSize origSize = tmpPage->viewportSize(); // Save the original screen-viewport size
    tmpPage->setViewportSize(tmpPage->mainFrame()->contentsSize());
    QImage image(tmpPage->viewportSize(), QImage::Format_ARGB32);
    QPainter painter(&image);
    tmpPage->mainFrame()->render(&painter);
    painter.end();
    tmpPage->setViewportSize(origSize); // Reset the original screen-viewport size

    QImage  thumbnail;
    if (propDialog.getBCropThumb())
    {
        QImage copyimg = image.scaledToWidth(propDialog.getThumbWidth(), Qt::SmoothTransformation);
        thumbnail = copyimg.copy( 0, 0, propDialog.getThumbWidth(), propDialog.getThumbHeight());
    }
    else
    {
        thumbnail = image.scaledToWidth(propDialog.getThumbWidth(), Qt::SmoothTransformation);
    }
    if (this->strCurrentProjectFileName.isEmpty())
        thumbnail.save("webtooth_thumbnail.png");
    else
        thumbnail.save(this->strCurrentProjectFileName.remove(QRegularExpression(".wpjson")) + "_thumb.png");

    this->setCursor(Qt::ArrowCursor);
}

void Window::fillTable(void)
{
    if (!tbl_stats.isEmpty())
    {
        tableWidgetStats->setRowCount(tbl_stats.size());

        int indexi = 0;
        QMapIterator<QString, QString> it(tbl_stats);
        while (it.hasNext())
        {
            it.next();
            //cout << tbl_stats.key() << ": " << tbl_stats.value() << endl;
            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setText(it.key());
            QTableWidgetItem* item2 = new QTableWidgetItem();
            item2->setText(it.value());
            tableWidgetStats->setItem(indexi,0, item1);
            tableWidgetStats->setItem(indexi,1, item2);
            ++indexi; // increase row counter
        }
    }
}

// Extract HTML from the webpage loaded
void Window::getHTML(void)
{
    this->sHtml = webView->page()->mainFrame()->toHtml();
    if (sHtml.isEmpty())
    {
        plainTextEditHtml->insertPlainText("...no HTML to display.");
    }
    else
    {
        plainTextEditHtml->insertPlainText(sHtml);
    }
}

void Window::on_pushButtonSelectedHtml_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(webView->selectedHtml().toHtmlEscaped());
    msgBox.setWindowTitle("Selected HTML");
    msgBox.exec();
}

void Window::on_lineEditURL_returnPressed()
{
    QString tmpUrl = "";
    tmpUrl = this->lineEditURL->text();
    if (tmpUrl.isEmpty())
        return;

    if (tmpUrl.contains("://"))
    {
        // Are the '//' at the front?
        int idx = tmpUrl.indexOf("://", 0);
        qDebug() << " :// found at pos:" << idx;
        if (idx < 3)
            qDebug() << "ERROR: Invalid URL received";
    }
    else
    {
        // Add http:// to the front
        tmpUrl.insert(0, "http://");
        this->lineEditURL->setText(tmpUrl);
    }

    this->setUrl(tmpUrl);
    //qDebug() << "on_lineEditURL_returnPressed() triggered :)";
}

void Window::on_webView_urlChanged(const QUrl &arg1)
{
    statusBar()->showMessage("Current Url: " + arg1.toString());
}


void Window::on_pushButtonAddFilter_clicked()
{
    QMessageBox msgBox;
    if (this->lineEditNewLabel->text().size() > 256)
    {
        QMessageBox::critical(this, "Error Adding Filters", "The filter label is too long (Max 256)!");
        return;
    }
    QStringList strlFilter = this->lineEditNewLabel->text().split(";");
    // Is the syntax correct?
    if (strlFilter.count() < 2 || strlFilter.last().isEmpty())
    {
        msgBox.setText("You have to add the desired type of filter (t or r) seperated by a ';'.");
        msgBox.setWindowTitle("Error Adding Filters");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    else
    {
        if (strlFilter.last().at(0) == 't' || strlFilter.last().at(0) == 'T') // Tag-Filter
        {
            QStringList strlCommand;
            QString sCommand = "";
            bool isMultiadd = false;
            unsigned short amountAdd = 0;

            // Is this a MultiAdd command?
            if (strlFilter.last().size() > 1) // String longer than 1 letter?
            {
                strlCommand = strlFilter.last().split("#");
                sCommand = strlCommand.first();
                isMultiadd = true;
                amountAdd = strlCommand.last().toShort(); //USHRT_MAX
            }

            QList<QListWidgetItem*> listwidgets = this->listWidgetFilters->findItems(strlFilter.first(), Qt::MatchExactly);
            if (listwidgets.count() > 0)
            {
                msgBox.setText("A filter with this label already exists. The label of a filter has to be unique, so choose a meaningful name.");
                msgBox.setWindowTitle("Error Adding Filters");
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();
                return;
            }
            if (isMultiadd)
            {
                for (unsigned short i=0; i < amountAdd; i++)
                {
                    QListWidgetItem *itm = new QListWidgetItem();
                    itm->setIcon(QIcon(":/img/res/Clipboard-icon.png"));
                    itm->setToolTip("HTML-Tag-Filter");
                    itm->setWhatsThis("tag");
                    itm->setText(strlFilter.first()+QString::number(i));
                    this->listWidgetFilters->addItem(itm);
                    this->addFilter(itm->text(), eFilterType::HTMLTAG);
                }
            }
            else
            {
                QListWidgetItem *itm = new QListWidgetItem();
                itm->setIcon(QIcon(":/img/res/Clipboard-icon.png"));
                itm->setToolTip("HTML-Tag-Filter");
                itm->setWhatsThis("tag");
                itm->setText(strlFilter.first());
                this->listWidgetFilters->addItem(itm);
                this->addFilter(itm->text(), eFilterType::HTMLTAG);
            }
        }
        else if (strlFilter.last().at(0) == 'r' || strlFilter.last().at(0) == 'R') // RegExp-Filter
        {
            QStringList strlCommand;
            QString sCommand = "";
            bool isMultiadd = false;
            unsigned short amountAdd = 0;

            // Is this a MultiAdd command?
            if (strlFilter.last().size() > 1) // String longer than 1 letter?
            {
                strlCommand = strlFilter.last().split("#");
                sCommand = strlCommand.first();
                isMultiadd = true;
                amountAdd = strlCommand.last().toShort(); //USHRT_MAX
            }

            QList<QListWidgetItem*> listwidgets = this->listWidgetFilters->findItems(strlFilter.first(), Qt::MatchExactly);
            if (listwidgets.count() > 0)
            {
                msgBox.setText("A filter with this label already exists. The label of a filter has to be unique, so choose a meaningful name.");
                msgBox.setWindowTitle("Error Adding Filters");
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();
                return;
            }

            if (isMultiadd)
            {
                for (unsigned short i=0; i < amountAdd; i++)
                {
                    QListWidgetItem *itm = new QListWidgetItem();
                    itm->setIcon(QIcon(":/img/res/Wand-icon.png"));
                    itm->setToolTip("RegExp-Filter");
                    itm->setText(strlFilter.first()+QString::number(i));
                    itm->setWhatsThis("regexp");
                    this->listWidgetFilters->addItem(itm);
                    this->addFilter(itm->text(), eFilterType::REGEXP);
                }
            }
            else
            {
                QListWidgetItem *itm = new QListWidgetItem();
                itm->setIcon(QIcon(":/img/res/Wand-icon.png"));
                itm->setToolTip("RegExp-Filter");
                itm->setText(strlFilter.first());
                itm->setWhatsThis("regexp");
                this->listWidgetFilters->addItem(itm);
                this->addFilter(itm->text(), eFilterType::REGEXP);
            }
        }
        else if (strlFilter.last() == "d" || strlFilter.last() == "D") // Delete this Filter
        {
            QList<QListWidgetItem*> listwidgets = this->listWidgetFilters->findItems(strlFilter.first(), Qt::MatchExactly);
            qDebug() << "listwidgets (Qt::MatchExactly) Counted Items: " << listwidgets.size();
            for (int i = 0; i < listwidgets.size(); ++i)
            {
                int row = 0;
                row = this->listWidgetFilters->row(listwidgets.at(i));
                this->listWidgetFilters->removeItemWidget(listwidgets.at(i)); // Remove ItemWidget first...
                this->listWidgetFilters->takeItem(row); // ...then remove item at row x
                this->removeFilter(strlFilter.first());
            }
        }
        else if (strlFilter.last() == "d*" || strlFilter.last() == "D*") // Delete all matching Filters
        {
            QList<QListWidgetItem*> listwidgets = this->listWidgetFilters->findItems(strlFilter.first(), Qt::MatchWildcard);
            qDebug() << "listwidgets (Qt::MatchWildcard) Counted Items: " << listwidgets.size();
            for (int i = 0; i < listwidgets.size(); ++i)
            {
                int row = 0;
                row = this->listWidgetFilters->row(listwidgets.at(i));
                this->listWidgetFilters->removeItemWidget(listwidgets.at(i)); // Remove ItemWidget first...
                this->listWidgetFilters->takeItem(row); // ...then remove item at row x
                this->removeFilter(listwidgets.at(i)->text());
                //qDebug() << "Filter Label (D*): " << listwidgets.at(i)->text();
            }
        }
        else
        {
            QMessageBox::critical(this, "Error Adding Filters", "The syntax for adding a filter is incomplete!");
        }
    }
    isDirtyData = true;
    QString tmpTitle = this->windowTitle();
    if (tmpTitle.right(1) != "*")
        this->setWindowTitle(tmpTitle+"*");
    this->actionSave_Project->setEnabled(true);

    lineEditNewLabel->selectAll(); //Preselect it for convenience serial use by keyboard
}

void Window::on_listWidgetFilters_itemClicked(QListWidgetItem *item)
{
    qDebug() << "Item with Label:" << item->text() << "was selected";
    this->strCurrentFilter = item->text();
    auto myvec = this->filterData.value(item->text());
    if (item->whatsThis() == "tag")
    {
        // Fill the PropertyBrowser if data exists or not if empty....
        ((QtStringPropertyManager*)(item1->propertyManager()))->setValue(item1, myvec->value(1));
        ((QtStringPropertyManager*)(item2->propertyManager()))->setValue(item2, myvec->value(2));
        ((QtStringPropertyManager*)(item3->propertyManager()))->setValue(item3, myvec->value(3));
        ((QtStringPropertyManager*)(item4->propertyManager()))->setValue(item4, myvec->value(4));
        ((QtStringPropertyManager*)(itemP1->propertyManager()))->setValue(itemP1, myvec->value(5));
        ((QtStringPropertyManager*)(itemP2->propertyManager()))->setValue(itemP2, myvec->value(6));
        ((QtStringPropertyManager*)(itemP3->propertyManager()))->setValue(itemP3, myvec->value(7));
        ((QtStringPropertyManager*)(itemP4->propertyManager()))->setValue(itemP4, myvec->value(8));

        ((QtIntPropertyManager*)(amtOccurrence->propertyManager()))->setValue(amtOccurrence, myvec->value(9).toInt());
        ((QtEnumPropertyManager*)(occurrences->propertyManager()))->setValue(occurrences, myvec->value(10).toInt());

        regexp_editor->hide();
        tag_editor->show();
    }
    else if (item->whatsThis() == "regexp")
    {
        ((QtStringPropertyManager*)(itemR1->propertyManager()))->setValue(itemR1, myvec->value(1));
        tag_editor->hide();
        regexp_editor->show();
    }
}

void Window::on_actionExit_Application_triggered()
{
    this->close();
}

void Window::closeEvent(QCloseEvent *event)
{
    qDebug() << "Window-Close Event triggered...";
    //this->on_actionNew_Project_triggered();// Is some data unsaved? Check it before the close.
    //this->on_actionNew_Project_triggered();
    // Needs to accept a Cancel from SaveFileDialog not to close the window.
    /*
    if (maybeSave())
    {
            writeSettings();
            event->accept();
    }
    else
    {
            event->ignore();
    }
    */
    // Check if old project is unsaved and ask if not
    if (isDirtyData)
    {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setInformativeText("Do you want to save your changes before closing the application or discard them alltogether?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret)
        {
          case QMessageBox::Save:
              this->on_actionSave_Project_triggered();
              event->accept();
              break;
          case QMessageBox::Discard:
              // RemoveAllFilters
              this->removeAllFilters();
              // Clear the QListWidget
              this->clearListWidget();
              // Clear strCurrentProjectFileName
              this->strCurrentProjectFileName = "";
              // Display new Project name in Window title
              this->setWindowTitle(initWindowTitle);
              event->accept();
              break;
          case QMessageBox::Cancel:
              // Cancel was clicked, no save and exit
              event->ignore();
              break;
          default:
              // should never be reached
              break;
        }
    }
    else
    {
        // RemoveAllFilters
        this->removeAllFilters();
        // Clear the QListWidget
        this->clearListWidget();
        // Clear strCurrentProjectFileName
        this->strCurrentProjectFileName = "";
        // Display new Project name in Window title
        this->setWindowTitle(initWindowTitle);

    }
}


// Setup both PropertyBrowser Types
void Window::setupPropertyBrowsers(void)
{
    stringManager = new QtStringPropertyManager(widgetPropertyBrowser);
    QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(widgetPropertyBrowser);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(widgetPropertyBrowser);

    /* Setup of Tag-Property-Browser-Widget */
    QtProperty *item0 = groupManager->addProperty("HTML-Tag");
    item1 = stringManager->addProperty("H Tag Name");
    item2 = stringManager->addProperty("H Attribute Name");
    item3 = stringManager->addProperty("H ID Name");
    item4 = stringManager->addProperty("H Class Name");
    item0->addSubProperty(item1);
    item0->addSubProperty(item2);
    item0->addSubProperty(item3);
    item0->addSubProperty(item4);
    //
    //QtIntPropertyManager* intManager = QtIntPropertyManager(widgetPropertyBrowser);
    amtOccurrence;
    intManager = new QtIntPropertyManager(widgetPropertyBrowser);
    amtOccurrence = intManager->addProperty("Occurence Amount");
    amtOccurrence->setToolTip("Occurrence number for the selected function (0 - 32767)");
    intManager->setRange(amtOccurrence, 0, 32767);
    intManager->setValue(amtOccurrence, 1);

    enumManager = new QtEnumPropertyManager(widgetPropertyBrowser);
    QtEnumEditorFactory* enumFactory = new QtEnumEditorFactory(widgetPropertyBrowser);
    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(widgetPropertyBrowser);
    occurrences = nullptr;
    QStringList typeslist;
    occurrences = enumManager->addProperty("Occurrence Function");
    occurrences->setToolTip("Select if you want to extract a number of occurences of this filter or the n-th occurrence of it");
    typeslist << "n-th occurrence" << "Amount of occurrences";
    enumManager->setEnumNames(occurrences, typeslist);
    enumManager->setValue(occurrences, 1);
    item0->addSubProperty(amtOccurrence);
    item0->addSubProperty(occurrences);

    itemP0 = groupManager->addProperty("Parent HTML-Tag");
    itemP1 = stringManager->addProperty("P Tag Name");
    itemP2 = stringManager->addProperty("P Attribute Name");
    itemP3 = stringManager->addProperty("P ID Name");
    itemP4 = stringManager->addProperty("P Class Name");
    itemP0->addSubProperty(itemP1);
    itemP0->addSubProperty(itemP2);
    itemP0->addSubProperty(itemP3);
    itemP0->addSubProperty(itemP4);


    tag_editor = new QtTreePropertyBrowser();
    //
    tag_editor->setFactoryForManager(enumManager, enumFactory);
    tag_editor->setFactoryForManager(intManager, spinBoxFactory);
    //
    tag_editor->setFactoryForManager(stringManager, lineEditFactory);
    tag_editor->addProperty(item0);
    tag_editor->addProperty(itemP0);
    verticalLayoutProp->addWidget(tag_editor);
    tag_editor->hide();
    //dockWidgetProp->setWidget(editor1);
    //((QtStringPropertyManager*)(item1->propertyManager()))->setValue(item1, "Gugus");
    /* Setup of RegExp-Property-Browser-Widget */
    QtProperty *itemR0 = groupManager->addProperty ("RegExp Search");
    itemR1 = stringManager->addProperty("Regular Expression");
    itemR0->addSubProperty(itemR1);


    regexp_editor = new QtTreePropertyBrowser();
    regexp_editor->setFactoryForManager(stringManager, lineEditFactory);
    regexp_editor->addProperty(itemR0);
    verticalLayoutProp->addWidget(regexp_editor);
    regexp_editor->hide();

    dockWidgetProp->setLayout(verticalLayoutProp);
}

void Window::on_lineEditNewLabel_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
        pushButtonAddFilter->setEnabled(false);
    else
        pushButtonAddFilter->setEnabled(true);
}

void Window::on_lineEditNewLabel_returnPressed()
{
    if (!lineEditNewLabel->text().isEmpty())
        this->on_pushButtonAddFilter_clicked();
}

// Read the project file in
void Window::on_actionOpen_Project_triggered()
{
    QString docPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    qDebug() << "QStandardPaths::DocumentsLocation: [" << docPath << "]";
    //QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    //qDebug() << "QStandardPaths::HomeLocation: [" << homeLocation << "]";
    QString tmpFileName = QFileDialog::getOpenFileName(this, "Open Project File", docPath, "WebTooth JSON Project Files (*.wpjson);;Text Files (*.txt);;All Files (*.*)");
    if (tmpFileName.isEmpty())
    {
        return; // User aborted the OpenFileDialog
    }
    else
    {
        qDebug() << "File:" << tmpFileName << "to be opened";
        if (this->projectFile.readJsonFile(this->filterData, propDialog.hProjectData, tmpFileName))
        {
            strCurrentProjectFileName = tmpFileName;
            this->setWindowTitle("WebTooth Extractor - ["+ strCurrentProjectFileName + "]");
            propDialog.LoadData();

            // QHash<QString, QVector<QString>*> t_filterdata;
            QHashIterator<QString, QVector<QString>*> itFilter(filterData);
            QString filtName = "";
            QVector<QString>* filterattr = nullptr;
            int ic = 0;
            qDebug() << "Amount of filters in filterData:" << filterData.size();
            while (itFilter.hasNext())
            {
                itFilter.next();
                ic++;
                qDebug() << "filterdata.key():" << itFilter.key() << ", ic:" << ic;
                filtName = itFilter.key();
                filterattr = itFilter.value();
                if (filterattr->at(0) == "tag")
                {
                    qDebug() << "FilterName 2:" << filtName;
                    QListWidgetItem *itm = new QListWidgetItem();
                    itm->setIcon(QIcon(":/img/res/Clipboard-icon.png"));
                    itm->setToolTip("HTML-Tag-Filter");
                    itm->setWhatsThis("tag");
                    itm->setText(filtName);
                    this->listWidgetFilters->addItem(itm);
                    this->addFilter(itm->text(), eFilterType::HTMLTAG);
                }
                else if (filterattr->at(0) == "reg")
                {
                    qDebug() << "FilterName 2:" << filtName;
                    QListWidgetItem *itm = new QListWidgetItem();
                    itm->setIcon(QIcon(":/img/res/Wand-icon.png"));
                    itm->setToolTip("RegExp-Filter");
                    itm->setWhatsThis("regexp");
                    itm->setText(filtName);
                    this->listWidgetFilters->addItem(itm);
                    this->addFilter(itm->text(), eFilterType::REGEXP);
                }
            }
        }
        else
        {
            qDebug() << "ERROR: Open Project-File failed.";
        }
    }
}

// Save the project data to file
void Window::on_actionSave_Project_triggered()
{
    if (strCurrentProjectFileName.isEmpty())
    {
        QString docPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory)+"WebTooth-Project";
        QString tmpFileName = QFileDialog::getSaveFileName(this, "Save Project File", docPath, "WebTooth JSON Project Files (*.wpjson);;Text Files (*.txt);;All Files (*.*)");
        if (tmpFileName.isEmpty())
        {
            return; // User aborted the SaveFileDialog
        }
        else
        {
            strCurrentProjectFileName = tmpFileName;
            this->setWindowTitle("WebTooth Extractor - ["+ strCurrentProjectFileName + "]");
            // Fill Properties
            // Fill this->listWidgetFilters
        }
    }

    if(this->filterData.size() < 1 && !strCurrentProjectFileName.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("No data to save.");
        //msgBox.setIcon(QMessageBox::Warning);
        msgBox.setIconPixmap(QPixmap(":/img/res/Spongebob-icon.png"));
        msgBox.setWindowTitle("Project Save");
        msgBox.exec();
    }
    else
    {
        QUrl tmpUrl(strCurrentProjectFileName);
        QString tmpFileName = tmpUrl.fileName().remove(QRegularExpression("\.[^.]*$")); // Remove path + extension
        propDialog.hProjectData["projectname"] = tmpFileName;
        QDateTime now = QDateTime::currentDateTime();
        propDialog.hProjectData["projectmodified"] = now.toString(Qt::ISODate);

        if (!this->projectFile.writeJsonFile(this->filterData, propDialog.hProjectData, strCurrentProjectFileName))
        {
            qDebug() << "Rollback Save-Process";
            this->setWindowTitle("WebTooth Extractor - [new project]*"); // If we could not save then rollback this process
            strCurrentProjectFileName = "";
            isDirtyData = true;
        }
        else
        {
            isDirtyData = false;
            QString tmpTitle = this->windowTitle();
            if (tmpTitle.right(1) == "*")
            {
                tmpTitle = tmpTitle.remove(tmpTitle.size()-1, 1); // Remove the trailing *
                this->setWindowTitle(tmpTitle);
            }
        }
    }
}

void Window::addFilter(QString fLabel, eFilterType ftype)
{
    if (this->filterData.size() < 1 || this->filterData[fLabel] == nullptr )
    {
        qDebug() << "No value found, create a new filter.";
        QVector<QString>* filterProperties = new QVector<QString>();
        if (ftype == eFilterType::HTMLTAG)
        {
            filterProperties->push_back("tag"); // eFilterType

            // QHash<QString, QVector<QString> t_filterdata;
            // HTML-Tags
            filterProperties->push_back(""); // "Tag Name"
            filterProperties->push_back(""); // "Attribute Name"
            filterProperties->push_back(""); // "ID Name"
            filterProperties->push_back(""); // "Class Name"
            // Parent-HTML-Tags
            filterProperties->push_back(""); // "Tag Name"
            filterProperties->push_back(""); // "Attribute Name"
            filterProperties->push_back(""); // "ID Name"
            filterProperties->push_back(""); // "Class Name"
            filterProperties->push_back("1"); // "Amount Occurrences"
            filterProperties->push_back("1"); // "Occurrences Function"
        }
        else if (ftype == eFilterType::REGEXP)
        {
            filterProperties->push_back("reg"); // eFilterType

            // RegExp Property
            filterProperties->push_back(""); // "Regular Expression"
        }
        else
        {
            qDebug() << "Uknown Filter-Type encountered";
            return;
        }
        this->filterData.insert(fLabel,filterProperties);
    }
    else
    {
        qDebug() << "Label for filter was found:" << fLabel << "fill the data in now";
        QVector<QString>* filterProperties = this->filterData[fLabel];
        if (ftype == eFilterType::HTMLTAG)
        {
            filterProperties->push_back("tag"); // eFilterType

            // QHash<QString, QVector<QString> t_filterdata;
            // HTML-Tags
            filterProperties->push_back(filterProperties->at(1)); // "Tag Name"
            filterProperties->push_back(filterProperties->at(2)); // "Attribute Name"
            filterProperties->push_back(filterProperties->at(3)); // "ID Name"
            filterProperties->push_back(filterProperties->at(4)); // "Class Name"
            // Parent-HTML-Tags
            filterProperties->push_back(filterProperties->at(5)); // "Tag Name"
            filterProperties->push_back(filterProperties->at(6)); // "Attribute Name"
            filterProperties->push_back(filterProperties->at(7)); // "ID Name"
            filterProperties->push_back(filterProperties->at(8)); // "Class Name"
            filterProperties->push_back(filterProperties->at(9)); // "Amount Occurrences"
            filterProperties->push_back(filterProperties->at(10)); // "Occurrences Function"
        }
        else if (ftype == eFilterType::REGEXP)
        {
            filterProperties->push_back("reg"); // eFilterType

            // RegExp Property
            filterProperties->push_back(filterProperties->at(1)); // "Regular Expression"
        }
    }
    //QSharedPointer<QVector<QString>> filterProperties = QSharedPointer<QVector<QString>>(new QVector<QString>);
}


void Window::removeFilter(QString fLabel)
{
    auto myvec = this->filterData.value(fLabel);
    delete myvec;
    this->filterData.remove(fLabel);
}

void Window::removeAllFilters(void)
{
    if (this->filterData.size() < 1)
        return;
    auto itFList = this->filterData.constBegin();
    while (itFList != this->filterData.constEnd())
    {
        QVector<QString>* myvec = itFList.value();
        if (!myvec)
        {
            qDebug() << "QVector not found";
        }
        else
        {
            //qDebug() << "QVector found and deleting now...";
            delete myvec;
        }
        ++itFList;
        //this->filterData.remove(itFList.key());
    }
    qDebug() << "Clearing the QMap now...";
    this->filterData.clear();
}

void Window::clearListWidget()
{
    QList<QListWidgetItem*> listwidgets = this->listWidgetFilters->findItems("*", Qt::MatchWildcard);
    //qDebug() << "listwidgets (Qt::MatchWildcard) Counted Items: " << listwidgets.size();
    for (int i = 0; i < listwidgets.size(); ++i)
    {
        int row = 0;
        row = this->listWidgetFilters->row(listwidgets.at(i));
        this->listWidgetFilters->removeItemWidget(listwidgets.at(i)); // Remove ItemWidget first...
        this->listWidgetFilters->takeItem(row); // ...then remove item at row x
        //this->removeFilter("*");
    }
}

void Window::on_widgetPropertyBrowser_valueChanged(QtProperty* myprop, const QString& strValue)
{
    QString sfType = "";
    sfType = myprop->propertyName().left(3);
    qDebug() << "++ Property Change Event fired!! FilterLabel: " << strCurrentFilter << sfType << myprop->propertyName() << ":" << strValue;
    auto myvec = this->filterData.value(strCurrentFilter);

    if (sfType == "H T")
    {
        // HTML-Tags may not contain any whitespaces in their name, remove them if found
        QString tmpText = strValue.simplified();
        if (!tmpText.contains(' '))
        {
            myvec->replace(1, tmpText);
            qDebug() << "Tag Name written:" << tmpText;
            if (!this->labelPropertyError->isHidden())
                this->labelPropertyError->hide();
        }
        else
        {
            qDebug() << "ERROR: Whitespaces found in the HTML-Tag name.";
            this->labelPropertyError->setText("Error: HTML-Tags cannot contain spaces!");
            this->labelPropertyError->show();
        }
    }
    else if (sfType == "H A")
    {
        myvec->replace(2, strValue.simplified());
    }
    else if (sfType == "H I")
    {
        myvec->replace(3, strValue.simplified());
    }
    else if (sfType == "H C")
    {
        myvec->replace(4, strValue.simplified());
    }
    else if (sfType == "P T")
    {
        // HTML-Tags may not contain any whitespaces in their name, remove them if found
        QString tmpText = strValue.simplified();
        if (!tmpText.contains(' '))
        {
            myvec->replace(5, tmpText);
            qDebug() << "Tag Name written:" << tmpText;
            if (!this->labelPropertyError->isHidden())
                this->labelPropertyError->hide();
        }
        else
        {
            qDebug() << "ERROR: Whitespaces found in the HTML-Tag name.";
            this->labelPropertyError->setText("Error: HTML-Tags cannot contain spaces!");
            this->labelPropertyError->show();
        }
    }
    else if (sfType == "P A")
    {
        myvec->replace(6, strValue.simplified());
    }
    else if (sfType == "P I")
    {
        myvec->replace(7, strValue.simplified());
    }
    else if (sfType == "P C")
    {
        myvec->replace(8, strValue.simplified());
    }
    else if (sfType == "Reg")
    {
        myvec->replace(1, strValue);
    }
    isDirtyData = true;
    QString tmpTitle = this->windowTitle();
    if (tmpTitle.right(1) != "*")
        this->setWindowTitle(tmpTitle+"*");
}

/*
void Window::on_stringEdit_editingFinished()
{
    qDebug() << "oo editingFinished Event fired!! ";
}
*/

void Window::on_actionNew_Project_triggered()
{
    // Check if old project is unsaved and ask if not
    if (isDirtyData)
    {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setInformativeText("Do you want to save your changes before creating a new project or discard them alltogether?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret)
        {
          case QMessageBox::Save:
              this->on_actionSave_Project_triggered();
              break;
          case QMessageBox::Discard:
                // RemoveAllFilters
                this->removeAllFilters();
                // Clear the QListWidget
                this->clearListWidget();
                // Clear strCurrentProjectFileName
                this->strCurrentProjectFileName = "";
                // Display new Project name in Window title
                this->setWindowTitle(initWindowTitle);
                this->actionSave_Project->setEnabled(false);
              break;
          case QMessageBox::Cancel:
              // Cancel was clicked, do nothing
              break;
          default:
              // should never be reached
              break;
        }
    }
    else
    {
        // RemoveAllFilters
        this->removeAllFilters();
        // Clear the QListWidget
        this->clearListWidget();
        // Clear strCurrentProjectFileName
        this->strCurrentProjectFileName = "";
        // Display new Project name in Window title
        this->setWindowTitle(initWindowTitle);
        this->actionSave_Project->setEnabled(false);
    }
}

void Window::on_toolButtonClipBoard_clicked()
{
    if (tbl_stats.size() < 1)
        return;
    QString tmpTable = "";
    QMapIterator<QString, QString> it(tbl_stats);
    while (it.hasNext())
    {
        it.next();
        tmpTable += it.key() + "\t" + it.value() + "\n";
    }
    tmpTable += "Data from the WebTooth Extractor (c) by Patrick Scheller 2015\n";
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tmpTable);
}

void Window::on_toolButtonLoadLocalHTML_clicked()
{
    QUrl homePath(QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory));
    QUrl urlpath = QFileDialog::getOpenFileUrl(this, "Open Local HTML File", homePath, "HTML File (*.html *.htm);;Text Files (*.txt);;All Files (*.*)");

    if (!urlpath.isEmpty())
        this->lineEditURL->setText(urlpath.toString());
}

void Window::on_toolButtonHome_clicked()
{
    this->setUrl(this->originalUrl);
}

void Window::on_toolButtonBack_clicked()
{
    this->webView->back();
}

void Window::on_toolButtonForward_clicked()
{
    this->webView->forward();
}

void Window::on_toolButtonReload_clicked()
{
    this->webView->reload();
}

void Window::on_actionProperties_triggered()
{
    propDialog.setSLastModificationDate(propDialog.hProjectData["projectmodified"]);

    switch (propDialog.exec())
    {
    case QDialog::Accepted:
        this->lineEditURL->setText(propDialog.getSUrlPath());
        break;
    case QDialog::Rejected:
        // Do nothing
        break;
    }
}

void Window::on_actionHelp_triggered()
{
    this->webView->setUrl(QUrl("qrc:/img/res/webtooth-help.html"));
    this->webView->show();
}

void Window::on_actionAbout_triggered()
{
    switch (aboutDialog.exec())
    {
    case QDialog::Accepted:
        //this->lineEditURL->setText(aboutDialog.getSUrlPath());
        break;
    case QDialog::Rejected:
        // Do nothing
        break;
    }
}

void Window::on_action50_triggered()
{
    this->webView->setZoomFactor(0.5);
    this->action100->setChecked(false);
    this->action125->setChecked(false);
    this->action150->setChecked(false);
    webZoomFactor = 0.5;
}

void Window::on_action100_triggered()
{
    this->webView->setZoomFactor(1.0);
    this->action50->setChecked(false);
    this->action125->setChecked(false);
    this->action150->setChecked(false);
    webZoomFactor = 1.0;
}

void Window::on_action150_triggered()
{
    this->webView->setZoomFactor(1.5);
    this->action50->setChecked(false);
    this->action100->setChecked(false);
    this->action125->setChecked(false);
    webZoomFactor = 1.25;
}

void Window::on_action125_triggered()
{
    this->webView->setZoomFactor(1.25);
    this->action50->setChecked(false);
    this->action100->setChecked(false);
    this->action150->setChecked(false);
    webZoomFactor = 1.5;
}

void Window::on_actionSet_Content_As_Editable_triggered()
{
    if (webView->page()->isContentEditable())
    {
        webView->page()->setContentEditable(false);
        qDebug() << "WebView setContentEditable(false)";
    }
    else
    {
        webView->page()->setContentEditable(true);
        qDebug() << "WebView setContentEditable(true)";
    }
}

void Window::on_webView_loadFinished(bool arg1)
{
    //dockWidgetInspector->close();
    insp->setPage(webView->page());
    /*
    dlg->setLayout(new QVBoxLayout());
    dlg->layout()->addWidget(insp);
    dlg->setModal(false);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
    */
}

void Window::on_actionShow_Web_Inspector_Dock_triggered(bool checked)
{
    if (checked)
        dockWidgetInspector->show();
    else
        dockWidgetInspector->close();
}




void Window::on_dockWidgetInspector_dockLocationChanged(const Qt::DockWidgetArea &area)
{
}

void Window::on_dockWidgetInspector_visibilityChanged(bool visible)
{
    if (visible)
    {
        qDebug() << "dockWidgetInspector isVisible";
        this->actionShow_Web_Inspector_Dock->setChecked(true);
    }
    else
    {
        qDebug() << "dockWidgetInspector isHidden";
        this->actionShow_Web_Inspector_Dock->setChecked(false);
    }
}

void Window::on_dockWidgetFilter_visibilityChanged(bool visible)
{
    if (visible)
    {
        this->actionShow_Filter_List_Dock->setChecked(true);
    }
    else
    {
        this->actionShow_Filter_List_Dock->setChecked(false);
    }
}

void Window::on_dockWidgetProp_visibilityChanged(bool visible)
{
    if (visible)
    {
        this->actionShow_Extraction_Properties_Dock->setChecked(true);
    }
    else
    {
        this->actionShow_Extraction_Properties_Dock->setChecked(false);
    }
}

void Window::on_dockWidgetRes_visibilityChanged(bool visible)
{
    if (visible)
    {
        this->actionShow_Results_Panel_Dock->setChecked(true);
    }
    else
    {
        this->actionShow_Results_Panel_Dock->setChecked(false);
    }
}

void Window::on_actionShow_Filter_List_Dock_triggered(bool checked)
{
    if (checked)
        dockWidgetFilter->show();
    else
        dockWidgetFilter->close();
}

void Window::on_actionShow_Extraction_Properties_Dock_triggered(bool checked)
{
    if (checked)
        dockWidgetProp->show();
    else
        dockWidgetProp->close();
}

void Window::on_actionShow_Results_Panel_Dock_triggered(bool checked)
{
    if (checked)
        dockWidgetRes->show();
    else
        dockWidgetRes->close();
}

void Window::on_widgetPropertyBrowser_IntValueChanged(QtProperty* myprop, int iValue)
{
    qDebug() << "IntValue changed:" << iValue;
    auto myvec = this->filterData.value(strCurrentFilter);
    if (myprop->propertyName() == "Occurrence Amount")
    {
         myvec->replace(9, QString::number(iValue));
    }
    isDirtyData = true;
    QString tmpTitle = this->windowTitle();
    if (tmpTitle.right(1) != "*")
        this->setWindowTitle(tmpTitle+"*");


}

void Window::on_widgetPropertyBrowser_EnumValueChanged(QtProperty* myprop, int iValue)
{
    qDebug() << "EnumValue changed:" << iValue;
    auto myvec = this->filterData.value(strCurrentFilter);
    if (myprop->propertyName() == "Occurrence Function")
    {
         myvec->replace(10, QString::number(iValue));
    }
    isDirtyData = true;
    QString tmpTitle = this->windowTitle();
    if (tmpTitle.right(1) != "*")
        this->setWindowTitle(tmpTitle+"*");


}

void Window::on_actionExport_as_CSV_triggered()
{
    QString docPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory)+"WebTooth-Results";
    QString tmpFileName = QFileDialog::getSaveFileName(this, "Save CSV File", docPath, "WebTooth CSV Result Files (*.csv);;Text Files (*.txt);;All Files (*.*)");
    if (tmpFileName.isEmpty())
    {
        return; // User aborted the SaveFileDialog
    }
    else
    {
        CSVFile mycsvfile;
        if (mycsvfile.WriteResultCSV(tbl_stats, tmpFileName, propDialog.getCsvSeparator()))
        {
            qDebug() << "CSV File has been successfully written :)";
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("The CSV file could not be written, please check your filesystem and permissions!");
            msgBox.setWindowTitle("Error Writing Data to CSV");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
}


void Window::on_actionWhat_s_this_Help_mode_triggered()
{
    if (QWhatsThis::inWhatsThisMode())
    {
        QWhatsThis::leaveWhatsThisMode();
    }
    else
    {
        QWhatsThis::enterWhatsThisMode();
    }
}


