#ifndef COMICSCANNER_H
#define COMICSCANNER_H

#include <QtCore/QObject>
#include <mutex>

namespace com::sptci::worker
{

  class ComicScanner : public QObject
  {
    Q_OBJECT
  public:
    explicit ComicScanner(const QString& directory, QObject* parent = nullptr);
    ~ComicScanner() override;

  signals:
    void comic(QString fileName);
    void finished(const QString directory, int count);

  public slots:
    void run();

  private:
    bool getAbort();
    void setAbort(bool flag);

    const QString directory;
    std::mutex mutex;
    bool abort = false;
  };

} // namespace com::sptci::worker

#endif // COMICSCANNER_H
