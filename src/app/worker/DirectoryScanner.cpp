#include "DirectoryScanner.h"

#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>
#include <QtCore/QDebug>
#include <QtGui/QImageReader>

using com::sptci::worker::DirectoryScanner;

Q_LOGGING_CATEGORY(DIRECTORY_SCANNER, "com::sptci::worker::DirectoryScanner")

DirectoryScanner::DirectoryScanner(const QString directory, QObject* parent) :
    QObject(parent), directory(directory) {}

DirectoryScanner::~DirectoryScanner()
{
  setAbort(true);
}

void DirectoryScanner::stop()
{
  qInfo(DIRECTORY_SCANNER) << "Stop requested";
  setAbort(true);
}

void DirectoryScanner::scan()
{
  QEventLoop loop;
  auto count = 0;
  QDirIterator iter{ directory, QDirIterator::Subdirectories};
  while (iter.hasNext())
  {
    if (abort)
    {
      qInfo(DIRECTORY_SCANNER) << "Aborted scanning " << directory << " after " << count << " images";
      return;
    }

    const auto path = iter.next();
    if (iter.fileInfo().isDir()) continue;
    const auto fileName = iter.fileInfo().fileName();
    if (fileName.contains("thumb")) continue;
    if (fileName.endsWith(".plist")) continue;
    if (fileName.endsWith(".db")) continue;
    if (fileName.endsWith(".html")) continue;
    if (fileName.endsWith(".htm")) continue;
    if (fileName.endsWith(".css")) continue;
    if (fileName.endsWith(".js")) continue;
    if (fileName.endsWith(".orig")) continue;
    if (fileName.endsWith(".bak")) continue;
    if (fileName.endsWith(".lock")) continue;
    if (fileName.startsWith("_")) continue;
    if (fileName.startsWith(".")) continue;
    if (directory.endsWith(".photoslibrary") && path.contains("derivative")) continue;
    if (directory.endsWith(".photoslibrary") && fileName.contains("facetile_")) continue;
    if (directory.endsWith(".photoslibrary") && fileName.contains("jpegvideocomplement_")) continue;
    if (directory.endsWith(".photoslibrary") && fileName.endsWith(".data")) continue;

    const auto bytes = QImageReader::imageFormat(path);
    if (!bytes.isEmpty())
    {
      emit file(path);
      if (!(++count % 100)) loop.processEvents();
    }
    else
    {
      qDebug(DIRECTORY_SCANNER) << "Ignoring unsupported file " << path;
    }
  }
  qInfo(DIRECTORY_SCANNER) << "Finished scanning " << directory << " with " << count << " images";
  emit finished(directory, count);
}

void DirectoryScanner::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
