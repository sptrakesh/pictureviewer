#include "WatermarkFiles.h"
#include "WatermarkEngine.h"

#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>

#include <algorithm>

using com::sptci::worker::WatermarkFiles;
using com::sptci::model::WatermarkSpecPtr;

Q_LOGGING_CATEGORY(WATERMARK_FILES, "com::sptci::worker::WatermarkFiles")

WatermarkFiles::WatermarkFiles(QStringList::ConstIterator begin,
    QStringList::ConstIterator end, const QDir& outDir,
    WatermarkSpecPtr spec, QObject* parent) :
  QObject(parent), files(filesFromIterators(begin, end)),
  outDir(outDir), spec(std::move(spec)) {}

WatermarkFiles::WatermarkFiles(const QDir& inDir, const QDir& outDir,
    WatermarkSpecPtr spec, QObject* parent) :
  QObject(parent), files(filesFromDir(inDir)), outDir(outDir), spec(std::move(spec)) {}

void WatermarkFiles::run()
{
  QEventLoop loop;

  for (int i = 0; i < files.size(); ++i)
  {
    if (getAbort())
    {
      qInfo(WATERMARK_FILES) << "Abort requested.  Bailing out after " <<
        i << " files out of " << files.size() << " files.";
      break;
    }

    const auto file = files.at(i);

    WatermarkEngine engine(spec.get());
    const auto image = engine.overlay(file);

    const QFileInfo qf(file);

    auto dest = QString("%1/%2").arg(outDir.path()).arg(qf.fileName());
    int fc = 0;
    while (QFile(dest).exists())
    {
      dest = QString("%1/%2 (%3).%4").
          arg(outDir.path()).arg(qf.baseName()).arg(++fc).arg(qf.completeSuffix());
    }

    if (!image.save(dest))
    {
      qWarning(WATERMARK_FILES) << "Unable to write watermarked file: " <<
        file << " to output file: " << dest;
    }

    emit progress(i+1, files.size(), file);
    loop.processEvents();
  }

  emit finished();
}

void WatermarkFiles::stop()
{
  qInfo(WATERMARK_FILES) << "Stop requested";
  setAbort(true);
}

QStringList WatermarkFiles::filesFromIterators(
      QStringList::ConstIterator begin, QStringList::ConstIterator end)
{
  QStringList list;
  std::for_each(begin, end, [&list](const QString& file) { list.append(file); });
  return list;
}

QStringList WatermarkFiles::filesFromDir(const QDir& dir)
{
  QStringList list;
  QDirIterator iter(dir);

  while (iter.hasNext()) list.append(iter.next());
  return list;
}


bool WatermarkFiles::getAbort()
{
  std::lock_guard<std::mutex> guard(mutex);
  return abort;
}

void WatermarkFiles::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
