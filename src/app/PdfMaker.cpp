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

  setWindowFlags(Qt::Window);
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));
}

PdfMaker::~PdfMaker()
{
  if (thread)
  {
    thread->quit();
    thread->wait(3000);
    thread->deleteLater();
  }

  delete ui;
}

void PdfMaker::saveAs()
{
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

#if defined(Q_OS_WIN)
  if (QFileInfo(fileName).exists())
  {
    const auto result = QMessageBox::question(this,
        tr("Over-write file?"),
        QString("File %1 exists").arg(fileName));

    if (QMessageBox::Yes == result)
    {
      save(fileName);
      return;
    }
  }
#endif

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
  progress->setLabelText(QString("Creating PDF... - (%1/%2)").arg(index).arg(total));
  qInfo(PDF_MAKER) << "Added file " << file;
}

void PdfMaker::progressCancelled()
{
  progress->hide();
  delete progress;
  progress = nullptr;
  ui->saveAs->setEnabled(true);
}

void PdfMaker::finished()
{
  qInfo(PDF_MAKER) << "Saved PDF " << dest;
  close();
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
  const auto index = ui->paperSize->model()->index(selected, 1);
  const auto data = ui->paperSize->model()->data(index);

  auto engine = PdfEngine(std::make_unique<PdfSpec>(data.toInt(), destination));
  engine.create(file);

  qInfo(PDF_MAKER) << "Saved PDF " << destination;
  close();
}

void PdfMaker::saveAll(const QString& destination)
{
  dest = destination;
  const auto list = files();
  if (list.isEmpty()) return;

  const auto selected = ui->paperSize->currentIndex();
  const auto index = ui->paperSize->model()->index(selected, 1);
  const auto data = ui->paperSize->model()->data(index);

  auto process = new PdfEngine(std::make_unique<PdfSpec>(data.toInt(), destination));
  thread = new QThread(this);
  process->moveToThread(thread);

  connect(thread, &QThread::finished, process, &QObject::deleteLater);
  connect(this, &PdfMaker::start, process, &PdfEngine::run);
  connect(process, &PdfEngine::progress, this, &PdfMaker::updateProgress);
  connect(process, &PdfEngine::finished, this, &PdfMaker::finished);
  connect(process, &PdfEngine::finished, thread, &QThread::quit);

  if (progress) delete progress;
  progress = new QProgressDialog(tr("Creating PDF..."),
    "Abort Process", 0, list.size(), this);
  connect(progress, &QProgressDialog::canceled, process, &PdfEngine::stop);
  connect(progress, &QProgressDialog::canceled, this, &PdfMaker::progressCancelled);

  thread->start();
  emit start(list);
}

QList<QString> PdfMaker::files()
{
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
}
