#ifndef PDFSPEC_H
#define PDFSPEC_H

#include <QtCore/QString>
#include <memory>

namespace com::sptci
{
  struct PdfSpec
  {
    PdfSpec(int ps, const QString& d) : paperSize(ps), destination(d) {}
    const int paperSize;
    const QString destination;
  };

  using PdfSpecPtr = std::unique_ptr<PdfSpec>;
}

#endif // PDFSPEC_H
