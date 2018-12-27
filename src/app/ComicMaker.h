#ifndef COM_SPTCI_COMICMAKER_H
#define COM_SPTCI_COMICMAKER_H

#include <QtWidgets/QWidget>
#include "worker/ComicEngine.h"

namespace com::sptci {

  namespace Ui { class ComicMaker; }

  class ComicMaker : public QWidget
  {
    Q_OBJECT

  public:
    explicit ComicMaker(const QString& file, QWidget* parent = nullptr);
    ~ComicMaker();

  public slots:
    void allFiles();
    void inDirectory();

  private:
    QString targetFile();
    void configureEngine(worker::ComicEngine* engine);

  private:
    const QString file;
    Ui::ComicMaker *ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_COMICMAKER_H
