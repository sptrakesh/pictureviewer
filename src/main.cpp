#include "app/MainWindow.h"
#include "app/logger.h"

#include <QtWidgets/QApplication>

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
