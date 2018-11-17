#include "FileList.h"

using com::sptci::FileList;

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
