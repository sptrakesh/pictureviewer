#ifndef PDFSPEC_H
#define PDFSPEC_H

#include <QtCore/QString>
#include <memory>

namespace com::sptci
{
  struct PdfSpec
  {
    PdfSpec(int paperSize, const QString& destination, const QString& title,
      const QString& creator, int compression = 100) :
        destination(destination), title(title),
        creator(creator), paperSize(paperSize), compression(compression)  {}
    const QString destination;
    const QString title;
    const QString creator;
    const int paperSize;
    const int compression;
  };

  using PdfSpecPtr = std::unique_ptr<PdfSpec>;
}

#endif // PDFSPEC_H
