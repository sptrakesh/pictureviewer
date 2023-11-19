#include "Writer.h"

#include <QtCore/QFileInfo>

#include <archive.h>

namespace com::sptci::archive
{
  struct WriteArchive
  {
    WriteArchive(const QString& file) : target(archive_write_new())
    {
      QFileInfo fi(file);
      if ("cbz" == fi.suffix()) archive_write_set_format_zip(target);
      if ("cbt" == fi.suffix()) archive_write_set_format_ustar(target);
      if ("cb7" == fi.suffix()) archive_write_set_format_7zip(target);

      archive_write_open_filename(target, file.toStdString().c_str());
      archive_write_set_bytes_in_last_block(target, 1);
    }

    ~WriteArchive()
    {
      archive_write_close(target);
      archive_write_free(target);
    }

    struct archive* target;
  };
}

using com::sptci::archive::Writer;

Writer::Writer(const QString& output, QObject* parent) :
  QObject(parent), outputFile(output) {}

Writer::~Writer()
{
}
