#include "DirectoryScanner.h"

#include <QtCore/QDirIterator>
#include <QtGui/QImageReader>

using com::sptci::DirectoryScanner;

DirectoryScanner::DirectoryScanner(const QString directory, QObject* parent) :
    QObject(parent), directory(directory) {}

DirectoryScanner::~DirectoryScanner()
{
  setAbort(true);
}

void DirectoryScanner::stop()
{
  qDebug("Stop requested");
  setAbort(true);
}

void DirectoryScanner::scan()
{
  QDirIterator iter{ directory, QDirIterator::Subdirectories};
  while (iter.hasNext())
  {
    if (abort) return;
    const auto path = iter.next();
    if (iter.fileInfo().isDir()) continue;

    const auto bytes = QImageReader::imageFormat(path);
    if (!bytes.isEmpty()) emit file(path);
  }
}

void DirectoryScanner::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
