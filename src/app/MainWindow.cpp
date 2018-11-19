#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirectoryScanner.h"
#include "InfoViewer.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMimeData>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QImageReader>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <algorithm>
#include <iterator>

#if defined(Q_OS_WIN)
#include <winbase.h>
#endif

using com::sptci::MainWindow;

Q_LOGGING_CATEGORY( MAIN_WINDOW, "com.sptci.MainWindow" )

const QString MainWindow::RECENT_FILES = "recentFiles";
const QString MainWindow::INTERVAL = "interval";
const QString MainWindow::DISPLAY_SLEEP = "displaySleep";

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->label->setBackgroundRole(QPalette::Base);

  createRecent();

  const auto time = interval();
  timer.setInterval(time * 1000);
  ui->intervalSlider->setValue(time);
  connect(&timer, &QTimer::timeout, this, &MainWindow::showImage);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
  const bool flag = sleepFlag();
  qDebug(MAIN_WINDOW) << "Stored display sleep flag: " << flag;
  ui->actionDisplay_sleep->setChecked(flag);
#else
  ui->actionDisplay_sleep->setEnabled(false);
#endif

  ui->statusbar->setStyleSheet("QStatusBar{color:black}");
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
  if (QProcess::Running == caffeinate.state())
  {
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
  using com::sptci::DirectoryScanner;

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
      displayImage(file.absoluteFilePath());
    }
    else
    {
      qWarning(MAIN_WINDOW) << "Unsupported file type:" << file.absoluteFilePath();
    }
  }
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

void MainWindow::play()
{
  ui->actionPlay->setEnabled(true);

  if (playing)
  {
    QIcon icon( ":/images/Play.png" );
    ui->actionPlay->setIcon(icon);
    ui->actionPlay->setText("Play");
    timer.stop();
    ui->actionFirst->setEnabled(true);
    ui->actionPrevious->setEnabled(true);
    ui->actionNext->setEnabled(true);
  }
  else
  {
    QIcon icon( ":/images/Pause.png" );
    ui->actionPlay->setIcon(icon);
    ui->actionPlay->setText("Pause");
    ui->actionFirst->setEnabled(false);
    ui->actionPrevious->setEnabled(false);
    ui->actionNext->setEnabled(false);
    timer.start();
  }

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

  if (flag)
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
  else
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
  const auto status = playing;

  if (status) play();

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

  if (status) play();
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
  }
  timer.setInterval(interval * 1000);
}

void MainWindow::about()
{
  com::sptci::InfoViewer::showPage("player.html");
}

void MainWindow::aboutQt()
{
  qApp->aboutQt();
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

bool MainWindow::sleepFlag()
{
  QSettings settings;
  return settings.value(DISPLAY_SLEEP, true).toBool();
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
  threads.append(thread);
  thread->start();

  emit scan();
  play();
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
