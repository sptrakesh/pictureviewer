#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <QtCore/QStringList>
#include <QtGui/QPainter>

#include <mutex>
#include <tuple>

#include "PdfSpec.h"

namespace com::sptci {

  class PdfEngine : public QObject
  {
    Q_OBJECT
  public:
    explicit PdfEngine(PdfSpecPtr spec, const QStringList& files, QObject* parent = nullptr);

  signals:
    void progress(int count, int total, QString file);
    void finished();

  public slots:
    void run();
    void stop();

  private:
    using Dimension = std::tuple<int,int>;

    void save(const QString& dest);
    void addFile(QPainter* painter, const Dimension& dimension, const QString& fileName);
    Dimension dimensions(int dpiX, int dpiY);
    bool getAbort();
    void setAbort(bool abort);

  private:
    std::mutex mutex;
    QStringList files;
    PdfSpecPtr spec;
    bool abort = false;
  };

} // namespace com::sptci

#endif // PDFENGINE_H
