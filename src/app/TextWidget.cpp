#include "TextWidget.h"

#include <QtGui/QPainter>

using com::sptci::TextWidget;

TextWidget::TextWidget(const QString& text, QWidget* parent) :
    QWidget(parent), text(text)
{
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

#if defined(Q_OS_MAC)
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_TransparentForMouseEvents);
#else
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_TransparentForMouseEvents);
#endif
}

void TextWidget::paintEvent(QPaintEvent* /* event */)
{
  QPainter p(this);
  p.setFont({"Helvetica", 30});
  QFontMetrics metrics(p.font());
  resize(metrics.size(0, text));
  p.drawText(rect(), Qt::AlignCenter, text);
}
