#include "FileList.h"

#include <QtCore/QFile>

using com::sptci::model::FileList;

FileList::FileList(QObject *parent) : QObject(parent) {}

void FileList::add(const QString& file)
{
  files.append(file);
}

void FileList::setIndex(int index)
{
  if (index >= files.count()) return;
  this->index = index;
}

const QString& FileList::at(int index)
{
  this->index = index;
  return files.at(index);
}

const QString& FileList::first()
{
  index = 0;
  return files.first();
}

const QString& FileList::next()
{
  if (++index >= files.count()) index = 0;
  return files.at(index);
}

const QString& FileList::previous()
{
  if (--index < 0) index = files.count() - 1;
  return files.at(index);
}

bool FileList::remove()
{
  if (index < 0) return false;
  const auto result = QFile::remove(current());
  if (result) files.removeAt(index);
  return result;
}
