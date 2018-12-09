#ifndef EXIFMODEL_H
#define EXIFMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QMap>

namespace com::sptci::model
{
  class ExifModel : public QAbstractTableModel
  {
    Q_OBJECT
  public:
    ExifModel(const QString& file, QObject* parent = nullptr);

     int rowCount(const QModelIndex& parent) const override;
     int columnCount(const QModelIndex& /* parent */) const override { return 2; }
     QVariant data(const QModelIndex& index, int role) const override;
     QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  private:
     void readData();

  private:
    QMap<QString,QString> map;
    const QString file;
  };

} // namespace com::sptci

#endif // EXIFMODEL_H
