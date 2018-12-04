#include "PdfMaker.h"
#include "ui_PdfMaker.h"
#include "functions.h"
#include "MainWindow.h"
#include "PaperSizeModel.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtGui/QPageSize>
#include <QtGui/QPdfWriter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <algorithm>

using com::sptci::PdfMaker;

Q_LOGGING_CATEGORY(PDF_MAKER, "com::sptci::PdfMaker")

PdfMaker::PdfMaker(const QString& file, QWidget* parent) :
    QWidget(parent), file(file), ui(new Ui::PdfMaker)
{
  ui->setupUi(this);

  auto model = new com::sptci::PaperSizeModel(this);
  ui->paperSize->setModel(model);
  ui->paperSize->setCurrentIndex(model->systemSize());

  setWindowFlags(Qt::Window);
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));
}

PdfMaker::~PdfMaker()
{
  delete ui;
}

void PdfMaker::saveAs()
{
  QFileInfo fi(file);

  const auto directory = com::sptci::documentsDirectory();
  const auto fn = QString("%1/%2.pdf").arg(directory).arg(fi.baseName());

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      fn,
      tr("Documents (*.pdf)"));
  if (fileName.isEmpty())
  {
    qDebug(PDF_MAKER) << "User cancelled file dialogue.";
    return;
  }

#if defined(Q_OS_WIN)
  if (QFileInfo(fileName).exists())
  {
    const auto result = QMessageBox::question(this,
        tr("Over-write file?"),
        QString("File %1 exists").arg(fileName));

    if (QMessageBox::Yes == result)
    {
      save(fileName);
      return;
    }
  }
#endif

  save(fileName);
}

void PdfMaker::save(const QString& destination)
{
  const auto selected = ui->paperSize->currentIndex();
  const auto index = ui->paperSize->model()->index(selected, 1);
  const auto data = ui->paperSize->model()->data(index);
  const auto value = static_cast<QPagedPaintDevice::PageSize>(data.toInt());

  QPdfWriter writer(destination);
  writer.setPageSize(value);
  QPainter painter(&writer);

  const auto dimension = dimensions(
    {data.toInt(), writer.logicalDpiX(), writer.logicalDpiY()});
  const auto& [width, height] = dimension;
  qDebug(PDF_MAKER) << "PDF page size: " << width << "x: " << height <<
    " file: " << file;

  switch (ui->file->currentIndex())
  {
    case 0: // File when pdf window was displayed
      addFile(&painter, dimension, file);
      break;
    case 1: // All files loaded
      const MainWindow* mw = dynamic_cast<MainWindow*>(parent());
      if (!mw)
      {
        qWarning(PDF_MAKER) << "Parent object not instance of com::sptci::MainWindow!";
        return;
      }

      auto addPage = false;
      std::for_each(mw->cbegin(), mw->cend(),
        [this, &painter, &dimension, &writer, &addPage](const QString& fileName) {
          if (addPage) writer.newPage();
          addFile(&painter, dimension, fileName);
          addPage = true;
        });
      break;
  }

  qInfo(PDF_MAKER) << "Saved PDF " << destination;
  close();
}

void PdfMaker::addFile(QPainter* painter, const Dimension& dimension,
    const QString& fileName)
{
  QImageReader reader(fileName);
  reader.setAutoTransform(true);

  const auto& [width, height] = dimension;
  const auto image = reader.read().scaled(
    width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  const auto x = (image.width() < width ) ?
    static_cast<int>((width - image.width())/2.0) : 0;
  const auto y = (image.height() < height ) ?
    static_cast<int>((height - image.height())/2.0) : 0;

  painter->drawImage(QPoint(x, y), image);
}

auto PdfMaker::dimensions(const PdfMaker::DimParam& param) -> Dimension
{
  const auto sid =  static_cast<QPageSize::PageSizeId>(param.sizeId);

  const auto definedSize = QPageSize::definitionSize(sid);
  const auto definedUnits = QPageSize::definitionUnits(sid);

  qDebug(PDF_MAKER) << "Defined size: " << definedSize << ", units: " << definedUnits;

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
  };

  return {param.dpiX * toInch(definedSize.width()), param.dpiY * toInch(definedSize.height())};
}
