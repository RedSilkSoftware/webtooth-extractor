QT       += webkitwidgets network widgets
FORMS     = window.ui \
    properties.ui \
    aboutdialog.ui
HEADERS   = window.h \
    jsonfile.h \
    properties.h \
    aboutdialog.h
SOURCES   = main.cpp \
            window.cpp \
    jsonfile.cpp \
    properties.cpp \
    aboutdialog.cpp

RC_ICONS = Blue-block-small.ico


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-head
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-headd
else:unix: LIBS += -L$$PWD/qtpropertybrowser/lib/ -lQt5Solutions_PropertyBrowser-head

win32:QMAKE_CXXFLAGS_RELEASE += /GL /Ox
unix:QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS_WINDOWS += /LTCG

INCLUDEPATH += $$PWD\qtpropertybrowser\src
#DEPENDPATH += $$PWD/qtpropertybrowser/src

RESOURCES += \
    webtooth.qrc

DISTFILES += \
