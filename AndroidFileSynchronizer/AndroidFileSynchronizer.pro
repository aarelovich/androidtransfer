QT += core gui widgets network

CONFIG += c++11

TARGET = AndroidFileSynchronizer

MOC_DIR = MOCS
OBJECTS_DIR = OBJS

TEMPLATE = app

#INCLUDEPATH += /home/ariela/repos/ColoUiDev/coloUi
INCLUDEPATH += ../..//ColoUiDev/coloUi

SOURCES += main.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouibase.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouibutton.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiconfiguration.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicontainer.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicreator.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouielement.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiitem.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouilist.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouisignalmanager.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouitransitionscreen.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouidropdownlist.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicheckbox.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouislider.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiprogressbar.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiplaceholder.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouikeyboard.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiview.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouimultilinetext.cpp \
#    /home/ariela/repos/ColoUiDev/coloUi/colouilineedit.cpp \
    androidtransferpc.cpp \
    ../common/datapacket.cpp \
    ../common/direxplorer.cpp \
    ../common/transfermanager.cpp \
    ../../ColoUiDev/coloUi/colouibase.cpp \
    ../../ColoUiDev/coloUi/colouibutton.cpp \
    ../../ColoUiDev/coloUi/colouicheckbox.cpp \
    ../../ColoUiDev/coloUi/colouiconfiguration.cpp \
    ../../ColoUiDev/coloUi/colouicontainer.cpp \
    ../../ColoUiDev/coloUi/colouicreator.cpp \
    ../../ColoUiDev/coloUi/colouidropdownlist.cpp \
    ../../ColoUiDev/coloUi/colouielement.cpp \
    ../../ColoUiDev/coloUi/colouiitem.cpp \
    ../../ColoUiDev/coloUi/colouikeyboard.cpp \
    ../../ColoUiDev/coloUi/colouilineedit.cpp \
    ../../ColoUiDev/coloUi/colouilist.cpp \
    ../../ColoUiDev/coloUi/colouimultilinetext.cpp \
    ../../ColoUiDev/coloUi/colouiplaceholder.cpp \
    ../../ColoUiDev/coloUi/colouiprogressbar.cpp \
    ../../ColoUiDev/coloUi/colouisignalmanager.cpp \
    ../../ColoUiDev/coloUi/colouislider.cpp \
    ../../ColoUiDev/coloUi/colouitransitionscreen.cpp \
    ../../ColoUiDev/coloUi/colouiview.cpp

HEADERS += \
#    /home/ariela/repos/ColoUiDev/coloUi/colouibase.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouibutton.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiconfiguration.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicontainer.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicreator.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouidefines.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouielement.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiitem.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouilist.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouisignalmanager.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouitransitionscreen.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouidropdownlist.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouicheckbox.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouislider.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiprogressbar.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiplaceholder.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouikeyboard.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouiview.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouimultilinetext.h \
#    /home/ariela/repos/ColoUiDev/coloUi/colouilineedit.h \
    androidtransferpc.h \
    elements.h \
    ../common/datapacket.h \
    ../common/direxplorer.h \
    ../common/transfermanager.h \
    ../../ColoUiDev/coloUi/colouibase.h \
    ../../ColoUiDev/coloUi/colouibutton.h \
    ../../ColoUiDev/coloUi/colouicheckbox.h \
    ../../ColoUiDev/coloUi/colouiconfiguration.h \
    ../../ColoUiDev/coloUi/colouicontainer.h \
    ../../ColoUiDev/coloUi/colouicreator.h \
    ../../ColoUiDev/coloUi/colouidefines.h \
    ../../ColoUiDev/coloUi/colouidropdownlist.h \
    ../../ColoUiDev/coloUi/colouielement.h \
    ../../ColoUiDev/coloUi/colouiitem.h \
    ../../ColoUiDev/coloUi/colouikeyboard.h \
    ../../ColoUiDev/coloUi/colouilineedit.h \
    ../../ColoUiDev/coloUi/colouilist.h \
    ../../ColoUiDev/coloUi/colouimultilinetext.h \
    ../../ColoUiDev/coloUi/colouiplaceholder.h \
    ../../ColoUiDev/coloUi/colouiprogressbar.h \
    ../../ColoUiDev/coloUi/colouisignalmanager.h \
    ../../ColoUiDev/coloUi/colouislider.h \
    ../../ColoUiDev/coloUi/colouitransitionscreen.h \
    ../../ColoUiDev/coloUi/colouiview.h

RESOURCES += assets.qrc

