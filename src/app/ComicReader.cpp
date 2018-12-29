#include "ComicReader.h"
#include "ui_ComicReader.h"
#include "ComicListWidget.h"

#include <QtWidgets/QVBoxLayout>

using com::sptci::ComicReader;

ComicReader::ComicReader(QWidget* parent) : QWidget(parent), ui(new Ui::ComicReader)
{
  ui->setupUi(this);

  init();

  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window);
  setWindowTitle("Comic Reader");
}

ComicReader::~ComicReader()
{
  delete ui;
}

void ComicReader::init()
{
  ui->tabWidget->addTab(new ComicListWidget(this), tr("Comics"));
}
