#ifndef COM_SPTCI_EXIFWINDOW_H
#define COM_SPTCI_EXIFWINDOW_H

#include <QtWidgets/QWidget>

namespace com::sptci
{
  namespace Ui { class ExifWindow; }

  class ExifWindow : public QWidget
  {
    Q_OBJECT

  public:
    explicit ExifWindow(const QString& file, QWidget* parent = nullptr);
    ~ExifWindow();

  private:
    const QString file;
    Ui::ExifWindow *ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_EXIFWINDOW_H
