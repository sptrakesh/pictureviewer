#include "app/MainWindow.h"

#include <QtCore/QDateTime>
#include <QtGui/QScreen>
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
  }
}

int main( int argc, char *argv[] )
{
  qInstallMessageHandler(com::sptci::formatMessage);
  setbuf(stdout, nullptr);
  QApplication a( argc, argv );
  a.setOrganizationName( "Rakesh Vidyadharan" );
  a.setOrganizationDomain( "rakeshv.org" );
  a.setApplicationName( "Picture Viewer" );

  com::sptci::MainWindow w;
  const auto screenrect = a.primaryScreen()->geometry();
  w.move(screenrect.left(), screenrect.top());
  w.setFixedSize(screenrect.width(), static_cast<int>(0.95 *screenrect.height()));
  w.show();

  return a.exec();
}
