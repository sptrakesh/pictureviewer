#include "PdfEngine.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtGui/QPageSize>
#include <QtGui/QPdfWriter>

using com::sptci::PdfEngine;
using com::sptci::PdfSpecPtr;

Q_LOGGING_CATEGORY(PDF_ENGINE, "com::sptci::PdfEngine")

PdfEngine::PdfEngine(PdfSpecPtr spec, QObject* parent) :
    QObject(parent), spec(std::move(spec)) {}

void PdfEngine::create(const QString& file)
{
  const auto value = static_cast<QPagedPaintDevice::PageSize>(spec->paperSize);
}
