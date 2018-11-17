#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtWidgets/QMainWindow>

#include "FileList.h"

namespace com::sptci
{
  namespace Ui { class MainWindow; }

  class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

  public slots:
    void openDirectory();
    void stopScanning();
    void play();
    void setIndex(int index);
    void setInterval(int interval);
    void removeFile();
    void aboutQt();

  protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

  signals:
    void scan();
    void scanStop();

  private slots:
    void addFile(const QString file);
    void scanFinished(const QString directory, int count);
    void showImage();

  private:
    QTimer timer;
    FileList files;
    QList<QThread*> threads;
    Ui::MainWindow* ui;
    double scaleFactor = 1;
    bool playing = false;
  };
}

#endif // MAINWINDOW_H
