#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirectoryScanner.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QFileInfo>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QImageReader>

using com::sptci::MainWindow;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  //ui->scrollArea->setBackgroundRole(QPalette::Dark);
  //ui->scrollArea->setWidgetResizable(true);
  ui->label->setBackgroundRole(QPalette::Base);

  timer.setInterval(5000);
  connect(&timer, &QTimer::timeout, this, &MainWindow::showImage);
}

MainWindow::~MainWindow()
{
  delete ui;
  for (auto* thread : threads)
  {
    thread->quit();
    thread->wait();
    thread->deleteLater();
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
  using com::sptci::DirectoryScanner;

  for (const auto& url : event->mimeData()->urls())
  {
    const auto fileName = url.toLocalFile();
    qDebug() << "Dropped file:" << fileName;

    const QFileInfo file{fileName};
    if (file.isDir())
    {
      auto scanner = new DirectoryScanner(file.absoluteFilePath());
      auto thread = new QThread;
      scanner->moveToThread(thread);
      connect(thread, &QThread::finished, scanner, &QObject::deleteLater);
      connect(this, &MainWindow::scan, scanner, &DirectoryScanner::scan);
      connect(this, &MainWindow::scanStop, scanner, &DirectoryScanner::stop);
      connect(scanner, &DirectoryScanner::file, this, &MainWindow::addFile);
      threads.append(thread);
      thread->start();
      if (thread->isRunning()) emit scan();
      play();
    }
    else
    {
    }
  }
}

void MainWindow::openDirectory()
{
}

void MainWindow::stopScanning()
{
  qDebug("Emitting scanStop");
  emit scanStop();
}

void MainWindow::play()
{
  timer.start();
}

void MainWindow::removeFile()
{
}

void MainWindow::aboutQt()
{
  qApp->aboutQt();
}

void MainWindow::addFile(const QString file)
{
  //ui->statusbar->showMessage(file);
  files.add(file);
  //QCoreApplication::processEvents();
}

void MainWindow::showImage()
{
  if (files.isEmpty()) return;
  const auto& file = files.next();

  QImageReader reader(file);
  reader.setAutoTransform(true);
  const QImage image = reader.read();

  if (image.isNull())
  {
    qDebug() << "Unable to display " << file;
    return;
  }

  auto pixmap = QPixmap::fromImage(image);
  if (pixmap.width() > pixmap.height() && pixmap.width() != this->width())
  {
    pixmap = pixmap.scaledToWidth(this->width());
  }
  else if (pixmap.height() > pixmap.width() && pixmap.height() != this->height())
  {
    pixmap = pixmap.scaledToHeight(this->height());
  }

  ui->label->setPixmap(pixmap);
  ui->statusbar->showMessage(
    QString( "%1 (%2/%3)" ).arg( file ).
      arg( files.currentIndex() + 1 ).
      arg( files.count() ) );
}
