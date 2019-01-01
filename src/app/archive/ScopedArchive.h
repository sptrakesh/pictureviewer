#ifndef SCOPEDARCHIVE_H
#define SCOPEDARCHIVE_H

#include <QtCore/QObject>

struct archive;

namespace com::sptci::archive
{
  class ScopedArchive : public QObject
  {
    Q_OBJECT

  public:
    ScopedArchive(const QString& file);
    ~ScopedArchive();

    struct archive* getArchive() const { return source; }

  signals:
    void warning(int errorCode, QString error);
    void error(int errorCode, QString error);

  private:
    void check(const int errorCode);

    struct archive* source;
  };
}

#endif // SCOPEDARCHIVE_H
