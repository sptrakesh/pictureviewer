#include "ComicDisplayWidget.h"
#include "ui_ComicDisplayWidget.h"
#include "functions.h"
#include "archive/Reader.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUuid>
#include <QtGui/QImageReader>

Q_LOGGING_CATEGORY(COMIC_DISPLAY, "com::sptci::ComicDisplayWidget")

namespace com::sptci::comic
{
  struct OutputFile
  {
    explicit OutputFile(const QString& input) : input(input), output(outputFile(input))
    {
      qDebug(COMIC_DISPLAY) << "Copied archive " << input << " to " << output;
    }

    OutputFile(const OutputFile&) = delete;
    OutputFile& operator=(const OutputFile&) = delete;

    ~OutputFile()
    {
      if (!output.isEmpty())
      {
        qDebug(COMIC_DISPLAY) << "Removing copied archive " << output;
        QFile::remove(output);
      }
    }

    static QString outputFile(const QString& file)
    {
      const auto fi = QFileInfo(file);
      const auto suffix = fi.suffix();
      if ("cbr" == suffix) return QString("%1/%2.rar").arg(tempDirectory()).arg(fi.baseName());
      if ("cbz" == suffix) return QString("%1/%2.zip").arg(tempDirectory()).arg(fi.baseName());
      if ("cbt" == suffix) return QString("%1/%2.tar").arg(tempDirectory()).arg(fi.baseName());
      if ("cb7" == suffix) return QString("%1/%2.7z").arg(tempDirectory()).arg(fi.baseName());
      return "";
    }

    const QString input;
    const QString output;
  };
}

using com::sptci::ComicDisplayWidget;

ComicDisplayWidget::ComicDisplayWidget(QWidget* parent) :
  QWidget(parent), ui(new Ui::ComicDisplayWidget), outputDirectory()
{
  ui->setupUi(this);
}

ComicDisplayWidget::~ComicDisplayWidget()
{
  delete ui;
}

void ComicDisplayWidget::display(const QString& file)
{
  files.clear();
  outputFile = std::make_unique<comic::OutputFile>(file);
  QFile::copy(file, outputFile->output);
  extract();

  ui->spinBox->setMaximum(files.size() - 1);
  ui->horizontalSlider->setMaximum(files.size() - 1);

  showFile();
}

void ComicDisplayWidget::atIndex(int index)
{
  if (index == currentIndex) return;
  showFile(index);
}

void ComicDisplayWidget::extract()
{
  archive::OutputDirectory outputDirectory;
  archive::Reader reader(outputFile->output);
  const auto list = reader.extractAll(outputDirectory);
  files.append(list);
}

void ComicDisplayWidget::showFile(int index)
{
  if (index >= files.size()) return;

  currentIndex = index;
  ui->spinBox->setValue(index);
  ui->horizontalSlider->setValue(index);

  auto fileName = files.at(index);
  auto file = QString("%1/%2").arg(outputDirectory.dir.absolutePath()).arg(fileName);
  qDebug(COMIC_DISPLAY) << "Displaying image " << file;
  QImageReader reader(file);
  reader.setAutoTransform(true);
  auto pixmap = QPixmap::fromImageReader(&reader);
  if (pixmap.isNull())
  {
    qWarning(COMIC_DISPLAY) << "Invalid image file " << file;
    return;
  }

  pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->imageLabel->setPixmap(pixmap);
}
