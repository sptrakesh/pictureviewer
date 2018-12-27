TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = libs \
  src

INCLUDEPATH += src/app \
  libs/QArchive \
  libs/QArchive/include
