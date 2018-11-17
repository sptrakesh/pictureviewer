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
    const QString& at(int index);
    const QString& first();
    const QString& next();
    const QString& previous();

    int count() const { return files.count(); }
    void clear() { files.clear(); }

  private:
    int index = 0;
    QList<QString> files;
  };
}

#endif // FILELIST_H
