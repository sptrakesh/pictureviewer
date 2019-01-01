#include "ScopedArchive.h"

#include <QtCore/QLoggingCategory>

#include <archive.h>

using com::sptci::archive::ScopedArchive;

Q_LOGGING_CATEGORY(SCOPED_ARCHIVE, "com::sptci::archive::ScopedArchive")

ScopedArchive::ScopedArchive(const QString& file) :
    QObject(nullptr), source(archive_read_new())
{
  check(archive_read_support_format_all(source));
  //check(archive_read_support_filter_all(source));
  check(archive_read_support_format_raw(source));
  check(archive_read_open_filename(source, file.toStdString().c_str(), 10240));
}

ScopedArchive::~ScopedArchive()
{
  if (!source) return;
  qDebug(SCOPED_ARCHIVE) << "Clearning up source archive handles";
  archive_read_close(source);
  archive_read_free(source);
}

void ScopedArchive::check(const int errorCode)
{
  if (ARCHIVE_OK == errorCode) return;

  const QString message = archive_error_string(source);
  switch (errorCode)
  {
    case ARCHIVE_WARN:
      emit warning(errorCode, message);
      qWarning(SCOPED_ARCHIVE) << "Error code: " << errorCode << ", message: " << message;
      break;
    case ARCHIVE_FATAL:
      emit error(errorCode, message);
      qCritical(SCOPED_ARCHIVE) << "Error code: " << errorCode << ", message: " << message;
      break;
  }
}
