#include "LogViewer.h"
#include "ui_LogViewer.h"
#include "logger.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

using com::sptci::LogViewer;

LogViewer::LogViewer(QWidget* parent) : QWidget(parent), ui(new Ui::LogViewer)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window);
  setWindowTitle("Picture Viewer Logs");
  refresh();
}

LogViewer::~LogViewer()
{
  delete ui;
}

void LogViewer::refresh()
{
  QFile file(com::sptci::logFile());
  file.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&file);
  QString line;

  while (!in.atEnd())
  {
    line = in.readLine();
    const QString html = addHtmlColor(line);
    ui->plainTextEdit->appendHtml(html);
  }

  file.close();
}

QString LogViewer::addHtmlColor(const QString& text) const
{
#if defined(Q_OS_MAC)
  QString color = "White";
#else
  QString color = "Black";
#endif

  if (text.contains("Critical:")) color = "Red";
  else if (text.contains("Warning:")) color = "Orange";
  else if (text.contains("Info:")) color = "Blue";
  else if (text.contains("Debug:")) color = "Green";

  return QString("<font color=\"%1\">%2</font>").arg(color).arg(text);
}
