#include "WatermarkEngine.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>

using com::sptci::WatermarkEngine;
using com::sptci::WatermarkSpec;

Q_LOGGING_CATEGORY(WATERMARK_ENGINE, "com::sptci::WatermarkEngine")

WatermarkEngine::WatermarkEngine(WatermarkSpec* spec, QObject *parent) :
  QObject(parent), font(spec->font), spec(spec), fontSize(spec->fontSize) {}

QImage WatermarkEngine::overlay(const QString& file)
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  auto image = reader.read();
  if (image.isNull()) return image;

  renderWatermark(&image);
  return image;
}

QImage WatermarkEngine::burnIn(const QString& file)
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  auto image = reader.read();
  if (image.isNull()) return image;

  QImage mark(image.width(), image.height(), QImage::Format_RGB32);
  renderWatermark(&mark);

  QRgb rgbSrc, rgbMark;
  int r, g, b;
  float alpha = 1 - (spec->textOpacity / 255.0f);
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

  return image;
}

void WatermarkEngine::renderWatermark(QImage* image)
{
  QPainter painter(image);
  const auto& [posX, posY] = textCoordinates(image, painter.device());
  qInfo(WATERMARK_ENGINE) << "Drawing text at: " << posX << "," << posY;

  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setBackgroundMode(Qt::OpaqueMode);

  painter.setPen(spec->fontColour);
  painter.setBackground(QBrush(spec->backgroundColour));
  painter.setFont(font);
  painter.drawText(posX, posY, spec->text);
}

std::tuple<int, int> WatermarkEngine::textCoordinates(QImage* image, QPaintDevice* device)
{
  font.setPointSize(fontSize);
  auto [textWidthInPixels, textHeightInPixels] = computeTextBox(device);

  while (textWidthInPixels > image->width())
  {
    --fontSize;
    qDebug(WATERMARK_ENGINE) << "Font size decreased to " << fontSize;

    font.setPointSize(fontSize);
    const auto& [w, h] = computeTextBox(device);
    textWidthInPixels = w;
    textHeightInPixels = h;
  }

  int posX = 0;
  int posY = 0;

  switch (spec->position)
  {
    case 0: // top
      posY = textHeightInPixels + 10;
      break;
    case 1: // middle
      posY = image->height()/2 - textHeightInPixels/2;
      break;
    case 2: // bottom
      posY = image->height() - 10;
      break;
  }

  switch (spec->alignment)
  {
    case 0: // left
      posX = 10;
      break;
    case 1: // centre
      posX = image->width()/2 - textWidthInPixels/2;
      break;
    case 2: // right
      posX = image->width() - (textWidthInPixels + 10);
      break;
  }

  qDebug(WATERMARK_ENGINE) << "Image dimensions: (" << image->width() <<
      "x" << image->height() << "); text position: (" <<
      posX << "x" << posY << "); font dimensions: (" <<
      textWidthInPixels << "x" << textHeightInPixels << ")";
  return {posX, posY};
}

std::tuple<int,int> WatermarkEngine::computeTextBox(QPaintDevice* device)
{
  const QFontMetrics fm(font, device);
  auto boundingRect = fm.boundingRect(spec->text);
  boundingRect = fm.boundingRect(boundingRect, 0, spec->text);

  auto textWidthInPixels = boundingRect.width();
  auto textHeightInPixels = boundingRect.height();
  return {textWidthInPixels, textHeightInPixels};
}
