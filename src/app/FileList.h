#ifndef FILELIST_H
#define FILELIST_H

#include <QtCore/QObject>
#include <QtCore/QList>

namespace com::sptci
{
  class FileList : public QObject
  {
    Q_OBJECT
  public:
    explicit FileList(QObject* parent = nullptr);

    void add(const QString& file);

    bool isEmpty() const { return files.isEmpty(); }
    int currentIndex() const { return index; }
    void setIndex(int index);

    const QString& at(int index);
    const QString& first();
    const QString& next();
    const QString& previous();
    const QString& current() { return files.at(index); }

    bool remove();

    int count() const { return files.count(); }
    void clear() { files.clear(); }

  private:
    QList<QString> files;
    int index = -1;
  };
}

#endif // FILELIST_H
