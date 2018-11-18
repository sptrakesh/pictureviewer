#include "InfoViewer.h"
#include "ui_InfoViewer.h"

#include <QtCore/QStringBuilder>

using com::sptci::InfoViewer;

InfoViewer::InfoViewer( const QString& page, QWidget* parent ) :
  QFrame( parent ), ui( new Ui::InfoViewer )
{
  ui->setupUi( this );

  setAttribute( Qt::WA_DeleteOnClose );
  setAttribute( Qt::WA_GroupLeader );

  ui->textBrowser->setSearchPaths( QStringList() << ":/images" << ":/docs" );
  ui->textBrowser->setSource( page );
}


InfoViewer::~InfoViewer()
{
  delete ui;
}


void InfoViewer::showPage( const QString& page )
{
  QString path = ":/docs/" % page;
  InfoViewer* viewer = new InfoViewer( page );
  viewer->show();
}


void InfoViewer::updateWindowTitle()
{
  setWindowTitle( tr( "About: %1" ).arg( ui->textBrowser->documentTitle() ) );
}
