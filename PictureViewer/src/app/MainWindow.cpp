#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirectoryScanner.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QFileInfo>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QImageReader>

#include <algorithm>
#include <iterator>

using com::sptci::MainWindow;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->label->setBackgroundRole(QPalette::Base);

  timer.setInterval(5000);
  connect(&timer, &QTimer::timeout, this, &MainWindow::showImage);
}

MainWindow::~MainWindow()
{
  using std::begin;
  using std::end;

  timer.stop();
  delete ui;

  std::for_each(begin(threads), end(threads), [](decltype(*begin(threads)) thread) {
    thread->quit();
    thread->wait();
    thread->deleteLater();
  });
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasUrls()) event->acceptProposedAction();
  else qWarning() << "Ignoring drop for non-url type";
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
    else if (file.isFile())
    {
      files.add(file.absoluteFilePath());
    }
    else
    {
      qWarning() << "Unsupported file type:" << file.absoluteFilePath();
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
  auto scaleDown = [this](const QPixmap& pixmap) -> QPixmap {
    QPixmap pm = pixmap;
    if (pixmap.width() > width())
    {
      qDebug() << "Scaling pixmap width down from " << pixmap.width() << " to " << width();
      pm = pixmap.scaledToWidth(width());
    }

    return pm;
  };

  auto scaleUp = [this](const QPixmap& pixmap) -> QPixmap
  {
    QPixmap pm = pixmap;
    if (pixmap.height() < height())
    {
      qDebug() << "Scaling pixmap height up from " << pixmap.height() << " to " << height();
      pm = pixmap.scaledToHeight(height());
    }

    return pm;
  };

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
  const auto w = width();
  const auto h = height();
  qDebug() << "Pixmap: " << pixmap.width() << "x" << pixmap.height() <<
      "; window: " << width() << "x" << height();

  pixmap = scaleUp(pixmap);
  pixmap = scaleDown(pixmap);

  qDebug() << "Resized pixmap: " << pixmap.width() << "x" << pixmap.height() <<
      "; window: " << width() << "x" << height();

  ui->label->setPixmap(pixmap);
  setFixedSize(w, h);
  setWindowFilePath(file);
  ui->statusbar->showMessage(
    QString( "%1 (%2/%3)" ).arg( file ).
      arg( files.currentIndex() + 1 ).
      arg( files.count() ) );
}
