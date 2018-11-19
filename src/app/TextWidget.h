#ifndef TRANSPARENTWIDGET_H
#define TRANSPARENTWIDGET_H

#include <QtWidgets/QWidget>

namespace com::sptci
{
  class TextWidget : public QWidget
  {
    Q_OBJECT
  public:
    explicit TextWidget(const QString& text, QWidget* parent = nullptr);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    const QString text;
  };
} // namespace com::sptci

#endif // TRANSPARENTWIDGET_H
