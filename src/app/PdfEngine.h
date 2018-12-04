#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <QtGui/QPainter>
#include <tuple>
#include "PdfSpec.h"

namespace com::sptci {

  class PdfEngine : public QObject
  {
    Q_OBJECT
  public:
    explicit PdfEngine(PdfSpecPtr spec, QObject* parent = nullptr);

  signals:
    void finished(const QString file);

  public slots:
    void create(const QString& file);

  private:
    struct DimParam
    {
      int sizeId;
      int dpiX;
      int dpiY;
    };

    using Dimension = std::tuple<int,int>;

    void save(const QString& dest);
    void addFile(QPainter* painter, const Dimension& dimension, const QString& fileName);
    Dimension dimensions(const DimParam& param);

  private:
    PdfSpecPtr spec;
  };

} // namespace com::sptci

#endif // PDFENGINE_H
