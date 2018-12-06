#ifndef WATERMARKFILES_H
#define WATERMARKFILES_H

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QStringList>

#include <mutex>

#include "WatermarkSpec.h"

namespace com::sptci
{

  class WatermarkFiles : public QObject
  {
    Q_OBJECT
  public:
    explicit WatermarkFiles(QStringList::ConstIterator begin,
      QStringList::ConstIterator end, const QDir& outDir,
      WatermarkSpecPtr spec, QObject* parent = nullptr);
    explicit WatermarkFiles(const QDir& inDir, const QDir& outDir,
      WatermarkSpecPtr spec, QObject* parent = nullptr);

    int size() const { return files.size(); }

  signals:
    void progress(int index, int total, QString file);
    void finished();

  public slots:
    void run();
    void stop();

  private:
    static QStringList filesFromIterators(
      QStringList::ConstIterator begin, QStringList::ConstIterator end);
    static QStringList filesFromDir(const QDir& dir);
    bool getAbort();
    void setAbort(bool flag);

  private:
    const QStringList files;
    QDir outDir;
    std::mutex mutex;
    WatermarkSpecPtr spec;
    bool abort = false;
  };

} // namespace com::sptci

#endif // WATERMARKFILES_H
