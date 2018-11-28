#include "Watermark.h"
#include "ui_Watermark.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QStandardPaths>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

using com::sptci::Watermark;

Q_LOGGING_CATEGORY(WATERMARK, "com::sptci::Watermark")

Watermark::Watermark(const QString& file, QWidget *parent) :
    QWidget(parent), file(file), fontColour(Qt::black),
    backgroundColour(Qt::transparent), ui(new Ui::Watermark)
{
  ui->setupUi(this);
  fontColour.setAlpha(ui->textOpacity->value());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());
  setWindowFlags(Qt::Window);
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));
  setForeground();
  displayImage();
}

Watermark::~Watermark()
{
  delete ui;
}

void Watermark::selectFontColour()
{
  const auto colour = QColorDialog::getColor(Qt::black, this, "Font Colour");
  if (!colour.isValid()) return;
  fontColour.setNamedColor(colour.name());
  fontColour.setAlpha(ui->textOpacity->value());

  auto pal = ui->fontColour->palette();
  pal.setColor(QPalette::Button, fontColour);
  ui->fontColour->setPalette(pal);
  ui->fontColour->update();
  raise();
}

void Watermark::selectBackgroundColour()
{
  const auto colour = QColorDialog::getColor(Qt::black, this, "Font Colour");
  if (!colour.isValid()) return;
  backgroundColour.setNamedColor(colour.name());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());

  auto pal = ui->backgroundColour->palette();
  pal.setColor(QPalette::Button, backgroundColour);
  ui->backgroundColour->setPalette(pal);
  ui->backgroundColour->update();
  raise();
}

void Watermark::clear()
{
  displayImage();
}

void Watermark::preview()
{
  fontColour.setAlpha(ui->textOpacity->value());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());

  switch (ui->mode->currentIndex())
  {
    case 0:
      clear();
      burnIn();
      burnIn();
      break;
    case 1:
      overlay();
      break;
  }
}

void Watermark::saveAs()
{
  QFileInfo fi(file);

  const auto picturesLocations =
    QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  const auto directory = picturesLocations.isEmpty() ?
    QDir::currentPath() : picturesLocations.last();
  const auto fn = QString("%1/%2").arg(directory).arg(fi.fileName());

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      fn,
      tr("Images (*.png *.xpm *.jpg *.gif *.png)"));
  if (fileName.isEmpty())
  {
    qDebug(WATERMARK) << "User cancelled file dialogue.";
    return;
  }

  if (QFileInfo(fileName).exists())
  {
    const auto result = QMessageBox::question(this,
        tr("Over-write file?"),
        QString("File %1 exists").arg(fileName));

    if (QMessageBox::Yes == result)
    {
      saveFile(fileName);
      return;
    }
  }

  saveFile(fileName);
}

void Watermark::setForeground()
{
#if defined(Q_OS_MAC)
  QPalette palette;
  palette.setColor(QPalette::Text, Qt::black);
  ui->text->setPalette(palette);
  ui->fontComboBox->setPalette(palette);
  ui->fontSize->setPalette(palette);
  ui->textOpacity->setPalette(palette);
  ui->position->setPalette(palette);
  ui->backgroundOpacity->setPalette(palette);
#endif

  auto pal = ui->fontColour->palette();
  pal.setColor(QPalette::Button, fontColour);
  palette.setColor(QPalette::Text, Qt::white);
  ui->fontColour->setPalette(pal);

  pal = ui->backgroundColour->palette();
  pal.setColor(QPalette::Button, backgroundColour);
  palette.setColor(QPalette::Text, Qt::white);
  ui->backgroundColour->setPalette(pal);
}

void Watermark::displayImage()
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  const auto image = reader.read();

  if (image.isNull()) return;

  displayImage(image);
}

void Watermark::displayImage(const QImage& image)
{
  auto pixmap = QPixmap::fromImage(image);
  ui->imageLabel->setPixmap(pixmap);
}

QString Watermark::getText()
{
  return (ui->text->text().isEmpty()) ?
      ui->text->placeholderText() : ui->text->text();
}

void Watermark::overlay()
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  auto image = reader.read();
  if (image.isNull()) return;

  renderWatermark(&image);
  displayImage(image);
}

void Watermark::burnIn()
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  auto image = reader.read();
  if (image.isNull()) return;

  QImage mark(image.width(), image.height(), QImage::Format_RGB32);
  renderWatermark(&mark);

  QRgb rgbSrc, rgbMark;
  int r, g, b;
  float alpha = 1 - (ui->textOpacity->value() / 255.0f);
  float beta = 1 - alpha;

  for(int x = 0; x < mark.width(); ++x)
  {
    for(int y = 0; y < mark.height(); ++y)
    {
      rgbSrc = image.pixel(x, y);
      rgbMark = mark.pixel(x, y);

      r = int(qRed(rgbSrc) * alpha + qRed(rgbMark) * beta);
      g = int(qGreen(rgbSrc) * alpha + qGreen(rgbMark) * beta);
      b = int(qBlue(rgbSrc) * alpha + qBlue(rgbMark) * beta);

      r = (0 <= r && r <= 255) ? r : 0;
      g = (0 <= g && g <= 255) ? g : 0;
      b = (0 <= b && b <= 255) ? b : 0;

      image.setPixel(x, y, qRgb(r, g, b));
    }
  }

  displayImage(image);
}

void Watermark::renderWatermark(QImage* image)
{
  auto font = ui->fontComboBox->currentFont();
  font.setPointSize(ui->fontSize->value());

  QPainter painter(image);
  const auto& [posX, posY] = textCoordinates(*image, painter.device());
  qInfo(WATERMARK) << "Drawing text at: " << posX << "," << posY;

  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setBackgroundMode(Qt::OpaqueMode);

  painter.setPen(fontColour);
  painter.setBackground(QBrush(backgroundColour));
  painter.setFont(font);
  painter.drawText(posX, posY, getText());
}

std::tuple<int, int> Watermark::textCoordinates(const QImage& image, QPaintDevice* device)
{
  auto font = ui->fontComboBox->currentFont();
  font.setPointSize(ui->fontSize->value());

  const QFontMetrics fm(font, device);
  const auto text = getText();

  auto boundingRect = fm.boundingRect(text);
  boundingRect = fm.boundingRect(boundingRect, 0, text);

  const auto textWidthInPixels = boundingRect.width();
  const auto textHeightInPixels = boundingRect.height();

  int posX = 0;
  int posY = 0;

  switch (ui->position->currentIndex())
  {
    case 0: // top
      posY = textHeightInPixels + 10;
      break;
    case 1: // middle
      posY = image.height()/2 - textHeightInPixels/2;
      break;
    case 2: // bottom
      posY = image.height() - 10;
      break;
  }

  switch (ui->alignment->currentIndex())
  {
    case 0: // left
      posX = 10;
      break;
    case 1: // centre
      posX = image.width()/2 - textWidthInPixels/2;
      break;
    case 2: // right
      posX = image.width() - (textWidthInPixels + 10);
      break;
  }

  qDebug(WATERMARK) << "Image dimensions: (" << image.width() <<
      "x" << image.height() << "); text position: (" <<
      posX << "x" << posY << "); font dimensions: (" <<
      textWidthInPixels << "x" << textHeightInPixels << ")";
  return {posX, posY};
}

void Watermark::saveFile(const QString& fileName)
{
  const auto pixmap = ui->imageLabel->pixmap();
  if (!pixmap)
  {
    qWarning(WATERMARK) << "Image label returned null pixmap";
    return;
  }

  const auto image = pixmap->toImage();
  if (image.save(fileName, nullptr, 100))
  {
    qInfo(WATERMARK) << "Wrote watermarked image to " << fileName;
  }
  else
  {
    QMessageBox::warning(this,
      tr("Unable to write file"),
      QString("Unable to write to file: %1").arg(fileName),
      QMessageBox::Ok);
  }
}
