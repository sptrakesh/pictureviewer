#ifndef WATERMARKDIRECTORY_H
#define WATERMARKDIRECTORY_H

#include <QtCore/QDir>
#include <QtCore/QObject>

#include <mutex>

#include "WatermarkSpec.h"

namespace com::sptci
{

  class WatermarkDirectory : public QObject
  {
    Q_OBJECT
  public:
    explicit WatermarkDirectory(const QDir& inDir, const QDir& outDir,
      WatermarkSpecPtr spec, QObject* parent = nullptr);

  signals:
    void progress(uint count, QString file);
    void finished();

  public slots:
    void run();
    void stop();

  private:
    void setAbort(bool flag);

    QDir inDir;
    QDir outDir;
    std::mutex mutex;
    WatermarkSpecPtr spec;
    bool abort = false;
  };

} // namespace com::sptci

#endif // WATERMARKDIRECTORY_H
