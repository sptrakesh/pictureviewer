#ifndef COM_SPTCI_WATERMARK_H
#define COM_SPTCI_WATERMARK_H

#include "model/WatermarkSpec.h"
#include <QtWidgets/QProgressDialog>

#include <memory>

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
    void allInDirectory();
    void allFiles();

  private slots:
    void updateProgress(int index, int total, QString file);
    void progressCancelled();
    void finished();

  private:
    model::WatermarkSpecPtr createSpec();
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
    std::unique_ptr<QProgressDialog> progress = nullptr;
    bool completed = true;
  };

} // namespace com::sptci
#endif // COM_SPTCI_WATERMARK_H
