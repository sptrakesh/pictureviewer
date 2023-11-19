#ifndef WRITER_H
#define WRITER_H

#include <QtCore/QObject>

namespace com::sptci::archive
{

  class Writer : public QObject
  {
    Q_OBJECT
  public:
    explicit Writer(const QString& output, QObject* parent = nullptr);
    ~Writer() override;

  signals:

  public slots:

  private:
    const QString outputFile;
  };

} // namespace com::sptci::archive

#endif // WRITER_H
