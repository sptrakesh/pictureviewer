#ifndef COM_SPTCI_PDFMAKER_H
#define COM_SPTCI_PDFMAKER_H

#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtWidgets/QProgressDialog>

namespace com::sptci {

  namespace Ui { class PdfMaker; }

  class PdfMaker : public QWidget
  {
    Q_OBJECT

  public:
    explicit PdfMaker(const QString& file, QWidget* parent = nullptr);
    ~PdfMaker();

  signals:
    void start(const QList<QString>& files);

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
    QList<QString> files();

  private:
    const QString file;
    QString dest;
    Ui::PdfMaker* ui;
    QThread* thread = nullptr;
    QProgressDialog* progress = nullptr;
  };

} // namespace com::sptci
#endif // COM_SPTCI_PDFMAKER_H
