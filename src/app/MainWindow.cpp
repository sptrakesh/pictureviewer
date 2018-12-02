#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirectoryScanner.h"
#include "InfoViewer.h"
#include "ExifWindow.h"
#include "Watermark.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMimeData>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QImageReader>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <algorithm>
#include <iterator>

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

using com::sptci::MainWindow;

Q_LOGGING_CATEGORY(MAIN_WINDOW, "com::sptci::MainWindow")

int8_t MainWindow::WINDOW_INDEX = -1;
const QString MainWindow::RECENT_FILES = "recentFiles";
const QString MainWindow::INTERVAL = "interval";
const QString MainWindow::DISPLAY_SLEEP = "displaySleep";

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  index = ++WINDOW_INDEX;
  qInfo(MAIN_WINDOW) << "Creating MainWindow with index: " << WINDOW_INDEX;
  ui->setupUi(this);
  ui->label->setBackgroundRole(QPalette::Base);

  createRecent();
  initInterval();
  initDisplaySleep();

  const auto title = QString("%1 : %2").arg(windowTitle()).arg(index);
  setWindowTitle(title);

  ui->statusbar->setStyleSheet("QStatusBar{color:black}");
  intervalTextWidget = new TextWidget(QString("%1 : %2s").
      arg(tr("Playback Interval")).
      arg(ui->intervalSlider->value()), this);
  initialised = true;
}

MainWindow::~MainWindow()
{
  using std::begin;
  using std::end;

  timer.stop();

  std::for_each(begin(threads), end(threads), [](auto thread) {
    thread->quit();
    thread->wait(3000);
    thread->deleteLater();
  });

#if defined(Q_OS_MAC)
  if (index == 0 && QProcess::Running == caffeinate.state())
  {
    qInfo(MAIN_WINDOW) << "Killing caffeinate from MainWindow with index: " << WINDOW_INDEX;
    caffeinate.kill();
  }
#endif

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
  for (const auto& url : event->mimeData()->urls())
  {
    const auto fileName = url.toLocalFile();
    qInfo(MAIN_WINDOW) << "Dropped file:" << fileName;

    const QFileInfo file(fileName);
    if (file.isDir())
    {
      processDirectory(fileName);
    }
    else if (file.isFile())
    {
      files.add(file.absoluteFilePath());
      if (files.currentIndex() < 0) files.next();
      displayImage(file.absoluteFilePath());
    }
    else
    {
      qWarning(MAIN_WINDOW) << "Unsupported file type:" << file.absoluteFilePath();
    }
  }
}

bool MainWindow::event(QEvent* event)
{
  switch (event->type())
  {
  case QEvent::Show:
      intervalTextWidget->show();
      QTimer::singleShot(50, this, &MainWindow::positionTextWidget);
      break;
  case QEvent::WindowActivate:
  case QEvent::Resize:
  case QEvent::Move:
      positionTextWidget();
      break;
  default:
      break;
  }

  return QMainWindow::event(event);
}

void MainWindow::openDirectory()
{
  const auto picturesLocations =
    QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  const auto directory = picturesLocations.isEmpty() ?
    QDir::currentPath() : picturesLocations.last();

  const auto fileName = QFileDialog::getExistingDirectory(
        this, tr("Select Directory"), directory);
  if (fileName.isNull() || fileName.isEmpty()) return;

  processDirectory(fileName);
}

void MainWindow::newWindow()
{
  auto w = new MainWindow(this);

  const auto desktop = qApp->desktop();
  const auto dg = desktop->availableGeometry();
  const auto cwg = geometry();
  auto x = cwg.left();
  auto y = cwg.top();

  auto wg = w->geometry();
  wg.moveLeft(((x + 50) > dg.bottomRight().x()) ? 25 : x + 50);
  wg.moveTop((( y + 50 ) > dg.bottom()) ? 25 : y + 50);

  w->setGeometry(wg);
  w->show();
}

void MainWindow::play()
{
  ui->actionPlay->setEnabled(true);

  if (playing) pause();
  else playback();

  playing = !playing;
}

void MainWindow::first()
{
  displayImage(files.first());
}

void MainWindow::previous()
{
  displayImage(files.previous());
}

void MainWindow::next()
{
  displayImage(files.next());
}

void MainWindow::stopScanning()
{
  qDebug(MAIN_WINDOW) << "Emitting scanStop";
  emit scanStop();
}

void MainWindow::displaySleep()
{
  const auto flag =  ui->actionDisplay_sleep->isChecked();

  if (initialised)
  {
    QSettings settings;
    settings.setValue(DISPLAY_SLEEP, flag);
    qInfo(MAIN_WINDOW()) << "Setting displaySleep preference to " << flag;
  }

  if (flag && index == 0)
  {
#if defined(Q_OS_MAC)
    if (QProcess::Running == caffeinate.state())
    {
      qDebug(MAIN_WINDOW()) << "Killing caffeinate process";
      caffeinate.kill();
    }
#elif defined(Q_OS_WIN)
    SetThreadExecutionState(ES_CONTINUOUS);
#endif
  }
  else if (index == 0)
  {
#if defined(Q_OS_MAC)
    qDebug(MAIN_WINDOW()) << "Starting caffeinate process";
    QStringList arguments;
    arguments << "-d";
    caffeinate.start("/usr/bin/caffeinate", arguments);
#elif defined(Q_OS_WIN)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif
  }
}

void MainWindow::showFile()
{
  if (files.currentIndex() < 0) return;
  const auto filePath = files.current();

#if defined(Q_OS_MAC)
  QStringList args;
  args << "-e";
  args << "tell application \"Finder\"";
  args << "-e";
  args << "activate";
  args << "-e";
  args << "select POSIX file \"" + filePath + "\"";
  args << "-e";
  args << "end tell";
  QProcess::startDetached( "osascript", args );
#elif defined(Q_OS_WIN)
  QStringList args;
  args << "/select," << QDir::toNativeSeparators(filePath);
  QProcess::startDetached("explorer", args);
#endif
}

void MainWindow::removeFile()
{
  if (files.currentIndex() < 0) return;
  const auto status = playing;

  if (status) pause();

  const auto file = files.current();
  auto message = QString("%1\n%2%3%4").
    arg("Delete selected file?\n").
    arg("File (").arg(file).
    arg(") will be removed permanently from the filesystem!");
  int response = QMessageBox::warning(this, tr("Delete File?"),
    message, QMessageBox::No, QMessageBox::Yes);

  if (response == QMessageBox::Yes)
  {
    if (!files.remove())
    {
      message = QString("%1%2%3\n%4").
        arg("File (").arg(file).
        arg(") could not be removed from the filesystem!").
        arg("You may not have sufficient privileges to remove files.");
      QMessageBox::information(this, "Delete File", message, QMessageBox::Close);
    }
  }

  if (status) playback();
}

void MainWindow::setIndex(int index)
{
  files.setIndex(index);
  if (playing) return;

  displayImage(files.current());
}

void MainWindow::setInterval(int interval)
{
  if (initialised)
  {
    QSettings settings;
    settings.setValue(INTERVAL, interval);
    qInfo(MAIN_WINDOW()) << "Setting timer interval preference to " << interval;

    delete intervalTextWidget;
    intervalTextWidget = new TextWidget(QString("%1s").arg(interval), this);
    intervalTextWidget->show();
    positionTextWidget();
  }
  timer.setInterval(interval * 1000);
}

void MainWindow::viewExif()
{
  if (files.currentIndex() < 0) return;

  const auto flag = playing;
  if (playing) pause();

  const auto file = files.current();
  auto window = new ExifWindow(file, this);
  window->show();

  if (flag) playback();
}

void MainWindow::watermark()
{
  if (files.currentIndex() < 0) return;

  const auto flag = playing;
  if (playing) pause();

  auto w = new com::sptci::Watermark(files.current(), this);
  w->show();

  if (flag) playback();
}

void MainWindow::about()
{
  com::sptci::InfoViewer::showPage("player.html");
}

void MainWindow::aboutQt()
{
  qApp->aboutQt();
}

void MainWindow::quit()
{
  if (index) qobject_cast<QMainWindow*>(parent())->close();
  else close();
}

void MainWindow::positionTextWidget()
{
  if (intervalTextWidget->width() <= ui->label->width() &&
      intervalTextWidget->height() <= ui->label->height())
  {
    intervalTextWidget->setWindowOpacity(1); // Show the widget
    QPoint p = ui->label->mapToGlobal(ui->label->pos());
    int x = p.x() + (ui->label->width() - intervalTextWidget->width()) / 2;
    int y = p.y() + (ui->label->height() - intervalTextWidget->height()) / 2;
    intervalTextWidget->move(x, y);
    intervalTextWidget->raise();
  }
  else
  {
    intervalTextWidget->setWindowOpacity(0); // Hide the widget
  }

  QTimer::singleShot(5000, intervalTextWidget, &TextWidget::close);
}

void MainWindow::openRecent()
{
  QAction* action = qobject_cast<QAction*>( sender() );
  if (action)
  {
    const QString& file = action->data().toString();
    processDirectory(file);
  }
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
  displayImage(file);
}

QString MainWindow::trimTrailingDirectorySeparator(const QString& input)
{
  auto fileName = input;
  if (fileName.endsWith("/"))
  {
    const auto pos = fileName.lastIndexOf(QChar('/'));
    fileName = fileName.left(pos);
  }

  if (fileName.endsWith("\\"))
  {
    const auto pos = fileName.lastIndexOf(QChar('\\'));
    fileName = fileName.left(pos);
  }

  return fileName;
}

int MainWindow::interval()
{
  QSettings settings;
  return (settings.contains(INTERVAL)) ?
      settings.value(INTERVAL).toInt() :
      ui->intervalSlider->value();
}

void MainWindow::initInterval()
{
  const auto time = interval();
  timer.setInterval(time * 1000);
  ui->intervalSlider->setValue(time);
  connect(&timer, &QTimer::timeout, this, &MainWindow::showImage);
}

bool MainWindow::sleepFlag()
{
  QSettings settings;
  return settings.value(DISPLAY_SLEEP, true).toBool();
}

void MainWindow::initDisplaySleep()
{
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
  const bool flag = sleepFlag();
  qDebug(MAIN_WINDOW) << "Stored display sleep flag: " << flag;
  ui->actionDisplay_sleep->setChecked(flag);
#else
  ui->actionDisplay_sleep->setEnabled(false);
#endif
}

void MainWindow::processDirectory(const QString& filename)
{
  const auto trimmed = trimTrailingDirectorySeparator(filename);
  ui->actionStop_Scanning->setEnabled(true);
  addRecent(trimmed);

  const QFileInfo file(filename);
  auto scanner = new DirectoryScanner(file.absoluteFilePath());
  auto thread = new QThread;
  scanner->moveToThread(thread);
  connect(thread, &QThread::finished, scanner, &QObject::deleteLater);
  connect(this, &MainWindow::scan, scanner, &DirectoryScanner::scan);
  connect(this, &MainWindow::scanStop, scanner, &DirectoryScanner::stop);
  connect(scanner, &DirectoryScanner::file, this, &MainWindow::addFile);
  connect(scanner, &DirectoryScanner::finished, this, &MainWindow::scanFinished);
  connect(scanner, &DirectoryScanner::finished, thread, &QThread::quit);
  threads.append(thread);
  thread->start();

  emit scan();
  if (!playing) play();
}

void MainWindow::addRecent(const QString& fileName)
{
  QSettings settings;
  QStringList files = settings.value(RECENT_FILES).toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MaxRecentFiles) files.removeLast();

  settings.setValue(RECENT_FILES, files);
  updateRecent();
}

void MainWindow::createRecent()
{
  if ( ! ui->actionRecent->menu() )
  {
    ui->actionRecent->setMenu( new QMenu( this ) );
  }

  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    recentFiles[i] = new QAction(this);
    recentFiles[i]->setCheckable(true);
    recentFiles[i]->setVisible(false);
    connect(recentFiles[i], &QAction::triggered, this, &MainWindow::openRecent);
    ui->actionRecent->menu()->addAction(recentFiles[i]);
  }

  updateRecent();
}

void MainWindow::updateRecent()
{
  QSettings settings;
  QStringList dirs = settings.value(RECENT_FILES).toStringList();

  int numRecentFiles = qMin(dirs.size(), static_cast<int>(MaxRecentFiles));

  int index = 0;
  for (int i = 0; i < numRecentFiles; ++i)
  {
    QString text = tr("&%1: %2").arg(index + 1).arg(dirs[i]);
    recentFiles[index]->setText(text);
    recentFiles[index]->setData(dirs[i]);
    recentFiles[index]->setVisible(true);
    recentFiles[index]->setStatusTip(dirs[i]);
    recentFiles[index]->setToolTip(dirs[i]);

    if (!QFile(dirs[i]).exists()) recentFiles[index]->setEnabled(false);
    ++index;
  }

  for (int i = numRecentFiles; i < MaxRecentFiles; ++i)
  {
    recentFiles[i]->setVisible(false);
  }
}

void MainWindow::showImageAt(int index)
{
  if (files.isEmpty()) return;
  const auto& file = files.at(index);
  displayImage(file);
}

void MainWindow::displayImage(const QString& file)
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  const auto image = reader.read();

  if (image.isNull())
  {
    ui->statusbar->showMessage(
      QString( "Unable to display %1 (%2/%3)" ).arg( file ).
        arg( files.currentIndex() + 1 ).
        arg( files.count() ) );
    return;
  }

  const auto format = QImageReader::imageFormat(file);
  ui->actionView_Exif->setEnabled((format == "jpeg"));

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

void MainWindow::playback()
{
  QIcon icon( ":/images/Pause.png" );
  ui->actionPlay->setIcon(icon);
  ui->actionPlay->setText("Pause");
  ui->actionFirst->setEnabled(false);
  ui->actionPrevious->setEnabled(false);
  ui->actionNext->setEnabled(false);
  timer.start();
}

void MainWindow::pause()
{
  QIcon icon( ":/images/Play.png" );
  ui->actionPlay->setIcon(icon);
  ui->actionPlay->setText("Play");
  timer.stop();
  ui->actionFirst->setEnabled(true);
  ui->actionPrevious->setEnabled(true);
  ui->actionNext->setEnabled(true);
}
