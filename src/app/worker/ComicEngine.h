#ifndef COMICENGINE_H
#define COMICENGINE_H

#include "model/ComicBookSpec.h"
#include <QArchive.hpp>

#include <QtCore/QDir>
#include <QtCore/QStringList>

namespace com::sptci::worker
{

  class ComicEngine : public QObject
  {
    Q_OBJECT
  public:
    explicit ComicEngine(QStringList::ConstIterator begin,
      QStringList::ConstIterator end, const com::sptci::model::ComicBookSpec& spec,
      QObject* parent = nullptr);
    explicit ComicEngine(const QDir& dir,
      const com::sptci::model::ComicBookSpec& spec,
      QObject* parent = nullptr);

    QString create();

  private:
    static QStringList filesFromIterators(bool sort,
      QStringList::ConstIterator begin, QStringList::ConstIterator end);
    static QStringList filesFromDir(bool sort, const QDir& dir);

  private:
    const QStringList files;
    const com::sptci::model::ComicBookSpec spec;
  };

} // namespace com::sptci::worker

#endif // COMICENGINE_H
