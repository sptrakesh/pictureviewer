#ifndef KEYPRESSFILTER_H
#define KEYPRESSFILTER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>

namespace com::sptci
{
  class KeyPressFilter : public QObject
  {
    Q_OBJECT

  public:
    explicit KeyPressFilter( QObject* parent = nullptr ) : QObject( parent ) {}

  protected:
    bool eventFilter( QObject* obj, QEvent* event );
  };
}

#endif // KEYPRESSFILTER_H
