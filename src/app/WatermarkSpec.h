#ifndef WATERMARKSPEC_H
#define WATERMARKSPEC_H

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QFont>

#include <memory>

namespace com::sptci
{
  struct WatermarkSpec
  {
    QString text;
    QColor fontColour;
    QFont font;
    QColor backgroundColour;
    int fontSize;
    int textOpacity;
    int backgroundOpacity;
    int position;
    int alignment;
  };

  using WatermarkSpecPtr = std::unique_ptr<WatermarkSpec>;
}

#endif // WATERMARKSPEC_H
