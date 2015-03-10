#ifndef WINDOW_H
#define WINDOW_H

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

#include <QMainWindow>
#include <QUrl>
#include <QWebElement>
#include <QWebInspector>
#include <qtpropertymanager.h>
#include "jsonfile.h"
#include "properties.h"
#include "aboutdialog.h"


QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QTableWidget;
QT_END_NAMESPACE

#include "ui_window.h"

class Window : public QMainWindow, private Ui::Window
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    void setUrl(const QUrl &url);
    void setupPropertyBrowsers(void);
    bool readProjectFile();
    bool saveProjectFile();
    //static void threadLoadUrl(QUrl url);

    typedef QHash<QString, QVector<QString>*> t_filterdata;
    enum class eFilterType {
        HTMLTAG,
        REGEXP
    };


public slots:
    void on_widgetPropertyBrowser_StringValueChanged(QtProperty* myprop, const QString& strValue);
    void on_widgetPropertyBrowser_IntValueChanged(QtProperty* myprop, int iValue);
    void on_widgetPropertyBrowser_EnumValueChanged(QtProperty* myprop, int iValue);
    //QtIntPropertyManager::valueChanged()
    //QtEnumPropertyManager::enumChanged()

private slots:
    void on_pushButtonScan_clicked();

    void on_pushButtonSelectedHtml_clicked();

    void on_lineEditURL_returnPressed();

    void on_webView_urlChanged(const QUrl &arg1);

    void on_pushButtonAddFilter_clicked();

    void on_actionExit_Application_triggered();

    void on_lineEditNewLabel_textChanged(const QString &arg1);

    void on_lineEditNewLabel_returnPressed();

    void on_actionOpen_Project_triggered();

    void on_actionSave_Project_triggered();

    void on_listWidgetFilters_itemClicked(QListWidgetItem *item);

    void on_actionNew_Project_triggered();

    void on_toolButtonClipBoard_clicked();

    void on_toolButtonLoadLocalHTML_clicked();

    void on_toolButtonHome_clicked();

    void on_toolButtonBack_clicked();

    void on_toolButtonForward_clicked();

    void on_toolButtonReload_clicked();

    void on_actionProperties_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();

    void on_action50_triggered();

    void on_action100_triggered();

    void on_action150_triggered();

    void on_action125_triggered();

    void on_actionSet_Content_As_Editable_triggered();

    void on_webView_loadFinished(bool arg1);

    void on_actionShow_Web_Inspector_Dock_triggered(bool checked);

    void on_dockWidgetInspector_dockLocationChanged(const Qt::DockWidgetArea &area);

    void on_dockWidgetInspector_visibilityChanged(bool visible);

    void on_dockWidgetFilter_visibilityChanged(bool visible);

    void on_dockWidgetProp_visibilityChanged(bool visible);

    void on_dockWidgetRes_visibilityChanged(bool visible);

    void on_actionShow_Filter_List_Dock_triggered(bool checked);

    void on_actionShow_Extraction_Properties_Dock_triggered(bool checked);

    void on_actionShow_Results_Panel_Dock_triggered(bool checked);

    void on_actionExport_as_CSV_triggered();

    void on_actionWhat_s_this_Help_mode_triggered();

private:
    void examineChildElements(const QWebElement &parentElement, QTreeWidgetItem *parentItem);
    QMap<QString, QString> tbl_stats;
    void fillTable(void);
    void getHTML(void);
    void addFilter(QString fLabel, eFilterType ftype); // Add Filter to the t_filterdata structure
    void removeFilter(QString fLabel); // Remove Filter from the t_filterdata structure
    void removeAllFilters(void); // Clear the t_filterdata structure
    void clearListWidget(void);
    void closeEvent(QCloseEvent *event);
    void processWebFilters(const QWebElement& parentElement);
    void scanAllWebElements(const QWebElement &parentElement, int &icounter);

    QString sHtml;
    QUrl currentUrl;
    QUrl originalUrl; // URL from the project settings
    bool isFirstCall;
    QRegExp searchRegExp;
    QtAbstractPropertyBrowser* regexp_editor;
    QtAbstractPropertyBrowser* tag_editor;
    JSONFile projectFile;
    t_filterdata filterData; //FilterLabel <PropertyLabel, PropertyValue>
    QtStringPropertyManager* stringManager;
    QtEnumPropertyManager* enumManager;
    QtIntPropertyManager* intManager;
    QString strCurrentFilter;
    QString strCurrentProjectFileName;
    bool isDirtyData;
    Properties propDialog;
    AboutDialog aboutDialog;
    qreal webZoomFactor;
    QWebInspector* insp;
    QDialog* dlg;
    const QString initWindowTitle = "WebTooth Extractor - [new project]";
    const int ui_timeout = 4000;

    // Property-Fields
    QtProperty *item1;
    QtProperty *item2;
    QtProperty *item3;
    QtProperty *item4;
    QtProperty *amtOccurrence;
    QtProperty *occurrences;
    QtProperty *itemP0;
    QtProperty *itemP1;
    QtProperty *itemP2;
    QtProperty *itemP3;
    QtProperty *itemP4;
    QtProperty *itemR1;
};

#endif
