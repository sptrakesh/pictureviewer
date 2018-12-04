#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

namespace com::sptci
{
  inline QString picturesDirectory()
  {
    const auto picturesLocations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    return picturesLocations.isEmpty() ?
      QDir::homePath() : picturesLocations.last();
  }

  inline QString documentsDirectory()
  {
    const auto documentsLocations =
      QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    return documentsLocations.isEmpty() ?
      QDir::homePath() : documentsLocations.last();
  }
}

  #endif // FUNCTIONS_H
