#include "ExifModel.h"
#include "../ext/exif.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QLoggingCategory>
#include <QtGui/QColor>

using com::sptci::ExifModel;

Q_LOGGING_CATEGORY(EXIF_MODEL, "com.sptci.ExifModel")

ExifModel::ExifModel(const QString& file, QObject* parent) :
    QAbstractTableModel(parent), file(file)
{
  readData();
}

void ExifModel::readData()
{
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly))
  {
    const auto error = QString("Unable to open file: %1").arg(file);
    qDebug(EXIF_MODEL) << error;
    map["Error"] = error;
    return;
  }

  easyexif::EXIFInfo result;
  auto data = f.readAll();
  if (int code = result.parseFrom(
      reinterpret_cast<unsigned char*>(const_cast<char *>(data.data())),
      static_cast<uint>(data.size())))
  {
    const auto error = QString("Error parsing EXIF from JPEG: %1 for file: %s").
        arg(code).arg(file);
    qDebug(EXIF_MODEL) << error;
    switch (code)
    {
      case PARSE_EXIF_ERROR_NO_JPEG:
        map["Error"] = "No JPEG markers found in buffer, possibly invalid JPEG file.";
        break;
      case PARSE_EXIF_ERROR_NO_EXIF:
        map["Error"] = "No EXIF header found in JPEG file.";
        break;
      case PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN:
        map["Error"] = "Byte alignment specified in EXIF file was unknown (not Motorola or Intel).";
        break;
      case PARSE_EXIF_ERROR_CORRUPT:
        map["Error"] = "EXIF header was found, but data was corrupted.";
        break;
      default: map["Error"] = error;
    }
    return;
  }

  map["Camera make"] = QString::fromStdString(result.Make);
  map["Camera model"] = QString::fromStdString(result.Model);
  map["Software"] = QString::fromStdString(result.Software);
  map["Bits per sample"] = QString("%1").arg(result.BitsPerSample);
  map["Image width"] = QString("%1").arg(result.ImageWidth);
  map["Image height"] = QString("%1").arg(result.ImageHeight);
  map["Image description"] = QString::fromStdString(result.ImageDescription);
  map["Image orientation"] = QString("%1").arg(result.Orientation);
  map["Image copyright"] = QString::fromStdString(result.Copyright);
  map["Image date/time"] = QString::fromStdString(result.DateTime);
  map["Original date/time"] = QString::fromStdString(result.DateTimeOriginal);
  map["Digitized date/time"] = QString::fromStdString(result.DateTimeDigitized);
  map["Subsecond time"] = QString::fromStdString(result.SubSecTimeOriginal);
  map["Exposure time"] = QString("1/%1").arg(static_cast<uint>(1.0 / result.ExposureTime));
  map["F-stop"] = QString("f/%1").arg(result.FNumber);
  map["Exposure program"] = QString("%1").arg(result.ExposureProgram);
  map["ISO speed"] = QString("%1").arg(result.ISOSpeedRatings);
  map["Subject distance"] = QString("%1").arg(result.SubjectDistance);
  map["Exposure bias"] = QString("%1 EV").arg(result.SubjectDistance);
  map["Flash used?"] = QString("%1").arg(result.Flash);
  map["Flash returned light"] = QString("%1").arg(result.FlashReturnedLight);
  map["Flash mode"] = QString("%1").arg(result.FlashMode);
  map["Metering mode"] = QString("%1").arg(result.MeteringMode);
  map["Lens focal length"] = QString("%1").arg(result.FocalLength);
  map["35mm focal length"] = QString("%1").arg(result.FocalLengthIn35mm);
  map["GPS Latitude"] = QString("%1 deg (%2 deg, %3 min, %4 sec %5)").
      arg(result.GeoLocation.Latitude).
      arg(result.GeoLocation.LatComponents.degrees).
      arg(result.GeoLocation.LatComponents.minutes).
      arg(result.GeoLocation.LatComponents.seconds).
      arg(result.GeoLocation.LatComponents.direction);
  map["GPS Longitude"] = QString("%1 deg (%2 deg, %3 min, %4 sec %5)").
      arg(result.GeoLocation.Longitude).
      arg(result.GeoLocation.LonComponents.degrees).
      arg(result.GeoLocation.LonComponents.minutes).
      arg(result.GeoLocation.LonComponents.seconds).
      arg(result.GeoLocation.LonComponents.direction);
  map["GPS Altitude"] = QString("%1").arg(result.GeoLocation.Altitude);
  map["GPS Precision (DOP)"] = QString("%1").arg(result.GeoLocation.DOP);
  map["Lens min focal length"] = QString("%1 mm").arg(result.LensInfo.FocalLengthMin);
  map["Lens max focal length"] = QString("%1 mm").arg(result.LensInfo.FocalLengthMax);
  map["Lens f-stop min"] = QString("f/%1").arg(result.LensInfo.FStopMin);
  map["Lens f-stop max"] = QString("f/%1").arg(result.LensInfo.FStopMax);
  map["Lens make"] = QString::fromStdString(result.LensInfo.Make);
  map["Lens model"] = QString::fromStdString(result.LensInfo.Model);
  map["Focal plane XRes"] = QString("%1").arg(result.LensInfo.FocalPlaneXResolution);
  map["Focal plane YRes"] = QString("%1").arg(result.LensInfo.FocalPlaneYResolution);
}

int ExifModel::rowCount(const QModelIndex& /* parent */) const
{
  return map.size();
}

QVariant ExifModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() >= map.size() || index.column() > 1) return QVariant();
  if (Qt::DisplayRole == role)
  {
    return (index.column() == 0) ? map.keys().at(index.row()) :
      map.values().at(index.row());
  }

#if defined(Q_OS_MAC)
  if (Qt::BackgroundRole == role) return QColor(Qt::white);
  if (Qt::ForegroundRole == role) return QColor(Qt::black);
#endif

  return QVariant();
}

QVariant ExifModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
  if (Qt::DisplayRole != role) return QVariant();
  if (Qt::Vertical == orientation) return QString::number(section + 1);

  switch (section)
  {
    case 0: return tr( "Metadata Tag" );
    case 1: return tr( "Value" );
    default: return QVariant();
  }
}
