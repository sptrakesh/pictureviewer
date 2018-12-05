#ifndef COM_SPTCI_PDFMAKER_H
#define COM_SPTCI_PDFMAKER_H

#include <QtCore/QStringList>
#include <QtWidgets/QProgressDialog>

#include <memory>

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

  private slots:
    void updateProgress(int index, int total, QString file);
    void progressCancelled();
    void finished();

  private:
    void save(const QString& destination);
    void saveFile(const QString& destination);
    void saveAll(const QString& destination);
    QStringList files();

  private:
    const QString file;
    QString dest;
    Ui::PdfMaker* ui;
    std::unique_ptr<QProgressDialog> progress = nullptr;
    bool completed = true;
  };

} // namespace com::sptci
#endif // COM_SPTCI_PDFMAKER_H
