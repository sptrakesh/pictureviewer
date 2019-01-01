#ifndef READER_H
#define READER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

namespace com::sptci::archive
{
  struct OutputDirectory;

  class Reader : public QObject
  {
    Q_OBJECT
  public:
    explicit Reader(const QString& file, QObject* parent = nullptr);

    QStringList extractAll(const OutputDirectory& outputDirectory);
    //bool extract(const QString& entry);

  signals:
    void extracted(QString fileName);
    void error(int errorCode, QString fileName);

  private slots:
    void archiveWarning(int errorCode, QString message);
    void archiveError(int errorCode, QString message);
    void entryWarning(int errorCode, QString fileName);
    void entryError(int errorCode, QString fileName);
    void eof() { hasMore = false; }

  private:
    const QString file;
    bool hasMore = true;
  };

} // namespace com::sptci::archive

#endif // READER_H
