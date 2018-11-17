#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirectoryScanner.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMimeData>
#include <QtCore/QFileInfo>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QImageReader>

#include <algorithm>
#include <iterator>

using com::sptci::MainWindow;

Q_LOGGING_CATEGORY( MAIN_WINDOW, "com.sptci.MainWindow" )

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->label->setBackgroundRole(QPalette::Base);

  timer.setInterval(ui->intervalSlider->value() * 1000);
  connect(&timer, &QTimer::timeout, this, &MainWindow::showImage);
}

MainWindow::~MainWindow()
{
  using std::begin;
  using std::end;

  timer.stop();

  std::for_each(begin(threads), end(threads), [](decltype(*begin(threads)) thread) {
    thread->quit();
    thread->wait(3000);
    thread->deleteLater();
  });

  delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  stopScanning();
  QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasUrls()) event->acceptProposedAction();
  else qWarning(MAIN_WINDOW) << "Ignoring drop for non-url type";
}

void MainWindow::dropEvent(QDropEvent* event)
{
  using com::sptci::DirectoryScanner;

  for (const auto& url : event->mimeData()->urls())
  {
    const auto fileName = url.toLocalFile();
    qInfo(MAIN_WINDOW) << "Dropped file:" << fileName;

    const QFileInfo file{fileName};
    if (file.isDir())
    {
      ui->actionStop_Scanning->setEnabled(true);
      auto scanner = new DirectoryScanner(file.absoluteFilePath());
      auto thread = new QThread;
      scanner->moveToThread(thread);
      connect(thread, &QThread::finished, scanner, &QObject::deleteLater);
      connect(this, &MainWindow::scan, scanner, &DirectoryScanner::scan);
      connect(this, &MainWindow::scanStop, scanner, &DirectoryScanner::stop);
      connect(scanner, &DirectoryScanner::file, this, &MainWindow::addFile);
      connect(scanner, &DirectoryScanner::finished, this, &MainWindow::scanFinished);
      threads.append(thread);
      thread->start();
      emit scan();
      play();
    }
    else if (file.isFile())
    {
      files.add(file.absoluteFilePath());
    }
    else
    {
      qWarning(MAIN_WINDOW) << "Unsupported file type:" << file.absoluteFilePath();
    }
  }
}

void MainWindow::openDirectory()
{
}

void MainWindow::stopScanning()
{
  qDebug(MAIN_WINDOW) << "Emitting scanStop";
  emit scanStop();
}

void MainWindow::play()
{
  if (playing) timer.stop();
  else timer.start();
  playing = !playing;
}

void MainWindow::setIndex(int index)
{
  files.setIndex(index);
}

void MainWindow::setInterval(int interval)
{
  timer.setInterval(interval * 1000);
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
  files.add(file);

  if (files.currentIndex() < 0)
  {
    showImage();
    QCoreApplication::processEvents();
    return;
  }

  if (!(files.count() % 10))
  {
    ui->indexSlider->setMaximum(files.count());
    QCoreApplication::processEvents();
  }
}

void MainWindow::scanFinished(const QString directory, int count)
{
  ui->actionStop_Scanning->setDisabled(true);
  ui->indexSlider->setMaximum(files.count());
  QCoreApplication::processEvents();
  qInfo(MAIN_WINDOW) << "Finished scanning " << count <<
    " image files in " << directory << " directory tree." ;
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
    ui->statusbar->showMessage(
      QString( "Unable to display %1 (%2/%3)" ).arg( file ).
        arg( files.currentIndex() + 1 ).
        arg( files.count() ) );
    return;
  }

  auto pixmap = QPixmap::fromImage(image);
  pixmap = pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->label->setPixmap(pixmap);
  setWindowFilePath(file);
  ui->statusbar->showMessage(
    QString( "%1 (%2/%3)" ).arg( file ).
      arg( files.currentIndex() + 1 ).
      arg( files.count() ) );
  ui->indexSlider->setValue(files.currentIndex());
}
