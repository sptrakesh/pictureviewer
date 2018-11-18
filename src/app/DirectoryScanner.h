#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H

#include <QtCore/QObject>
#include <mutex>

namespace com::sptci
{
  class DirectoryScanner : public QObject
  {
    Q_OBJECT
  public:
    explicit DirectoryScanner(const QString directory, QObject* parent = nullptr);
    ~DirectoryScanner() override;

  signals:
    void file(const QString file);
    void finished(const QString directory, int count);

  public slots:
    void scan();
    void stop();

  private:
    void setAbort(bool flag);

    const QString directory;
    bool abort = false;
    std::mutex mutex;
  };
}

  #endif // DIRECTORYSCANNER_H
