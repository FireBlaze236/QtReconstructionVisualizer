QT       += core gui
LIBS += -lopengl32 -lglu32


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets openglwidgets opengl

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myglwidget.cpp

HEADERS += \
    Camera.h \
    Filepaths.h \
    happly.h \
    mainwindow.h \
    myglwidget.h \
    mystructs.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
