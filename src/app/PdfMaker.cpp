#include "PdfMaker.h"
#include "ui_PdfMaker.h"
#include "functions.h"
#include "MainWindow.h"
#include "PaperSizeModel.h"
#include "PdfEngine.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QPainter>
#include <QtGui/QPageSize>
#include <QtGui/QPdfWriter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <algorithm>

using com::sptci::PdfMaker;

Q_LOGGING_CATEGORY(PDF_MAKER, "com::sptci::PdfMaker")

PdfMaker::PdfMaker(const QString& file, QWidget* parent) :
    QWidget(parent), file(file), ui(new Ui::PdfMaker)
{
  ui->setupUi(this);

  auto model = new com::sptci::PaperSizeModel(this);
  ui->paperSize->setModel(model);
  ui->paperSize->setCurrentIndex(model->systemSize());

  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window);
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));
}

PdfMaker::~PdfMaker()
{
  delete ui;
}

void PdfMaker::saveAs()
{
  completed = true;
  QFileInfo fi(file);

  const auto directory = com::sptci::documentsDirectory();
  const auto fn = QString("%1/%2.pdf").arg(directory).arg(fi.baseName());

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      fn,
      tr("Documents (*.pdf)"));
  if (fileName.isEmpty())
  {
    qDebug(PDF_MAKER) << "User cancelled file dialogue.";
    return;
  }

  ui->cancel->setEnabled(false);
  ui->saveAs->setEnabled(false);
  save(fileName);
}

void PdfMaker::updateProgress(int index, int total, QString file)
{
  if (!progress)
  {
    qWarning(PDF_MAKER) << "No progress dialogue to update";
    return;
  }

  progress->setValue(index);
  progress->setLabelText(QString("Added page... - (%1/%2)").arg(index).arg(total));
  qInfo(PDF_MAKER) << "Added file " << file;
}

void PdfMaker::progressCancelled()
{
  progress->hide();
  progress = nullptr;
  ui->cancel->setEnabled(true);
  ui->saveAs->setEnabled(true);
  completed = false;
  raise();
}

void PdfMaker::finished()
{
  qInfo(PDF_MAKER) << "Saved PDF " << dest;
  if (completed)
  {
    if (ui->showFile->isChecked()) com::sptci::showFile(dest);
    close();
  }
}

void PdfMaker::save(const QString& destination)
{
  const auto selected = ui->paperSize->currentIndex();
  const auto index = ui->paperSize->model()->index(selected, 1);
  const auto data = ui->paperSize->model()->data(index);
  raise();

  switch (ui->file->currentIndex())
  {
    case 0: // File when pdf window was displayed
      saveFile(destination);
      break;
    case 1: // All files loaded
      saveAll(destination);
      break;
  }
}

void PdfMaker::saveFile(const QString& destination)
{
  const auto selected = ui->paperSize->currentIndex();
  const auto model = dynamic_cast<PaperSizeModel*>(ui->paperSize->model());
  const auto sizeId = model->sizeId(selected);
  qDebug(PDF_MAKER) << "Paper size " << sizeId;
  const QStringList files(file);

  PdfEngine engine(std::make_unique<PdfSpec>(
    sizeId, destination, ui->title->text(), ui->creator->text(),
    ui->compression->value()), files);
  engine.run();

  qInfo(PDF_MAKER) << "Saved PDF " << destination;
  close();
}

void PdfMaker::saveAll(const QString& destination)
{
  dest = destination;
  const auto list = files();
  if (list.isEmpty()) return;

  const auto selected = ui->paperSize->currentIndex();
  const auto model = dynamic_cast<PaperSizeModel*>(ui->paperSize->model());
  const auto sizeId = model->sizeId(selected);

  auto process = new PdfEngine(std::make_unique<PdfSpec>(
    sizeId, destination, ui->title->text(), ui->creator->text(),
    ui->compression->value()), list);
  auto thread = new QThread;
  process->moveToThread(thread);

  connect(thread, &QThread::started, process, &PdfEngine::run);
  connect(process, &PdfEngine::progress, this, &PdfMaker::updateProgress);
  connect(process, &PdfEngine::finished, this, &PdfMaker::finished);
  connect(process, &PdfEngine::finished, thread, &QThread::quit);
  connect(process, &PdfEngine::finished, process, &PdfEngine::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  progress = std::make_unique<QProgressDialog>(tr("Creating PDF..."),
    "Abort Process", 0, list.size());
  connect(progress.get(), &QProgressDialog::canceled, process, &PdfEngine::stop);
  connect(progress.get(), &QProgressDialog::canceled, this, &PdfMaker::progressCancelled);

  thread->start();
}

QStringList PdfMaker::files()
{
  auto sorted = [this]() {
    QMap<QString,bool> map;

    const MainWindow* mw = dynamic_cast<MainWindow*>(parent());
    if (!mw)
    {
      qWarning(PDF_MAKER) << "Parent object not instance of com::sptci::MainWindow!";
      return map.keys();
    }

    std::for_each(mw->cbegin(), mw->cend(),
      [&map](const QString& fileName) {
        map.insert(fileName, false);
      });

    return map.keys();
  };

  auto unsorted = [this]() {
    QStringList list;

    const MainWindow* mw = dynamic_cast<MainWindow*>(parent());
    if (!mw)
    {
      qWarning(PDF_MAKER) << "Parent object not instance of com::sptci::MainWindow!";
      return list;
    }

    std::for_each(mw->cbegin(), mw->cend(),
      [&list](const QString& fileName) { list.append(fileName); });

    return list;
  };

  return (ui->sortFiles->isChecked()) ? sorted() : unsorted();
}
