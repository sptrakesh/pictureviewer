#include "ComicListWidget.h"
#include "ui_ComicListWidget.h"
#include "functions.h"
#include "worker/ComicScanner.h"

#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtWidgets/QFileDialog>

using com::sptci::ComicListWidget;

ComicListWidget::ComicListWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ComicListWidget), model(new model::ComicList(this))
{
  ui->setupUi(this);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
}

ComicListWidget::~ComicListWidget()
{
  delete ui;
}

void ComicListWidget::addFile()
{
  const auto fileName = QFileDialog::getOpenFileName(this,
    tr("Open Comic"), com::sptci::documentsDirectory(),
    tr("Comics (*.cbr *.cbt *.cbz *.cb7)"));
  if (fileName.isEmpty()) return;
  model->addFile(fileName);
}

void ComicListWidget::removeFile()
{
  const auto index = ui->tableView->currentIndex();
  model->removeRow(index.row());
}

void ComicListWidget::scan()
{
  using worker::ComicScanner;

  const auto dir = QFileDialog::getExistingDirectory(this,
    tr("Under Directory"), com::sptci::documentsDirectory(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (dir.isEmpty()) return;

  auto worker = new ComicScanner(dir);
  auto thread = new QThread;
  worker->moveToThread(thread);

  connect(thread, &QThread::started, worker, &ComicScanner::run);
  connect(worker, &ComicScanner::comic, this, &ComicListWidget::addComic);
  connect(worker, &ComicScanner::finished, thread, &QThread::quit);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  connect(thread, &QThread::finished, worker, &ComicScanner::deleteLater);

  thread->start();
}

void ComicListWidget::showComic()
{
}

void ComicListWidget::addComic(QString file)
{
  model->addFile(file);
}
