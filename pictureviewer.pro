QT += core gui widgets

TARGET = PictureViewer
TEMPLATE = app

CONFIG += c++1z sdk_no_version_check 
QMAKE_CXXFLAGS = -std=c++17

CONFIG(debug, debug|release) {
  DESTDIR = build/debug
}
CONFIG(release, debug|release) {
  DESTDIR = build/release
  CONFIG += optimize_full
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.u

macx {
  QMAKE_INFO_PLIST = resources/Info.plist
  ICON = resources/images/Icon.icns
  QMAKE_CFLAGS += -gdwarf-2
  QMAKE_CXXFLAGS += -gdwarf-2
}


win32 {
  vsproj.spec = $$basename(QMAKESPEC)
  DEFINES += NOMINMAX WIN32 _WINDOWS QCE_BUILD
  QMAKE_CFLAGS += /D_CRT_SECURE_NO_WARNINGS /DSUPPORT_UTF8 /MP
  QMAKE_CXXFLAGS = $$QMAKE_CFLAGS
  RC_FILE = resources/winicon.rc

  LIBS += -lAdvapi32 -lWs2_32 -lIphlpapi
}

INCLUDEPATH += src/app

HEADERS  += \
    src/app/InfoViewer.h \
    src/app/MainWindow.h \
    src/app/DirectoryScanner.h \
    src/app/FileList.h

SOURCES += \
    src/main.cpp \
    src/app/MainWindow.cpp \
    src/app/InfoViewer.cpp \
    src/app/DirectoryScanner.cpp \
    src/app/FileList.cpp

FORMS    += \
    src/app/InfoViewer.ui \
    src/app/MainWindow.ui

RESOURCES += resources/resources.qrc
