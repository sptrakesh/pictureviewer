#ifndef COM_SPTCI_COMICMAKER_H
#define COM_SPTCI_COMICMAKER_H

#include <QWidget>

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
    const QString file;
    Ui::ComicMaker *ui;
  };


} // namespace com::sptci
#endif // COM_SPTCI_COMICMAKER_H
