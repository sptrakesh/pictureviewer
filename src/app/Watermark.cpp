#include "Watermark.h"
#include "ui_Watermark.h"
#include "MainWindow.h"
#include "worker/WatermarkEngine.h"
#include "worker/WatermarkFiles.h"
#include "functions.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtCore/QThread>
#include <QtGui/QImageReader>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

using com::sptci::Watermark;
using com::sptci::model::WatermarkSpecPtr;

Q_LOGGING_CATEGORY(WATERMARK, "com::sptci::Watermark")

Watermark::Watermark(const QString& file, QWidget *parent) :
    QWidget(parent), file(file), fontColour(Qt::black),
    backgroundColour(Qt::transparent), ui(new Ui::Watermark)
{
  ui->setupUi(this);
  setWindowFlags(Qt::Window);
  setAttribute(Qt::WA_DeleteOnClose);

  fontColour.setAlpha(ui->textOpacity->value());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));

  setForeground();
  displayImage();
}

Watermark::~Watermark()
{
  delete ui;
}

void Watermark::selectFontColour()
{
  const auto colour = QColorDialog::getColor(Qt::black, this, tr("Font Colour"));
  if (!colour.isValid()) return;
  fontColour.setNamedColor(colour.name());
  fontColour.setAlpha(ui->textOpacity->value());

  auto pal = ui->fontColour->palette();
  pal.setColor(QPalette::Button, fontColour);
  ui->fontColour->setPalette(pal);
  ui->fontColour->update();
  raise();
}

void Watermark::selectBackgroundColour()
{
  const auto colour = QColorDialog::getColor(Qt::black, this, "Font Colour");
  if (!colour.isValid()) return;
  backgroundColour.setNamedColor(colour.name());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());

  auto pal = ui->backgroundColour->palette();
  pal.setColor(QPalette::Button, backgroundColour);
  ui->backgroundColour->setPalette(pal);
  ui->backgroundColour->update();
  raise();
}

void Watermark::clear()
{
  displayImage();
}

void Watermark::preview()
{
  fontColour.setAlpha(ui->textOpacity->value());
  backgroundColour.setAlpha(ui->backgroundOpacity->value());
  overlay();
}

void Watermark::saveAs()
{
  QFileInfo fi(file);

  const auto directory = com::sptci::picturesDirectory();
  const auto fn = QString("%1/%2").arg(directory).arg(fi.fileName());

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      fn,
      tr("Images (*.png *.xpm *.jpg *.gif *.png)"));
  if (fileName.isEmpty())
  {
    qDebug(WATERMARK) << "User cancelled file dialogue.";
    return;
  }

  if (QFileInfo(fileName).exists())
  {
    const auto result = QMessageBox::question(this,
        tr("Over-write file?"),
        QString("File %1 exists").arg(fileName));

    if (QMessageBox::Yes == result)
    {
      saveFile(fileName);
      return;
    }
  }

  saveFile(fileName);
}

void Watermark::allInDirectory()
{
  using worker::WatermarkFiles;

  completed = true;
  QFileInfo fi(file);
  const auto directory = com::sptci::picturesDirectory();

  const auto dir = QFileDialog::getExistingDirectory(this,
      tr("Select Output Folder"),
      directory,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isEmpty()) return;

  auto process = new WatermarkFiles(fi.absoluteDir(), QDir(dir), createSpec());
  auto thread = new QThread;
  process->moveToThread(thread);
  connect(thread, &QThread::started, process, &WatermarkFiles::run);
  connect(process, &WatermarkFiles::progress, this, &Watermark::updateProgress);
  connect(process, &WatermarkFiles::finished, thread, &QThread::quit);
  connect(process, &WatermarkFiles::finished, this, &Watermark::finished);
  connect(process, &WatermarkFiles::finished, process, &WatermarkFiles::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  progress = std::make_unique<QProgressDialog>(tr("Watermarking files..."),
    "Abort Process", 0, process->size());
  connect(progress.get(), &QProgressDialog::canceled, process, &WatermarkFiles::stop);
  connect(progress.get(), &QProgressDialog::canceled, this, &Watermark::progressCancelled);

  thread->start();
}

void Watermark::allFiles()
{
  using worker::WatermarkFiles;

  completed = true;
  const auto directory = com::sptci::picturesDirectory();

  const auto dir = QFileDialog::getExistingDirectory(this,
      tr("Select Output Folder"),
      directory,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isEmpty()) return;

  auto mw = dynamic_cast<MainWindow*>(parent());

  auto process = new WatermarkFiles(mw->cbegin(), mw->cend(), QDir(dir), createSpec());
  auto thread = new QThread;
  process->moveToThread(thread);
  connect(thread, &QThread::started, process, &WatermarkFiles::run);
  connect(process, &WatermarkFiles::progress, this, &Watermark::updateProgress);
  connect(process, &WatermarkFiles::finished, thread, &QThread::quit);
  connect(process, &WatermarkFiles::finished, this, &Watermark::finished);
  connect(thread, &QThread::finished, process, &WatermarkFiles::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  progress = std::make_unique<QProgressDialog>(tr("Watermarking files..."),
    "Abort Process", 0, process->size());
  connect(progress.get(), &QProgressDialog::canceled, process, &WatermarkFiles::stop);
  connect(progress.get(), &QProgressDialog::canceled, this, &Watermark::progressCancelled);

  thread->start();
}

void Watermark::updateProgress(int index, int total, QString file)
{
  if (!progress)
  {
    qWarning(WATERMARK) << "No progress dialogue to update";
    return;
  }

  progress->setValue(index);
  progress->setLabelText(QString("Watermarked file... (%1/%2)").arg(index).arg(total));
  qInfo(WATERMARK) << "Saved watermark file " << file <<
    "(" << index << "/" << total << ")";
}

void Watermark::progressCancelled()
{
  completed = false;
  progress->hide();
  progress = nullptr;
  raise();
}

void Watermark::finished()
{
  if (completed) close();
}

WatermarkSpecPtr Watermark::createSpec()
{
  return std::make_unique<model::WatermarkSpec>(
    model::WatermarkSpec{
      getText(), fontColour, ui->fontComboBox->currentFont(),
      backgroundColour, ui->fontSize->value(),
      ui->textOpacity->value(), ui->backgroundOpacity->value(),
      ui->position->currentIndex(), ui->alignment->currentIndex()
  });
}

void Watermark::setForeground()
{
#if defined(Q_OS_MAC)
  QPalette palette;
  palette.setColor(QPalette::Text, Qt::black);
  ui->text->setPalette(palette);
  ui->fontComboBox->setPalette(palette);
  ui->fontSize->setPalette(palette);
  ui->textOpacity->setPalette(palette);
  ui->position->setPalette(palette);
  ui->backgroundOpacity->setPalette(palette);
#endif

  auto pal = ui->fontColour->palette();
  pal.setColor(QPalette::Button, fontColour);
  pal.setColor(QPalette::Text, Qt::white);
  ui->fontColour->setPalette(pal);

  pal = ui->backgroundColour->palette();
  pal.setColor(QPalette::Button, backgroundColour);
  pal.setColor(QPalette::Text, Qt::white);
  ui->backgroundColour->setPalette(pal);
}

void Watermark::displayImage()
{
  QImageReader reader(file);
  reader.setAutoTransform(true);
  const auto image = reader.read();

  if (image.isNull()) return;

  displayImage(image);
}

void Watermark::displayImage(const QImage& image)
{
  auto pixmap = QPixmap::fromImage(image);
  pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->imageLabel->setPixmap(pixmap);
}

QString Watermark::getText()
{
  return (ui->text->text().isEmpty()) ?
      ui->text->placeholderText() : ui->text->text();
}

void Watermark::overlay()
{
  auto spec = createSpec();
  worker::WatermarkEngine engine(spec.get(), this);
  const auto image = engine.overlay(file);
  displayImage(image);
}

void Watermark::saveFile(const QString& fileName)
{
  const auto pixmap = ui->imageLabel->pixmap();
  if (!pixmap)
  {
    qWarning(WATERMARK) << "Image label returned null pixmap";
    return;
  }

  const auto image = pixmap->toImage();
  if (image.save(fileName, nullptr, 100))
  {
    qInfo(WATERMARK) << "Wrote watermarked image to " << fileName;
  }
  else
  {
    QMessageBox::warning(this,
      tr("Unable to write file"),
      QString("Unable to write to file: %1").arg(fileName),
      QMessageBox::Ok);
  }
}
