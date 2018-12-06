#include "PdfEngine.h"

#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QLoggingCategory>
#include <QtCore/QTemporaryFile>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtGui/QPageSize>
#include <QtGui/QPdfWriter>
#include <QtGui/QPixmap>

using com::sptci::PdfEngine;
using com::sptci::PdfSpecPtr;

Q_LOGGING_CATEGORY(PDF_ENGINE, "com::sptci::PdfEngine")

PdfEngine::PdfEngine(PdfSpecPtr spec, const QStringList& files, QObject* parent) :
    QObject(parent), files(files), spec(std::move(spec)) {}

void PdfEngine::run()
{
  QEventLoop loop;
  int count = 0;

  QPdfWriter writer(spec->destination);
  const auto value = static_cast<QPagedPaintDevice::PageSize>(spec->paperSize);
  writer.setPageSize(value);

  if (!spec->title.isEmpty()) writer.setTitle(spec->title);
  if (!spec->creator.isEmpty()) writer.setCreator(spec->creator);

  QPainter painter(&writer);

  const auto dimension = dimensions(writer.logicalDpiX(), writer.logicalDpiY());
  const auto& [width, height] = dimension;
  qDebug(PDF_ENGINE) << "PDF page size: " << width << "x: " << height <<
    " file: " << *files.begin();

  bool addPage = false;
  for (const auto& file : files)
  {
    if (getAbort()) break;
    if (addPage) writer.newPage();
    addFile(&painter, dimension, file);
    addPage = true;
    emit progress(++count, files.size(), file);
    loop.processEvents();
  }

  emit finished();
}

void PdfEngine::stop()
{
  qInfo(PDF_ENGINE) << "Stop requested";
  setAbort(true);
}

void PdfEngine::addFile(QPainter* painter, const Dimension& dimension,
    const QString& fileName)
{
  QImageReader reader(fileName);
  reader.setAutoTransform(true);

  const auto& [width, height] = dimension;
  auto pixmap = QPixmap::fromImageReader(&reader).scaled(
    width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  const auto x = (pixmap.width() < width ) ?
    static_cast<int>((width - pixmap.width())/2.0) : 0;
  const auto y = (pixmap.height() < height ) ?
    static_cast<int>((height - pixmap.height())/2.0) : 0;

  if (spec->compression != 100)
  {
    qInfo(PDF_ENGINE) << "Compressing file to JPEG quality " << spec->compression;
    QTemporaryFile tf;
    if (tf.open())
    {
      pixmap.save(tf.fileName(), "JPEG", spec->compression);
      QImageReader r(tf.fileName());
      pixmap = QPixmap::fromImageReader(&r);
    }
  }

  painter->drawPixmap(QPoint(x, y), pixmap);
}

auto PdfEngine::dimensions(int dpiX, int dpiY) -> Dimension
{
  const auto sid =  static_cast<QPageSize::PageSizeId>(spec->paperSize);

  const auto definedSize = QPageSize::definitionSize(sid);
  const auto definedUnits = QPageSize::definitionUnits(sid);

  qDebug(PDF_ENGINE) << "Defined size: " << definedSize << ", units: " << definedUnits;

  auto toInch = [&definedUnits](double value) -> int
  {
    switch (definedUnits)
    {
      case QPageSize::Millimeter:
        return static_cast<int>(value / 25.4);
      case QPageSize::Point:
        return static_cast<int>(value / 72.0);
      case QPageSize::Inch:
        return static_cast<int>(value);
      case QPageSize::Pica:
        return static_cast<int>(value * 6);
      case QPageSize::Didot:
        return static_cast<int>(value * 0.375 / 25.4);
      case QPageSize::Cicero:
        return static_cast<int>(value * 4.5 / 25.4);
    }

    return static_cast<int>(value);
  };

  return {dpiX * toInch(definedSize.width()), dpiY * toInch(definedSize.height())};
}

bool PdfEngine::getAbort()
{
  std::lock_guard<std::mutex> guard(mutex);
  return abort;
}

void PdfEngine::setAbort(bool flag)
{
  std::lock_guard<std::mutex> guard(mutex);
  abort = flag;
}
