#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>
#include <QtCore/QStringList>

namespace com::sptci
{
  inline QString picturesDirectory()
  {
    const auto picturesLocations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    return picturesLocations.isEmpty() ?
      QDir::homePath() : picturesLocations.last();
  }

  inline QString documentsDirectory()
  {
    const auto documentsLocations =
      QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    return documentsLocations.isEmpty() ?
      QDir::homePath() : documentsLocations.last();
  }

  inline void showFile(const QString& filePath)
  {
#if defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + filePath + "\"";
    args << "-e";
    args << "end tell";
    QProcess::startDetached( "osascript", args );
#elif defined(Q_OS_WIN)
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(filePath);
    QProcess::startDetached("explorer", args);
#endif
  }
}

  #endif // FUNCTIONS_H
