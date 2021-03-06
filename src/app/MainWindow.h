#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtWidgets/QMainWindow>

#if defined(Q_OS_MAC)
#include <QtCore/QProcess>
#endif

#include "model/FileList.h"
#include "TextWidget.h"

namespace com::sptci
{
  namespace Ui { class MainWindow; }

  class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    model::FileList::Iterator cbegin() const { return files.cbegin(); }
    model::FileList::Iterator cend() const { return files.cend(); }

  public slots:
    void openDirectory();
    void newWindow();
    void play();
    void first();
    void previous();
    void next();
    void stopScanning();
    void displaySleep();
    void showFile();
    void removeFile();
    void setIndex(int index);
    void setInterval(int interval);
    void viewExif();
    void watermark();
    void createPdf();
    void createComic();
    void about();
    void aboutQt();
    void quit();

  protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool event(QEvent* event) override;

  signals:
    void scanStop();

  private slots:
    void positionTextWidget();
    void openRecent();
    void addFile(const QString file);
    void scanFinished(const QString directory, int count);
    void showImage();

  private:
    QString trimTrailingDirectorySeparator(const QString& input);
    int interval();
    void initInterval();
    bool sleepFlag();
    void initDisplaySleep();
    void processDirectory(const QString& dir);
    void addRecent(const QString& dir);
    void createRecent();
    void updateRecent();
    void showImageAt(int index);
    void displayImage(const QString& file);
    void playback();
    void pause();
    void enableMenu();

  private:
    static int8_t WINDOW_INDEX;
    static const QString RECENT_FILES;
    static const QString INTERVAL;
    static const QString DISPLAY_SLEEP;

    QTimer timer;
    model::FileList files;
    QList<QThread*> threads;

    enum { MaxRecentFiles = 10 };
    QAction* recentFiles[MaxRecentFiles];

#if defined(Q_OS_MAC)
    QProcess caffeinate;
#endif
    Ui::MainWindow* ui;
    TextWidget* intervalTextWidget;
    double scaleFactor = 1;
    int8_t index;
    bool playing = false;
    bool initialised = false;
  };
}

#endif // MAINWINDOW_H
