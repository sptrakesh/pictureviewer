#include "ComicMaker.h"
#include "ui_ComicMaker.h"
#include "MainWindow.h"
#include "functions.h"

#include <QtWidgets/QFileDialog>

using com::sptci::ComicMaker;

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
  const auto directory = com::sptci::documentsDirectory();

  const auto dir = QFileDialog::getExistingDirectory(this,
      tr("Select Output Folder"),
      directory,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isEmpty()) return;

  auto mw = dynamic_cast<MainWindow*>(parent());
}

void ComicMaker::inDirectory()
{
  const auto directory = com::sptci::documentsDirectory();

  const auto dir = QFileDialog::getExistingDirectory(this,
      tr("Select Output Folder"),
      directory,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isEmpty()) return;

  auto mw = dynamic_cast<MainWindow*>(parent());
}
