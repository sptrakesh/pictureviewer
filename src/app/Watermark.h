#ifndef COM_SPTCI_WATERMARK_H
#define COM_SPTCI_WATERMARK_H

#include "WatermarkSpec.h"
#include <QtCore/QThread>
#include <QtWidgets/QWidget>
#include <QtWidgets/QProgressDialog>

namespace com::sptci
{

  namespace Ui { class Watermark; }

  class Watermark : public QWidget
  {
    Q_OBJECT

  public:
    explicit Watermark(const QString& file, QWidget* parent = nullptr);
    ~Watermark();

  signals:
    void startWatermarking();

  public slots:
    void selectFontColour();
    void selectBackgroundColour();
    void clear();
    void preview();
    void saveAs();
    void allInDirectory();

  private slots:
    void updateProgress(int index, QString file);
    void progressCancelled();

  private:
    WatermarkSpecPtr createSpec();
    void setForeground();
    void displayImage();
    void displayImage(const QImage& image);
    QString getText();
    void overlay();
    void saveFile(const QString& fileName);

  private:
    const QString file;
    QColor fontColour;
    QColor backgroundColour;
    Ui::Watermark *ui;
    QThread* thread = nullptr;
    QProgressDialog *progress = nullptr;
  };

} // namespace com::sptci
#endif // COM_SPTCI_WATERMARK_H
