#include "ComicEngine.h"

#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMap>

#include <algorithm>

using com::sptci::worker::ComicEngine;
using com::sptci::model::ComicBookSpec;

Q_LOGGING_CATEGORY(COMIC_ENGINE, "com::sptci::worker::ComicEngine")

ComicEngine::ComicEngine(QStringList::ConstIterator begin,
      QStringList::ConstIterator end, const ComicBookSpec& spec,
      QObject* parent) :
  QObject(parent), files(filesFromIterators(spec.sortFiles, begin, end)), spec(spec) {}

ComicEngine::ComicEngine(const QDir& dir, const ComicBookSpec& spec,
    QObject* parent) :
  QObject(parent), files(filesFromDir(spec.sortFiles, dir)), spec(spec) {}

QString ComicEngine::create()
{
  using QArchive::DiskCompressor;

  QEventLoop loop;
  QFileInfo fi{spec.outFile};
  auto suffix = fi.suffix();
  if ("rar" == suffix) suffix = "cbr";
  if ("tar" == suffix) suffix = "cbt";
  if ("zip" == suffix) suffix = "cbz";
  if ("7z" == suffix) suffix = "cb7";

  auto c = DiskCompressor{spec.outFile};
  c.addFiles(files);

  connect(&c, &DiskCompressor::progress , [&](QString file, int processedEntries, int totalEntries, qint64, qint64 ) {
      qInfo(COMIC_ENGINE) << "Compressed " << file <<
          " (" << processedEntries << "/" << totalEntries << ").";
  });

  connect(&c, &DiskCompressor::finished, &loop, &QEventLoop::quit);

  qInfo(COMIC_ENGINE) << "Creating comic book with " << suffix << " extension";
  c.start();
  loop.exec();

  const auto fileName = QString("%1/%2.%3").arg(fi.absolutePath()).arg(fi.completeBaseName()).arg(suffix);
  qInfo(COMIC_ENGINE()) << "Destination file " << fileName;
  QFile::remove(fileName);
  QFile(spec.outFile).rename(fileName);

  return fileName;
}

QStringList ComicEngine::filesFromIterators(bool sort,
      QStringList::ConstIterator begin, QStringList::ConstIterator end)
{
  auto sorted = [&]() {
    QMap<QString,bool> map;
    std::for_each(begin, end, [&map](const QString& file) { map.insert(file, false); });
    return map.keys();
  };

  auto unsorted = [&]() {
    QStringList list;
    std::for_each(begin, end, [&list](const QString& file) { list.append(file); });
    return list;
  };

  return sort ? sorted() : unsorted();
}

QStringList ComicEngine::filesFromDir(bool sort, const QDir& dir)
{
  QDirIterator iter(dir);

  auto sorted = [&iter]() {
    QMap<QString,bool> map;
    while (iter.hasNext()) map.insert(iter.next(), false);
    return map.keys();
  };

  auto unsorted = [&iter]() {
    QStringList list;
    while (iter.hasNext()) list.append(iter.next());
    return list;
  };

  return sort ? sorted() : unsorted();
}
