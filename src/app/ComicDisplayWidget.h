#ifndef COM_SPTCI_COMICDISPLAYWIDGET_H
#define COM_SPTCI_COMICDISPLAYWIDGET_H

#include <QtCore/QStringList>
#include <QtWidgets/QWidget>

#include <memory>

#include "archive/OutputDirectory.h"

namespace com::sptci
{

  namespace Ui { class ComicDisplayWidget; }

  namespace comic { struct OutputFile; }

  class ComicDisplayWidget : public QWidget
  {
    Q_OBJECT

  public:
    explicit ComicDisplayWidget(QWidget* parent = nullptr);
    ~ComicDisplayWidget();

  public slots:
    void display(const QString& file);
    void atIndex(int index);

  private:
    void extract();
    void showFile(int index = 0);

  private:
    QStringList files;
    Ui::ComicDisplayWidget* ui;
    std::unique_ptr<comic::OutputFile> outputFile;
    archive::OutputDirectory outputDirectory;
    int currentIndex = 0;
  };

} // namespace com::sptci
#endif // COM_SPTCI_COMICDISPLAYWIDGET_H
