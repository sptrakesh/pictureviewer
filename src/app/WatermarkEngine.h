#ifndef WATERMARKENGINE_H
#define WATERMARKENGINE_H

#include "WatermarkSpec.h"

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <tuple>

namespace com::sptci
{
  class WatermarkEngine : public QObject
  {
    Q_OBJECT
  public:
    explicit WatermarkEngine(WatermarkSpec* spec, QObject* parent = nullptr);

    QImage overlay(const QString& file);
    QImage burnIn(const QString& file);

  private:
    void renderWatermark(QImage* image);
    std::tuple<int,int> textCoordinates(QImage* image, QPaintDevice* device);
    std::tuple<int,int> computeTextBox(QPaintDevice* device);

  private:
    QFont font;
    WatermarkSpec* spec;
    int fontSize;
  };

} // namespace com::sptci

#endif // WATERMARKENGINE_H
