QT += core gui widgets printsupport

TARGET = PictureViewer
TEMPLATE = app

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
  CONFIG += c++1z sdk_no_version_check
  QMAKE_CXXFLAGS = -std=c++17
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
  QMAKE_CXXFLAGS += /std:c++latest
  RC_FILE = resources/winicon.rc

  LIBS += -lAdvapi32 -lWs2_32 -lIphlpapi
}

INCLUDEPATH += src/app

HEADERS  += \
    src/ext/exif.h \
    src/app/InfoViewer.h \
    src/app/MainWindow.h \
    src/app/DirectoryScanner.h \
    src/app/FileList.h \
    src/app/TextWidget.h \
    src/app/ExifModel.h \
    src/app/ExifWindow.h \
    src/app/Watermark.h \
    src/app/WatermarkEngine.h \
    src/app/WatermarkSpec.h \
    src/app/WatermarkDirectory.h \
    src/app/PdfMaker.h \
    src/app/PaperSizeModel.h \
    src/app/functions.h \
    src/app/PdfEngine.h \
    src/app/PdfSpec.h

SOURCES += \
    src/main.cpp \
    src/ext/exif.cpp \
    src/app/MainWindow.cpp \
    src/app/InfoViewer.cpp \
    src/app/DirectoryScanner.cpp \
    src/app/FileList.cpp \
    src/app/TextWidget.cpp \
    src/app/ExifModel.cpp \
    src/app/ExifWindow.cpp \
    src/app/Watermark.cpp \
    src/app/WatermarkEngine.cpp \
    src/app/WatermarkDirectory.cpp \
    src/app/PdfMaker.cpp \
    src/app/PaperSizeModel.cpp \
    src/app/PdfEngine.cpp

FORMS    += \
    src/app/InfoViewer.ui \
    src/app/MainWindow.ui \
    src/app/ExifWindow.ui \
    src/app/Watermark.ui \
    src/app/PdfMaker.ui

RESOURCES += resources/resources.qrc
