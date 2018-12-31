#ifndef COMICLIST_H
#define COMICLIST_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QList>
#include <QtCore/QMap>

namespace com::sptci::model
{
  class ComicList : public QAbstractTableModel
  {
    Q_OBJECT
  public:
    explicit ComicList(QObject* parent = nullptr);

    int rowCount(const QModelIndex& /* parent */) const override;
    int columnCount(const QModelIndex& /* parent */) const override { return 2; }
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    QString fileName(QModelIndex index) const;

  public slots:
    void addFile(const QString& file);

  private:
    void populate();

  private:
    static const QString COMIC_FILES;

    QList<QMap<QString,QVariant>> list;
  };

} // namespace com::sptci::model

#endif // COMICLIST_H
