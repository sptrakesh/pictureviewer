#ifndef COM_SPTCI_COMICREADER_H
#define COM_SPTCI_COMICREADER_H

#include <QtWidgets/QWidget>

namespace com::sptci {

  namespace Ui { class ComicReader; }

  class ComicReader : public QWidget
  {
    Q_OBJECT

  public:
    explicit ComicReader(QWidget* parent = nullptr);
    ~ComicReader();

  signals:
    void display(QString file);

  private slots:
    void displayFile(QString file);

  private:
    void init();

  private:
    Ui::ComicReader* ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_COMICREADER_H
