#ifndef COM_SPTCI_COMICLISTWIDGET_H
#define COM_SPTCI_COMICLISTWIDGET_H

#include <QtWidgets/QWidget>
#include "model/ComicList.h"

namespace com::sptci
{

  namespace Ui { class ComicListWidget; }

  class ComicListWidget : public QWidget
  {
    Q_OBJECT

  public:
    explicit ComicListWidget(QWidget* parent = nullptr);
    ~ComicListWidget();

  signals:
    void display(QString file);

  public slots:
    void addFile();
    void removeFile();
    void scan();
    void showComic(QModelIndex index);

  private slots:
    void addComic(QString file);

  private:
    Ui::ComicListWidget* ui;
    model::ComicList* model;
  };

} // namespace com::sptci
#endif // COM_SPTCI_COMICLISTWIDGET_H
