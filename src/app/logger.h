#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore/QtMsgHandler>

namespace com::sptci
{
  QString logFile();
  void formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);
}

#endif // LOGGER_H
