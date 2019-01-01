#include "ScopedArchiveEntry.h"
#include "ScopedArchive.h"

#include <QtCore/QLoggingCategory>

#include <archive.h>
#include <archive_entry.h>

using com::sptci::archive::ScopedArchiveEntry;

Q_LOGGING_CATEGORY(SCOPED_ARCHIVE_ENTRY, "com::sptci::archive::ScopedArchiveEntry")

ScopedArchiveEntry::ScopedArchiveEntry(const QString& directory, const ScopedArchive& s) :
  QObject(nullptr), directory(directory), source(s.getArchive()),
  destination(archive_write_disk_new())
{
  auto flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;
  archive_write_disk_set_options(destination, flags);
  archive_write_disk_set_standard_lookup(destination);
}

ScopedArchiveEntry::~ScopedArchiveEntry()
{
  if (!source) return;
  qDebug(SCOPED_ARCHIVE_ENTRY) << "Clearning up destination archive handles";
  archive_write_finish_entry(destination);
  archive_write_close(destination);
  archive_write_free(destination);
}

QString ScopedArchiveEntry::next()
{
  struct archive_entry* entry;
  auto r = archive_read_next_header(source, &entry);
  if (ARCHIVE_EOF == r)
  {
    emit eof();
    return QString();
  }

  if (!check(r)) return QString();

  const QString fileName = archive_entry_pathname(entry);
  const QString outFile = QString("%1/%2").arg(directory).arg(fileName);
  archive_entry_set_pathname(entry, outFile.toStdString().c_str());

  if (!check(archive_write_header(destination, entry))) return QString();

  if (archive_entry_size(entry)) check(copy());
  return fileName;
}

int ScopedArchiveEntry::copy()
{
  const void *buff;
  size_t size;
  __LA_INT64_T offset;

  while (true)
  {
    auto code = archive_read_data_block(source, &buff, &size, &offset);
    if (code == ARCHIVE_EOF || code == ARCHIVE_OK) return ARCHIVE_OK;
    auto bytes = archive_write_data_block(destination, buff, size, offset);
    if (bytes != ARCHIVE_OK) return static_cast<int>(bytes);
  }
}

bool ScopedArchiveEntry::check(const int errorCode)
{
  if (ARCHIVE_OK == errorCode) return true;

  const QString message = archive_error_string(source);
  switch (errorCode)
  {
    case ARCHIVE_WARN:
      emit warning(errorCode, message);
      qWarning(SCOPED_ARCHIVE_ENTRY) << "Error code: " << errorCode << ", message: " << message;
      return true;
    case ARCHIVE_FATAL:
      emit error(errorCode, message);
      qCritical(SCOPED_ARCHIVE_ENTRY) << "Error code: " << errorCode << ", message: " << message;
      break;
  }

  return false;
}
