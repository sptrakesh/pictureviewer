#include "KeyPressFilter.h"
#include "MainWindow.h"

#include <QtGui/QKeyEvent>

using com::sptci::KeyPressFilter;

bool KeyPressFilter::eventFilter( QObject* obj, QEvent* event )
{
  using com::sptci::MainWindow;

  if ( event->type() == QEvent::KeyPress )
  {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
    MainWindow* mw = static_cast<MainWindow*>( parent() );

    switch ( keyEvent->key() )
    {
      case Qt::Key_Space:
        mw->play();
        return true;
      case Qt::Key_Delete:
        mw->removeFile();
        return true;
      default:
        return false;
    }
  }

  return QObject::eventFilter( obj, event );
}
