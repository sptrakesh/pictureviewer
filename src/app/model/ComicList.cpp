#include "ComicList.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QColor>

using com::sptci::model::ComicList;

const QString ComicList::COMIC_FILES = "comicFiles";

ComicList::ComicList(QObject* parent) : QAbstractTableModel(parent)
{
  populate();
}

int ComicList::rowCount(const QModelIndex& /* parent */) const
{
  return list.size();
}

QVariant ComicList::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() >= list.size() || index.column() > 1 ||
    Qt::DisplayRole != role ) return QVariant();

  auto map = list.at(index.row());
  switch (index.column())
  {
    case 0:
    {
      QFileInfo fi(map["file"].toString());
      return fi.baseName();
    }
    default:
      return map["read"];
  }
}

QVariant ComicList::headerData(int section, Qt::Orientation orientation,
    int role) const
{
  if (Qt::DisplayRole != role) return QVariant();
  if (Qt::Vertical == orientation) return QString::number(section + 1);

  switch (section)
  {
    case 0: return tr( "File" );
    case 1: return tr( "Last Viewed" );
    default: return QVariant();
  }
}

bool ComicList::insertRows(int row, int count, const QModelIndex& parent)
{
  beginInsertRows(parent, row, row + count - 1);
  QSettings settings;
  auto l = settings.value(COMIC_FILES).toList();
  auto m = l.last().toMap();
  list.append(m);
  endInsertRows();
  return true;
}

bool ComicList::removeRows(int row, int count, const QModelIndex& parent)
{
  beginRemoveRows(parent, row, row + count - 1);
  list.removeAt(row);
  QSettings settings;
  auto l = settings.value(COMIC_FILES).toList();
  l.removeAt(row);
  settings.setValue(COMIC_FILES, l);
  endRemoveRows();
  return true;
}

QString ComicList::fileName(QModelIndex index) const
{
  const auto map = list.at(index.row());
  return map["file"].toString();
}

void ComicList::addFile(const QString& file)
{
  QSettings settings;
  auto l = settings.value(COMIC_FILES).toList();
  QMap<QString,QVariant> map;
  map["file"] = file;
  map["read"] = QString();
  l.append(map);
  settings.setValue(COMIC_FILES, l);
  insertRows(list.size(), 1);
}

void ComicList::populate()
{
  QSettings settings;
  const auto l = settings.value(COMIC_FILES).toList();
  for (auto iter = l.cbegin(); iter != l.cend(); ++iter)
  {
    auto m = iter->toMap();
    QMap<QString,QVariant> map;
    map["file"] = m["file"].toString();
    map["read"] = m["read"];
    list.append(map);
  }
}
