#include "DirectoryScanner.h"

#include <QtCore/QDirIterator>
#include <QtCore/QDebug>
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
  auto count = 0;
  QDirIterator iter{ directory, QDirIterator::Subdirectories};
  while (iter.hasNext())
  {
    if (abort) return;
    const auto path = iter.next();
    if (iter.fileInfo().isDir()) continue;
    if (iter.fileInfo().fileName().contains("thumb")) continue;

    const auto bytes = QImageReader::imageFormat(path);
    if (!bytes.isEmpty())
    {
      ++count;
      emit file(path);
    }
    else
    {
      qDebug() << "Ignoring unsupported file " << path;
    }
  }
  qDebug() << "Finished scanning " << directory << " with " << count << " images";
}

void DirectoryScanner::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
