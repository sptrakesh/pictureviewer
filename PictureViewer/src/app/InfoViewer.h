/* $Id: InfoViewer.h 5706 2014-11-27 19:10:55Z rakesh $ */
#ifndef INFOVIEWER_H
#define INFOVIEWER_H

#include <QtWidgets/QFrame>

namespace com::sptci
{
  namespace Ui { class InfoViewer; }

  class InfoViewer : public QFrame
  {
    Q_OBJECT

  public:
    InfoViewer( const QString& page, QWidget* parent = nullptr );
    ~InfoViewer();

    static void showPage( const QString& page );

  public slots:
    void updateWindowTitle();

  private:
    Ui::InfoViewer* ui;
  };
}

#endif // INFOVIEWER_H
