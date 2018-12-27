QT += core concurrent gui widgets printsupport

TARGET = ../PictureViewer
TEMPLATE = app

CONFIG(debug, debug|release) {
  DESTDIR = ../build/debug
}
CONFIG(release, debug|release) {
  DESTDIR = ../build/release
  CONFIG += optimize_full
}

LIBS += ../libs/QArchive/libQArchive.a
INCLUDEPATH += app \
  ../libs/QArchive/include

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.u

macx {
  CONFIG += c++1z sdk_no_version_check
  QMAKE_CXXFLAGS = -std=c++17
  QMAKE_INFO_PLIST = ../resources/Info.plist
  ICON = ../resources/images/Icon.icns
  QMAKE_CFLAGS += -gdwarf-2
  QMAKE_CXXFLAGS += -gdwarf-2

  INCLUDEPATH += /opt/local/include
  LIBS += /opt/local/lib/libarchive.a \
    /opt/local/lib/libiconv.a \
    /opt/local/lib/liblzo2.a \
    /opt/local/lib/liblzma.a \
    /opt/local/lib/liblz4.a \
    /opt/local/lib/libcharset.a \
    /opt/local/lib/libbz2.a \
    /opt/local/lib/libz.a \
    /opt/local/lib/libxml2.a \
    /opt/local/lib/libicui18n.a \
    /opt/local/lib/libicuuc.a \
    /opt/local/lib/libicudata.a
}


win32 {
  vsproj.spec = $$basename(QMAKESPEC)
  DEFINES += NOMINMAX WIN32 _WINDOWS QCE_BUILD
  INCLUDEPATH += /opt/local/include
  LIBS += -L/opt/local/lib -larchive
  QMAKE_CFLAGS += /D_CRT_SECURE_NO_WARNINGS /DSUPPORT_UTF8 /MP
  QMAKE_CXXFLAGS = $$QMAKE_CFLAGS
  QMAKE_CXXFLAGS += /std:c++latest
  RC_FILE = ../resources/winicon.rc

  LIBS += -lAdvapi32 -lWs2_32 -lIphlpapi
}

HEADERS  += \
    ext/exif.h \
    app/InfoViewer.h \
    app/MainWindow.h \
    app/worker/DirectoryScanner.h \
    app/model/FileList.h \
    app/TextWidget.h \
    app/model/ExifModel.h \
    app/ExifWindow.h \
    app/Watermark.h \
    app/worker/WatermarkEngine.h \
    app/model/WatermarkSpec.h \
    app/PdfMaker.h \
    app/model/PaperSizeModel.h \
    app/functions.h \
    app/worker/PdfEngine.h \
    app/model/PdfSpec.h \
    app/worker/WatermarkFiles.h \
    app/ComicMaker.h \
    app/worker/ComicEngine.h \
    app/model/ComicBookSpec.h

SOURCES += \
    main.cpp \
    ext/exif.cpp \
    app/MainWindow.cpp \
    app/InfoViewer.cpp \
    app/worker/DirectoryScanner.cpp \
    app/model/FileList.cpp \
    app/TextWidget.cpp \
    app/model/ExifModel.cpp \
    app/ExifWindow.cpp \
    app/Watermark.cpp \
    app/worker/WatermarkEngine.cpp \
    app/PdfMaker.cpp \
    app/model/PaperSizeModel.cpp \
    app/worker/PdfEngine.cpp \
    app/worker/WatermarkFiles.cpp \
    app/ComicMaker.cpp \
    app/worker/ComicEngine.cpp

FORMS    += \
    app/InfoViewer.ui \
    app/MainWindow.ui \
    app/ExifWindow.ui \
    app/Watermark.ui \
    app/PdfMaker.ui \
    app/ComicMaker.ui

RESOURCES += ../resources/resources.qrc
