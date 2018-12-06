#include "ExifWindow.h"
#include "ui_ExifWindow.h"
#include "ExifModel.h"

using com::sptci::ExifWindow;

ExifWindow::ExifWindow(const QString& file, QWidget *parent) :
    QWidget(parent), file(file), ui(new Ui::ExifWindow)
{
  setWindowFlags(Qt::Window);
  setAttribute(Qt::WA_DeleteOnClose);
  ui->setupUi(this);
  auto model = new ExifModel(file, this);
  ui->tableView->setModel(model);
  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

ExifWindow::~ExifWindow()
{
  delete ui;
}
