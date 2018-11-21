#include "DirectoryScanner.h"

#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>
#include <QtCore/QDebug>
#include <QtGui/QImageReader>

using com::sptci::DirectoryScanner;

Q_LOGGING_CATEGORY(DIRECTORY_SCANNER, "com.sptci.DirectoryScanner")

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
    if (iter.fileInfo().fileName().contains("thumb")) continue;
    if (iter.fileInfo().fileName().endsWith(".plist")) continue;
    if (iter.fileInfo().fileName().endsWith(".db")) continue;
    if (iter.fileInfo().fileName().endsWith(".html")) continue;
    if (iter.fileInfo().fileName().endsWith(".htm")) continue;
    if (iter.fileInfo().fileName().endsWith(".css")) continue;
    if (iter.fileInfo().fileName().endsWith(".js")) continue;
    if (iter.fileInfo().fileName().endsWith(".orig")) continue;
    if (iter.fileInfo().fileName().endsWith(".bak")) continue;
    if (iter.fileInfo().fileName().startsWith("_")) continue;
    if (iter.fileInfo().fileName().startsWith(".")) continue;

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
