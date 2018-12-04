#ifndef COM_SPTCI_PDFMAKER_H
#define COM_SPTCI_PDFMAKER_H

#include <QtWidgets/QWidget>
#include <tuple>

namespace com::sptci {

  namespace Ui { class PdfMaker; }

  class PdfMaker : public QWidget
  {
    Q_OBJECT

  public:
    explicit PdfMaker(const QString& file, QWidget* parent = nullptr);
    ~PdfMaker();

  public slots:
    void saveAs();

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
    const QString file;
    Ui::PdfMaker* ui;
  };

} // namespace com::sptci
#endif // COM_SPTCI_PDFMAKER_H
