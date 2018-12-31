#include "ComicDisplayWidget.h"
#include "ui_ComicDisplayWidget.h"
#include "functions.h"

#include <QArchive.hpp>

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

  struct OutputDirectory
  {
    OutputDirectory()
    {
      auto uuid = QUuid::createUuid().toString();
      uuid.replace("{", "");
      uuid.replace("}", "");
      auto name = QString("%1/%2").arg(tempDirectory()).arg(uuid);
      dir = QDir(name);
      if (!dir.exists()) dir.mkpath(".");
    }

    OutputDirectory(const OutputDirectory&) = delete;
    OutputDirectory& operator=(const OutputDirectory&) = delete;

    ~OutputDirectory()
    {
      qDebug(COMIC_DISPLAY) << "Removing unarchived files under " << dir.absolutePath();
      dir.removeRecursively();
    }

    QDir dir;
  };
}

using com::sptci::ComicDisplayWidget;

ComicDisplayWidget::ComicDisplayWidget(QWidget* parent) :
  QWidget(parent), ui(new Ui::ComicDisplayWidget)
{
  ui->setupUi(this);
}

ComicDisplayWidget::~ComicDisplayWidget()
{
  delete ui;
}

void ComicDisplayWidget::display(const QString& file)
{
  using QArchive::Reader;

  files.clear();
  outputFile = std::make_unique<comic::OutputFile>(file);
  QFile::copy(file, outputFile->output);

  const auto json = Reader(outputFile->output).start().waitForFinished().getFilesList();
  files << json.keys();

  ui->spinBox->setMaximum(files.size() - 1);
  ui->horizontalSlider->setMaximum(files.size() - 1);

  extract();
  showFile();
}

void ComicDisplayWidget::atIndex(int index)
{
  if (index == currentIndex) return;
  showFile(index);
}

void ComicDisplayWidget::extract()
{
  using QArchive::Extractor;

  if (!outputDirectory) outputDirectory = std::make_unique<comic::OutputDirectory>();
  Extractor extractor(outputFile->output, outputDirectory->dir.absolutePath());

  extractor.setFunc(QArchive::EXTRACTED, [&](QString file) {
    qDebug(COMIC_DISPLAY) << "Extracted file: " << file;
  });
  extractor.setFunc([&](short code, QString file) {
    qWarning(COMIC_DISPLAY) << "Extraction error - code: " << code << ", file: " << file;
  });

  extractor.start().waitForFinished();
}

void ComicDisplayWidget::showFile(int index)
{
  if (index >= files.size()) return;

  currentIndex = index;
  ui->spinBox->setValue(index);
  ui->horizontalSlider->setValue(index);

  auto fileName = files.at(index);
  auto file = QString("%1/%2").arg(outputDirectory->dir.absolutePath()).arg(fileName);
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
