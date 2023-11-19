#include "logger.h"
#include "functions.h"

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <mutex>

namespace com::sptci
{
  struct Logger
  {
    Logger() : outFile(logFile()), stream(&outFile)
    {
      outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
      fprintf(stdout, "Logging to file: %s\n", outFile.fileName().toLocal8Bit().constData());
    }

    void log(const QString& message)
    {
      std::lock_guard<std::mutex> lock(mutex);
      stream << message << Qt::endl;
    }

    ~Logger()
    {
      outFile.close();
      fprintf(stdout, "Closing log file: %s\n", outFile.fileName().toLocal8Bit().constData());
    }

    QFile outFile;
    QTextStream stream;
    std::mutex mutex;
  };

  QString logFile()
  {
    return QString("%1/pictureviewer.log").arg(com::sptci::tempDirectory());
  }

  void formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
  {
    static Logger logger;

    const auto localMsg = msg.toLocal8Bit();
    const auto ts = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    const auto time = ts.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
      {
        fprintf(stdout, "%s - %s Debug: %s (%s:%u, %s)\n",
          time.constData(), context.category,
          localMsg.constData(), context.file, context.line, context.function);
        const auto message = QString("%1 - %2 Debug: %3").arg(ts).arg(context.category).arg(msg);
        logger.log(message);
      }
        break;
    case QtInfoMsg:
      {
        fprintf(stdout, "%s - %s Info: %s (%s:%u, %s)\n",
          time.constData(), context.category,
          localMsg.constData(), context.file, context.line, context.function);
        const auto message = QString("%1 - %2 Info: %3").arg(ts).arg(context.category).arg(msg);
        logger.log(message);
      }
      break;
    case QtWarningMsg:
      {
        fprintf(stderr, "%s - %s Warning: %s (%s:%u, %s)\n",
          time.constData(), context.category,
          localMsg.constData(), context.file, context.line, context.function);
        const auto message = QString("%1 - %2 Warning: %3").arg(ts).arg(context.category).arg(msg);
        logger.log(message);
      }
      break;
    case QtCriticalMsg:
      {
        fprintf(stderr, "%s - %s Critical: %s (%s:%u, %s)\n",
          time.constData(), context.category,
          localMsg.constData(), context.file, context.line, context.function);
        const auto message = QString("%1 - %2 Critical: %3").arg(ts).arg(context.category).arg(msg);
        logger.log(message);
      }
      break;
    case QtFatalMsg:
      {
        fprintf(stderr, "%s - %s Fatal: %s (%s:%u, %s)\n",
          time.constData(), context.category,
          localMsg.constData(), context.file, context.line, context.function);
        const auto message = QString("%1 - %2 Fatal: %3").arg(ts).arg(context.category).arg(msg);
        logger.log(message);
        exit(1);
      }
    }
  }
}
