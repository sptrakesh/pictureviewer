#ifndef COM_SPTCI_PAPERSIZEMODEL_H
#define COM_SPTCI_PAPERSIZEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtGui/QPageSize>

#include <tuple>
#include <vector>

namespace com::sptci {

  class PaperSizeModel : public QAbstractListModel
  {
    Q_OBJECT

  public:
    explicit PaperSizeModel(QObject* parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int systemSize() { return static_cast<int>(systemSizeIndex); }

  private:
    void populate();
    void setIndex();

  private:
    using Item = std::tuple<QPageSize::PageSizeId, QString>;
    using Items = std::vector<Item>;
    Items items;
    std::size_t systemSizeIndex = 0;
  };

} // namespace com::sptci

#endif // COM_SPTCI_PAPERSIZEMODEL_H
