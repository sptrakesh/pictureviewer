#include "ExifWindow.h"
#include "ui_ExifWindow.h"
#include "ExifModel.h"

using com::sptci::ExifWindow;

ExifWindow::ExifWindow(const QString& file, QWidget *parent) :
    QWidget(parent), file(file), ui(new Ui::ExifWindow)
{
  setWindowFlags(Qt::Window);
  ui->setupUi(this);
  auto model = new ExifModel(file, this);
  ui->tableView->setModel(model);
}

ExifWindow::~ExifWindow()
{
  delete ui;
}
