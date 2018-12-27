#ifndef COMICBOOKSPEC_H
#define COMICBOOKSPEC_H

#include <QtCore/QString>

namespace com::sptci::model
{
  struct ComicBookSpec
  {
    QString outFile;
    int format;
    bool sortFiles;
    bool showFile;
  };
}

#endif // COMICBOOKSPEC_H
