#include "PaperSizeModel.h"

#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtPrintSupport/QPrinter>

using com::sptci::PaperSizeModel;

Q_LOGGING_CATEGORY(PAPER_SIZE_MODEL, "com::sptci::PaperSizeModel")

PaperSizeModel::PaperSizeModel(QObject* parent) : QAbstractListModel(parent)
{
  populate();
  setIndex();
}

QVariant PaperSizeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( Qt::DisplayRole != role ) return QVariant();
  if ( Qt::Vertical == orientation ) return QString::number( section + 1 );
  return tr( "PDF Paper Size" );
}

int PaperSizeModel::rowCount(const QModelIndex &parent) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if (parent.isValid()) return 0;

  return static_cast<int>(items.size());
}

QVariant PaperSizeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || Qt::DisplayRole != role) return QVariant();
  return std::get<1>(items.at(static_cast<std::size_t>(index.row())));
}

void PaperSizeModel::populate()
{
  items.reserve(128);

  items.emplace_back(QPageSize::A0, "A0");
  items.emplace_back(QPageSize::A1, "A1");
  items.emplace_back(QPageSize::A2, "A2");
  items.emplace_back(QPageSize::A3, "A3");
  items.emplace_back(QPageSize::A4, "A4");
  items.emplace_back(QPageSize::A5, "A5");
  items.emplace_back(QPageSize::A6, "A6");
  items.emplace_back(QPageSize::A7, "A7");
  items.emplace_back(QPageSize::A8, "A8");
  items.emplace_back(QPageSize::A9, "A9");
  items.emplace_back(QPageSize::A10, "A10");

  items.emplace_back(QPageSize::B0, "B0");
  items.emplace_back(QPageSize::B1, "B1");
  items.emplace_back(QPageSize::B2, "B2");
  items.emplace_back(QPageSize::B3, "B3");
  items.emplace_back(QPageSize::B4, "B4");
  items.emplace_back(QPageSize::B5, "B5");
  items.emplace_back(QPageSize::B6, "B6");
  items.emplace_back(QPageSize::B7, "B7");
  items.emplace_back(QPageSize::B8, "B8");
  items.emplace_back(QPageSize::B9, "B9");
  items.emplace_back(QPageSize::B10, "B10");

  items.emplace_back(QPageSize::C5E, "C5E");
  items.emplace_back(QPageSize::Comm10E, "Comm10E");
  items.emplace_back(QPageSize::DLE, "DLE");
  items.emplace_back(QPageSize::Executive, "Executive");
  items.emplace_back(QPageSize::Folio, "Folio");
  items.emplace_back(QPageSize::Ledger, "Ledger");
  items.emplace_back(QPageSize::Legal, "Legal");
  items.emplace_back(QPageSize::Letter, "Letter");
  items.emplace_back(QPageSize::Tabloid, "Tabloid");
  items.emplace_back(QPageSize::Custom, "Custom");

  items.emplace_back(QPageSize::A3Extra, "A3Extra");
  items.emplace_back(QPageSize::A4Extra, "A4Extra");
  items.emplace_back(QPageSize::A4Plus, "A4Plus");
  items.emplace_back(QPageSize::A4Small, "A4Small");
  items.emplace_back(QPageSize::A5Extra, "A5Extra");
  items.emplace_back(QPageSize::B5Extra, "B5Extra");

  items.emplace_back(QPageSize::JisB0, "JisB0");
  items.emplace_back(QPageSize::JisB1, "JisB1");
  items.emplace_back(QPageSize::JisB2, "JisB2");
  items.emplace_back(QPageSize::JisB3, "JisB3");
  items.emplace_back(QPageSize::JisB4, "JisB4");
  items.emplace_back(QPageSize::JisB5, "JisB5");
  items.emplace_back(QPageSize::JisB6, "JisB6");
  items.emplace_back(QPageSize::JisB7, "JisB7");
  items.emplace_back(QPageSize::JisB8, "JisB8");
  items.emplace_back(QPageSize::JisB9, "JisB9");
  items.emplace_back(QPageSize::JisB10, "JisB10");

  items.emplace_back(QPageSize::AnsiA, "AnsiA");
  items.emplace_back(QPageSize::AnsiB, "AnsiB");
  items.emplace_back(QPageSize::AnsiC, "AnsiC");
  items.emplace_back(QPageSize::AnsiD, "AnsiD");
  items.emplace_back(QPageSize::AnsiE, "AnsiE");

  items.emplace_back(QPageSize::LegalExtra, "LegalExtra");
  items.emplace_back(QPageSize::LetterExtra, "LetterExtra");
  items.emplace_back(QPageSize::LetterPlus, "LetterPlus");
  items.emplace_back(QPageSize::LetterSmall, "LetterSmall");
  items.emplace_back(QPageSize::TabloidExtra, "TabloidExtra");

  items.emplace_back(QPageSize::ArchA, "ArchA");
  items.emplace_back(QPageSize::ArchB, "ArchB");
  items.emplace_back(QPageSize::ArchC, "ArchC");
  items.emplace_back(QPageSize::ArchD, "ArchD");
  items.emplace_back(QPageSize::ArchE, "ArchE");

  items.emplace_back(QPageSize::Imperial7x9, "Imperial7x9");
  items.emplace_back(QPageSize::Imperial8x10, "Imperial8x10");
  items.emplace_back(QPageSize::Imperial9x11, "Imperial9x11");
  items.emplace_back(QPageSize::Imperial9x12, "Imperial9x12");
  items.emplace_back(QPageSize::Imperial10x11, "Imperial10x11");
  items.emplace_back(QPageSize::Imperial10x13, "Imperial10x13");
  items.emplace_back(QPageSize::Imperial10x14, "Imperial10x14");
  items.emplace_back(QPageSize::Imperial12x11, "Imperial12x11");
  items.emplace_back(QPageSize::Imperial15x11, "Imperial15x11");

  items.emplace_back(QPageSize::ExecutiveStandard, "ExecutiveStandard");
  items.emplace_back(QPageSize::Note, "Note");
  items.emplace_back(QPageSize::Quarto, "Quarto");
  items.emplace_back(QPageSize::Statement, "Statement");
  items.emplace_back(QPageSize::SuperA, "SuperA");
  items.emplace_back(QPageSize::SuperB, "SuperB");
  items.emplace_back(QPageSize::Postcard, "Postcard");
  items.emplace_back(QPageSize::DoublePostcard, "DoublePostcard");
  items.emplace_back(QPageSize::Prc16K, "Prc16K");
  items.emplace_back(QPageSize::Prc32K, "Prc32K");
  items.emplace_back(QPageSize::Prc32KBig, "Prc32KBig");

  items.emplace_back(QPageSize::FanFoldUS, "FanFoldUS");
  items.emplace_back(QPageSize::FanFoldGerman, "FanFoldGerman");
  items.emplace_back(QPageSize::FanFoldGermanLegal, "FanFoldGermanLegal");

  items.emplace_back(QPageSize::EnvelopeB4, "EnvelopeB4");
  items.emplace_back(QPageSize::EnvelopeB5, "EnvelopeB5");
  items.emplace_back(QPageSize::EnvelopeB6, "EnvelopeB6");

  items.emplace_back(QPageSize::EnvelopeC0, "EnvelopeC0");
  items.emplace_back(QPageSize::EnvelopeC1, "EnvelopeC1");
  items.emplace_back(QPageSize::EnvelopeC2, "EnvelopeC2");
  items.emplace_back(QPageSize::EnvelopeC3, "EnvelopeC3");
  items.emplace_back(QPageSize::EnvelopeC4, "EnvelopeC4");
  items.emplace_back(QPageSize::EnvelopeC5, "EnvelopeC5");
  items.emplace_back(QPageSize::EnvelopeC6, "EnvelopeC6");
  items.emplace_back(QPageSize::EnvelopeC65, "EnvelopeC65");
  items.emplace_back(QPageSize::EnvelopeC7, "EnvelopeC7");
  items.emplace_back(QPageSize::EnvelopeDL, "EnvelopeDL");
  items.emplace_back(QPageSize::Envelope9, "Envelope9");
  items.emplace_back(QPageSize::Envelope10, "Envelope10");
  items.emplace_back(QPageSize::Envelope11, "Envelope11");
  items.emplace_back(QPageSize::Envelope12, "Envelope12");
  items.emplace_back(QPageSize::Envelope14, "Envelope14");
  items.emplace_back(QPageSize::EnvelopeMonarch, "EnvelopeMonarch");
  items.emplace_back(QPageSize::EnvelopePersonal, "EnvelopePersonal");
  items.emplace_back(QPageSize::EnvelopeChou3, "EnvelopeChou3");
  items.emplace_back(QPageSize::EnvelopeChou4, "EnvelopeChou4");
  items.emplace_back(QPageSize::EnvelopeInvite, "EnvelopeInvite");
  items.emplace_back(QPageSize::EnvelopeItalian, "EnvelopeItalian");
  items.emplace_back(QPageSize::EnvelopeKaku2, "EnvelopeKaku2");
  items.emplace_back(QPageSize::EnvelopeKaku3, "EnvelopeKaku3");

  items.emplace_back(QPageSize::EnvelopePrc1, "EnvelopePrc1");
  items.emplace_back(QPageSize::EnvelopePrc2, "EnvelopePrc2");
  items.emplace_back(QPageSize::EnvelopePrc3, "EnvelopePrc3");
  items.emplace_back(QPageSize::EnvelopePrc4, "EnvelopePrc4");
  items.emplace_back(QPageSize::EnvelopePrc5, "EnvelopePrc5");
  items.emplace_back(QPageSize::EnvelopePrc6, "EnvelopePrc6");
  items.emplace_back(QPageSize::EnvelopePrc7, "EnvelopePrc7");
  items.emplace_back(QPageSize::EnvelopePrc8, "EnvelopePrc8");
  items.emplace_back(QPageSize::EnvelopePrc9, "EnvelopePrc9");
  items.emplace_back(QPageSize::EnvelopePrc10, "EnvelopePrc10");
  items.emplace_back(QPageSize::EnvelopeYou4, "EnvelopeYou4");

  qDebug(PAPER_SIZE_MODEL) << "Loaded " << items.size() << " standard paper sizes.";
}

void PaperSizeModel::setIndex()
{
  const auto system = QPrinter().pageSize();
  for (std::size_t i = 0; i < items.size(); ++i)
  {
    if (system == static_cast<QPagedPaintDevice::PageSize>(std::get<0>(items[i])))
    {
      systemSizeIndex = i;
      qInfo(PAPER_SIZE_MODEL) << "Using sysem default paper size at index: " << i;
       break;
    }
  }
}
