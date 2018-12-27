#include "ComicMaker.h"
#include "ui_ComicMaker.h"
#include "model/ComicBookSpec.h"
#include "MainWindow.h"
#include "functions.h"

#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>
#include <QtWidgets/QFileDialog>

using com::sptci::ComicMaker;

Q_LOGGING_CATEGORY(COMIC_MAKER, "com::sptci::ComicMaker")

ComicMaker::ComicMaker(const QString& file, QWidget* parent) :
  QWidget(parent), file(file), ui(new Ui::ComicMaker)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window);
  setWindowTitle(QString("%1 - %2").arg(windowTitle()).arg(file));
}

ComicMaker::~ComicMaker()
{
  delete ui;
}

void ComicMaker::allFiles()
{
  const auto fileName = targetFile();
  if (fileName.isEmpty())
  {
    qDebug(COMIC_MAKER) << "User cancelled file dialogue.";
    return;
  }

  const model::ComicBookSpec spec{fileName, ui->archiveType->currentIndex(),
    ui->sortFiles->isChecked(), ui->showFile->isChecked()};
  auto mw = dynamic_cast<MainWindow*>(parent());
  auto engine = new worker::ComicEngine(mw->cbegin(), mw->cend(), spec);
  configureEngine(engine);
}

void ComicMaker::inDirectory()
{
  const auto fileName = targetFile();
  if (fileName.isEmpty())
  {
    qDebug(COMIC_MAKER) << "User cancelled file dialogue.";
    return;
  }

  const model::ComicBookSpec spec{fileName, ui->archiveType->currentIndex(),
    ui->sortFiles->isChecked(), ui->showFile->isChecked()};
  auto engine = new worker::ComicEngine(QFileInfo(file).dir(), spec);
  configureEngine(engine);
}

QString ComicMaker::targetFile()
{
  QString extension;
  switch (ui->archiveType->currentIndex())
  {
    case 0: // rar
      extension = "rar";
      break;
    case 1: // zip
      extension = "zip";
      break;
    case 2: // tar
      extension = "tar";
      break;
    case 3: // 7z
    default:
      extension = "7z";
      break;
  }

  const auto directory = com::sptci::documentsDirectory();
  const QFileInfo fi(file);
  const auto fn = QString("%1/%2.%3").arg(directory).arg(fi.baseName()).arg(extension);

  return QFileDialog::getSaveFileName(this,
      tr("Save File"),
      fn,
      tr("Documents (*.rar *.zip *.tar *.7z)"));
}

void ComicMaker::configureEngine(worker::ComicEngine* process)
{
  auto fileName = process->create();
  if (ui->showFile->isChecked()) showFile(fileName);
  process->deleteLater();
  close();
}
