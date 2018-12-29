#include "ComicScanner.h"

#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QSet>

using com::sptci::worker::ComicScanner;

Q_LOGGING_CATEGORY(COMIC_SCANNER, "com::sptci::worker::ComicScanner")

ComicScanner::ComicScanner(const QString& directory, QObject* parent) :
  QObject(parent), directory(directory) {}

ComicScanner::~ComicScanner()
{
  setAbort(true);
}

void ComicScanner::run()
{
  const QSet<QString> extensions{"cbr", "cbz", "cbt", "cb7"};
  QEventLoop loop;

  int count = 0;
  auto iter = QDirIterator(QDir(directory), QDirIterator::Subdirectories);
  while (iter.hasNext())
  {
    if (getAbort())
    {
      qInfo(COMIC_SCANNER) << "Aborted scanning " << directory << " after " << count << " comic books";
      break;
    }

    loop.processEvents();
    const auto file = iter.next();
    const auto fi = QFileInfo(file);

    if (fi.isDir()) continue;
    if (extensions.find(fi.suffix()) == extensions.cend()) continue;

    ++count;
    qInfo(COMIC_SCANNER) << "Found comic book file " << file;
    emit comic(file);
  }

  qInfo(COMIC_SCANNER) << "Finished scanning " << directory <<
    " directory tree.  Found " << count << " comic book files.";
  emit finished(directory, count);
}

bool ComicScanner::getAbort()
{
  std::lock_guard<std::mutex> guard(mutex);
  return abort;
}

void ComicScanner::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
