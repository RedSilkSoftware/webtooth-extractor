QT       += webkitwidgets network widgets
FORMS     = window.ui \
    properties.ui \
    aboutdialog.ui
HEADERS   = window.h \
    jsonfile.h \
    properties.h \
    aboutdialog.h \
    csvfile.h
SOURCES   = main.cpp \
            window.cpp \
    jsonfile.cpp \
    properties.cpp \
    aboutdialog.cpp \
    csvfile.cpp

RC_ICONS = Blue-block-small.ico

win32:CONFIG(release, debug|release): CONFIG += embed_manifest_exe LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-head
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-headd
else:unix: LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-head

win32:QMAKE_CXXFLAGS_RELEASE += /GL /Ox
unix:QMAKE_CXXFLAGS += -std=c++11 -O3
QMAKE_LFLAGS_WINDOWS += /LTCG

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

unix:INCLUDEPATH += $$PWD/qtpropertybrowser/src
win32:INCLUDEPATH += $$PWD\qtpropertybrowser\src
#DEPENDPATH += $$PWD/qtpropertybrowser/src

RESOURCES += \
    webtooth.qrc

DISTFILES += \
