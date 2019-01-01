#include "Reader.h"
#include "OutputDirectory.h"
#include "ScopedArchive.h"
#include "ScopedArchiveEntry.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(READER, "com::sptci::archive::Reader")

using com::sptci::archive::Reader;

Reader::Reader(const QString& file, QObject* parent) : QObject(parent), file(file) {}

QStringList Reader::extractAll(const OutputDirectory& outputDirectory)
{
  QStringList files;
  if (!QFile::exists(file)) return files;

  ScopedArchive archive(file);
  connect(&archive, &ScopedArchive::warning, this, &Reader::archiveWarning);
  connect(&archive, &ScopedArchive::error, this, &Reader::archiveError);

  while (hasMore)
  {
    ScopedArchiveEntry entry(outputDirectory.dir.absolutePath(), archive);
    connect(&entry, &ScopedArchiveEntry::warning, this, &Reader::entryWarning);
    connect(&entry, &ScopedArchiveEntry::error, this, &Reader::entryError);
    connect(&entry, &ScopedArchiveEntry::eof, this, &Reader::eof);
    auto file = entry.next();
    qDebug(READER) << "Unarchived file " << file;
    if (!file.isEmpty()) files.append(file);
  }

  return files;
}

void Reader::archiveWarning(int errorCode, QString message)
{
  qWarning(READER) << "Warning received while opening archive " << file
    << "\n" << message;
  emit error(errorCode, message);
}

void Reader::archiveError(int errorCode, QString message)
{
  qCritical(READER) << "Error received while opening archive " << file
    << "\n" << message;
  emit error(errorCode, message);
}

void Reader::entryWarning(int errorCode, QString message)
{
  qWarning(READER) << "Warning received while saving entry in " << file
    << "\n" << message;
  emit error(errorCode, message);
}

void Reader::entryError(int errorCode, QString message)
{
  qCritical(READER) << "Error received while saving entry in " << file
    << "\n" << message;
  emit error(errorCode, message);
}
