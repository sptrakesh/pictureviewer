#ifndef COM_SPTCI_LOGVIEWER_H
#define COM_SPTCI_LOGVIEWER_H

#include <QtWidgets/QWidget>

namespace com::sptci
{

  namespace Ui { class LogViewer; }

  class LogViewer : public QWidget
  {
    Q_OBJECT

  public:
    explicit LogViewer(QWidget* parent = nullptr);
    ~LogViewer();

  public slots:
    void refresh();

  private:
    QString addHtmlColor(const QString& text) const;

  private:
    Ui::LogViewer* ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_LOGVIEWER_H
