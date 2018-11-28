#ifndef COM_SPTCI_WATERMARK_H
#define COM_SPTCI_WATERMARK_H

#include <QtWidgets/QWidget>
#include <tuple>

namespace com::sptci
{

  namespace Ui { class Watermark; }

  class Watermark : public QWidget
  {
    Q_OBJECT

  public:
    explicit Watermark(const QString& file, QWidget* parent = nullptr);
    ~Watermark();

  public slots:
    void selectFontColour();
    void selectBackgroundColour();
    void clear();
    void preview();
    void saveAs();

  private:
    void setForeground();
    void displayImage();
    void displayImage(const QImage& image);
    QString getText();
    void overlay();
    void burnIn();
    void renderWatermark(QImage* image);
    std::tuple<int, int> textCoordinates(const QImage& image, QPaintDevice* device);
    void saveFile(const QString& fileName);

  private:
    const QString file;
    QColor fontColour;
    QColor backgroundColour;
    Ui::Watermark *ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_WATERMARK_H
