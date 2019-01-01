#ifndef OUTPUTDIRECTORY_H
#define OUTPUTDIRECTORY_H

#include <QtCore/QDir>

namespace com::sptci::archive
{

  struct OutputDirectory
  {
    OutputDirectory();
    ~OutputDirectory();

    OutputDirectory(const OutputDirectory&) = delete;
    OutputDirectory& operator=(const OutputDirectory&) = delete;

    QDir dir;
  };

} // namespace com::sptci::archive

#endif // OUTPUTDIRECTORY_H
