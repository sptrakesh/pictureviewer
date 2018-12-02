#include "WatermarkDirectory.h"
#include "WatermarkEngine.h"

#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>

using com::sptci::WatermarkDirectory;
using com::sptci::WatermarkSpecPtr;

Q_LOGGING_CATEGORY(WATERMARK_DIRECTORY, "com::sptci::WatermarkDirectory")

WatermarkDirectory::WatermarkDirectory(const QDir& inDir, const QDir& outDir,
  WatermarkSpecPtr spec, QObject *parent) :
    QObject(parent), inDir(inDir), outDir(outDir), spec(std::move(spec)) {}

void WatermarkDirectory::run()
{
  using com::sptci::WatermarkEngine;

  QEventLoop loop;

  QDirIterator iter(inDir);
  uint count = 0;
  while (iter.hasNext())
  {
    if (abort)
    {
      qInfo(WATERMARK_DIRECTORY) << "Abort requested.  Bailing out after " <<
        count << " files out of " << inDir.count() << " files.";
      break;
    }

    const auto file = iter.next();

    WatermarkEngine engine(spec.get(), this);
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
      qWarning(WATERMARK_DIRECTORY) << "Unable to write watermarked file: " <<
        file << " to output file: " << dest;
    }

    emit progress(++count, file);
    loop.processEvents();
  }

  emit finished();
}

void WatermarkDirectory::stop()
{
  qInfo(WATERMARK_DIRECTORY) << "Stop requested";
  setAbort(true);
}

void WatermarkDirectory::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
