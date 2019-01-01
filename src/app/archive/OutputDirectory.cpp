#include "OutputDirectory.h"
#include "../functions.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QUuid>

using com::sptci::archive::OutputDirectory;

Q_LOGGING_CATEGORY(OUTPUT_DIRECTORY, "com::sptci::archive::OutputDirectory")

OutputDirectory::OutputDirectory()
{
  auto uuid = QUuid::createUuid().toString();
  uuid.replace("{", "").replace("}", "");
  auto name = QString("%1/%2").arg(tempDirectory()).arg(uuid);
  dir = QDir(name);
  if (!dir.exists()) dir.mkpath(".");
  qDebug(OUTPUT_DIRECTORY) << "Unarchiving files under " << dir.absolutePath();
}

OutputDirectory::~OutputDirectory()
{
  qDebug(OUTPUT_DIRECTORY) << "Removing unarchived files under " << dir.absolutePath();
  dir.removeRecursively();
}
