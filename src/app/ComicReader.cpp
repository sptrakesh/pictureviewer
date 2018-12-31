#include "ComicReader.h"
#include "ui_ComicReader.h"
#include "ComicListWidget.h"
#include "ComicDisplayWidget.h"

#include <QtCore/QDebug>
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

void ComicReader::displayFile(QString file)
{
  qDebug() << "ComicReader - Received display file event " << file;
  ui->tabWidget->setCurrentIndex(1);
  emit display(file);
}

void ComicReader::init()
{
  auto clw = new ComicListWidget(this);
  connect(clw, &ComicListWidget::display, this, &ComicReader::displayFile);
  ui->tabWidget->addTab(clw, tr("Comics"));

  auto cdw = new ComicDisplayWidget(this);
  connect(this, &ComicReader::display, cdw, &ComicDisplayWidget::display);
  ui->tabWidget->addTab(cdw, tr("Read"));
}
