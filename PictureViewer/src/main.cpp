#include "app/MainWindow.h"

#include <QtWidgets/QApplication>

int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );
  a.setOrganizationName( "Rakesh Vidyadharan" );
  a.setOrganizationDomain( "rakeshv.org" );
  a.setApplicationName( "MoviePlayer" );

  com::sptci::MainWindow w;
  w.show();

  return a.exec();
}
