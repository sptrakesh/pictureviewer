#include "app/MainWindow.h"
#include "app/logger.h"
#if defined(Q_OS_MACOS)
#include "log/NanoLog.hpp"
#endif

#include <QtWidgets/QApplication>

int main( int argc, char *argv[] )
{
  qInstallMessageHandler( com::sptci::formatMessage );
  setbuf(stdout, nullptr);

  QApplication a( argc, argv );
  a.setOrganizationName( "Rakesh Vidyadharan" );
  a.setOrganizationDomain( "com.sptci" );
  a.setApplicationName( "Picture Viewer" );

#if defined(Q_OS_MACOS)
  nanolog::initialize( nanolog::GuaranteedLogger(), "/tmp/", "PictureViewer", false );
#endif
  com::sptci::MainWindow w;
  w.show();

  return a.exec();
}
