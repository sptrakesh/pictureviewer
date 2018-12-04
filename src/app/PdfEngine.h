#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <QtCore/QList>
#include <QtGui/QPainter>

#include <mutex>
#include <tuple>

#include "PdfSpec.h"

namespace com::sptci {

  class PdfEngine : public QObject
  {
    Q_OBJECT
  public:
    explicit PdfEngine(PdfSpecPtr spec, QObject* parent = nullptr);

  signals:
    void progress(int count, int total, QString file);
    void finished();

  public slots:
    void create(const QString& file);
    void run(const QList<QString>& files);
    void stop();

  private:
    using Dimension = std::tuple<int,int>;

    void save(const QString& dest);
    void addFile(QPainter* painter, const Dimension& dimension, const QString& fileName);
    Dimension dimensions(int dpiX, int dpiY);
    void setAbort(bool abort);

  private:
    std::mutex mutex;
    PdfSpecPtr spec;
    bool abort = false;
  };

} // namespace com::sptci

#endif // PDFENGINE_H
