#include "app/MainWindow.h"

#include <QtCore/QDateTime>
#include <QtWidgets/QApplication>

namespace com::sptci
{
  void formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
  {
    const auto localMsg = msg.toLocal8Bit();
    const auto time = QDateTime::currentDateTime().toString(Qt::ISODateWithMs).toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
      fprintf(stdout, "%s - %s Debug: %s (%s:%u, %s)\n",
        time.constData(), context.category,
        localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtInfoMsg:
      fprintf(stdout, "%s - %s Info: %s (%s:%u, %s)\n",
        time.constData(), context.category,
        localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtWarningMsg:
      fprintf(stderr, "%s - %s Warning: %s (%s:%u, %s)\n",
        time.constData(), context.category,
        localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtCriticalMsg:
      fprintf(stderr, "%s - %s Critical: %s (%s:%u, %s)\n",
        time.constData(), context.category,
        localMsg.constData(), context.file, context.line, context.function);
      break;
    case QtFatalMsg:
      fprintf(stderr, "%s - %s Fatal: %s (%s:%u, %s)\n",
        time.constData(), context.category,
        localMsg.constData(), context.file, context.line, context.function);
      break;
    }

    /*
QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
        QFile outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
     */
  }
}

int main( int argc, char *argv[] )
{
  qInstallMessageHandler(com::sptci::formatMessage);
  setbuf(stdout, nullptr);

  QApplication a( argc, argv );
  a.setOrganizationName( "Rakesh Vidyadharan" );
  a.setOrganizationDomain( "com.sptci" );
  a.setApplicationName( "Picture Viewer" );

  com::sptci::MainWindow w;
  w.show();

  return a.exec();
}
