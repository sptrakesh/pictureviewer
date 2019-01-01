#ifndef SCOPEDARCHIVEENTRY_H
#define SCOPEDARCHIVEENTRY_H

#include <QtCore/QObject>

struct archive;

namespace com::sptci::archive
{

  class ScopedArchive;

  class ScopedArchiveEntry : public QObject
  {
    Q_OBJECT
  public:
    explicit ScopedArchiveEntry(const QString& directory, const ScopedArchive& s);
    ~ScopedArchiveEntry();

    ScopedArchiveEntry(const ScopedArchiveEntry&) = delete;
    ScopedArchiveEntry& operator=(const ScopedArchiveEntry&) = delete;

    QString next();

  signals:
    void eof();
    void warning(int errorCode, QString error);
    void error(int errorCode, QString error);

  private:
    int copy();
    bool check(const int errorCode);

    const QString directory;
    struct archive* source;
    struct archive* destination;
  };

} // namespace com::sptci::archive

#endif // SCOPEDARCHIVEENTRY_H
