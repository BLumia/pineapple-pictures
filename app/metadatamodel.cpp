// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "metadatamodel.h"
#include "exiv2wrapper.h"

#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QImageReader>

using namespace Qt::Literals::StringLiterals;

MetadataModel::MetadataModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

MetadataModel::~MetadataModel()
{

}

void MetadataModel::setFile(const QString &imageFilePath)
{
    QFileInfo fileInfo(imageFilePath);
    // It'll be fine if we don't re-use the image reader we pass to the graphics scene for now.
    QImageReader imgReader(imageFilePath);
    imgReader.setAutoTransform(true);
    imgReader.setDecideFormatFromContent(true);

    const QString & itemTypeString = tr("%1 File").arg(QString(imgReader.format().toUpper()));
    const QString & sizeString = QLocale().formattedDataSize(fileInfo.size());
    const QString & birthTimeString = QLocale().toString(fileInfo.birthTime(), QLocale::LongFormat);
    const QString & lastModifiedTimeString = QLocale().toString(fileInfo.lastModified(), QLocale::LongFormat);
    const QString & imageDimensionsString = imageSize(imgReader.size());
    const QString & imageRatioString = imageSizeRatio(imgReader.size());

    appendSection(u"Description"_s, tr("Description", "Section name."));
    appendSection(u"Origin"_s, tr("Origin", "Section name."));
    appendSection(u"Image"_s, tr("Image", "Section name."));
    appendSection(u"Camera"_s, tr("Camera", "Section name."));
    appendSection(u"AdvancedPhoto"_s, tr("Advanced photo", "Section name."));
    appendSection(u"GPS"_s, tr("GPS", "Section name."));
    appendSection(u"File"_s, tr("File", "Section name."));

    if (imgReader.supportsOption(QImageIOHandler::Size)) {
        appendProperty(u"Image"_s, u"Image.Dimensions"_s,
                       tr("Dimensions"), imageDimensionsString);
        appendProperty(u"Image"_s, u"Image.SizeRatio"_s,
                       tr("Aspect ratio"), imageRatioString);
    }
    if (imgReader.supportsAnimation() && imgReader.imageCount() > 1) {
        appendProperty(u"Image"_s, u"Image.FrameCount"_s,
                       tr("Frame count"), QString::number(imgReader.imageCount()));
    }

    appendProperty(u"File"_s, u"File.Name"_s,
                   tr("Name"), fileInfo.fileName());
    appendProperty(u"File"_s, u"File.ItemType"_s,
                   tr("Item type"), itemTypeString);
    appendProperty(u"File"_s, u"File.Path"_s,
                   tr("Folder path"), QDir::toNativeSeparators(fileInfo.path()));
    appendProperty(u"File"_s, u"File.Size"_s,
                   tr("Size"), sizeString);
    appendProperty(u"File"_s, u"File.CreatedTime"_s,
                   tr("Date created"), birthTimeString);
    appendProperty(u"File"_s, u"File.LastModified"_s,
                   tr("Date modified"), lastModifiedTimeString);

    Exiv2Wrapper wrapper;
    if (wrapper.load(imageFilePath)) {
        wrapper.cacheSections();

        appendExivPropertyIfExist(wrapper, u"Description"_s,
                                  u"Xmp.dc.title"_s, tr("Title"), true);
        appendExivPropertyIfExist(wrapper, u"Description"_s,
                                  u"Exif.Image.ImageDescription"_s, tr("Subject"), true);
        appendExivPropertyIfExist(wrapper, u"Description"_s,
                                  u"Exif.Image.Rating"_s, tr("Rating"));
        appendExivPropertyIfExist(wrapper, u"Description"_s,
                                  u"Xmp.dc.subject"_s, tr("Tags"));
        appendPropertyIfNotEmpty(u"Description"_s, u"Description.Comments"_s,
                                 tr("Comments"), wrapper.comment());

        appendExivPropertyIfExist(wrapper, u"Origin"_s,
                                  u"Exif.Image.Artist"_s, tr("Authors"));
        appendExivPropertyIfExist(wrapper, u"Origin"_s,
                                  u"Exif.Photo.DateTimeOriginal"_s, tr("Date taken"));
        // FIXME: We may fetch the same type of metadata from different metadata collection.
        //        Current implementation is not pretty and may need to do a rework...
        // appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
        //                                  QStringLiteral("Xmp.xmp.CreatorTool"), tr("Program name"));
        appendExivPropertyIfExist(wrapper, u"Origin"_s,
                                  u"Exif.Image.Software"_s, tr("Program name"));
        appendExivPropertyIfExist(wrapper, u"Origin"_s,
                                  u"Exif.Image.Copyright"_s, tr("Copyright"));

        appendExivPropertyIfExist(wrapper, u"Image"_s,
                                  u"Exif.Image.XResolution"_s, tr("Horizontal resolution"));
        appendExivPropertyIfExist(wrapper, u"Image"_s,
                                  u"Exif.Image.YResolution"_s, tr("Vertical resolution"));
        appendExivPropertyIfExist(wrapper, u"Image"_s,
                                  u"Exif.Image.ResolutionUnit"_s, tr("Resolution unit"));
        appendExivPropertyIfExist(wrapper, u"Image"_s,
                                  u"Exif.Photo.ColorSpace"_s, tr("Colour representation"));

        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Image.Make"_s, tr("Camera maker"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Image.Model"_s, tr("Camera model"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.FNumber"_s, tr("F-stop"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.ExposureTime"_s, tr("Exposure time"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.ISOSpeedRatings"_s, tr("ISO speed"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.ExposureBiasValue"_s, tr("Exposure bias"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.FocalLength"_s, tr("Focal length"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.MaxApertureValue"_s, tr("Max aperture"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.MeteringMode"_s, tr("Metering mode"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.SubjectDistance"_s, tr("Subject distance"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.Flash"_s, tr("Flash mode"));
        appendExivPropertyIfExist(wrapper, u"Camera"_s,
                                  u"Exif.Photo.FocalLengthIn35mmFilm"_s, tr("35mm focal length"));

        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.LensModel"_s, tr("Lens model"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.Contrast"_s, tr("Contrast"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.BrightnessValue"_s, tr("Brightness"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.ExposureProgram"_s, tr("Exposure program"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.Saturation"_s, tr("Saturation"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.Sharpness"_s, tr("Sharpness"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.WhiteBalance"_s, tr("White balance"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.DigitalZoomRatio"_s, tr("Digital zoom"));
        appendExivPropertyIfExist(wrapper, u"AdvancedPhoto"_s,
                                  u"Exif.Photo.ExifVersion"_s, tr("EXIF version"));

        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSLatitudeRef"_s, tr("Latitude reference"));
        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSLatitude"_s, tr("Latitude"));
        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSLongitudeRef"_s, tr("Longitude reference"));
        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSLongitude"_s, tr("Longitude"));
        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSAltitudeRef"_s, tr("Altitude reference"));
        appendExivPropertyIfExist(wrapper, u"GPS"_s,
                                  u"Exif.GPSInfo.GPSAltitude"_s, tr("Altitude"));

    }
}

QString MetadataModel::imageSize(const QSize &size)
{
    QString imageSize;

    if (size.isValid()) {
        imageSize = tr("%1 x %2").arg(QString::number(size.width()), QString::number(size.height()));
    } else {
        imageSize = QLatin1Char('-');
    }

    return imageSize;
}

int simplegcd(int a, int b) {
    return b == 0 ? a : simplegcd(b, a % b);
}

QString MetadataModel::imageSizeRatio(const QSize &size)
{
    if (!size.isValid()) {
        return QStringLiteral("-");
    }
    int gcd = simplegcd(size.width(), size.height());
    return tr("%1 : %2").arg(QString::number(size.width() / gcd), QString::number(size.height() / gcd));
}

bool MetadataModel::appendSection(const QString &sectionKey, QStringView sectionDisplayName)
{
    if (m_sections.contains(sectionKey)) {
        return false;
    }

    m_sections.append(sectionKey);
    m_sectionProperties[sectionKey] = qMakePair<QString, QList<QString> >(sectionDisplayName.toString(), {});

    return true;
}

bool MetadataModel::appendPropertyIfNotEmpty(const QString &sectionKey, const QString &propertyKey, const QString &propertyDisplayName, const QString &propertyValue)
{
    if (propertyValue.isEmpty()) return false;

    return appendProperty(sectionKey, propertyKey, propertyDisplayName, propertyValue);
}

bool MetadataModel::appendProperty(const QString &sectionKey, const QString &propertyKey, QStringView propertyDisplayName, QStringView propertyValue)
{
    if (!m_sections.contains(sectionKey)) {
        return false;
    }

    QList<QString> & propertyList = m_sectionProperties[sectionKey].second;
    if (!propertyList.contains(propertyKey)) {
        propertyList.append(propertyKey);
    }

    m_properties[propertyKey] = qMakePair<QString, QString>(propertyDisplayName.toString(), propertyValue.toString());

    return true;
}

bool MetadataModel::appendExivPropertyIfExist(const Exiv2Wrapper &wrapper, const QString &sectionKey, const QString &exiv2propertyKey, const QString &propertyDisplayName, bool isXmpString)
{
    const QString & value = wrapper.value(exiv2propertyKey);
    if (!value.isEmpty()) {
        appendProperty(sectionKey, exiv2propertyKey,
                       propertyDisplayName.isEmpty() ? wrapper.label(exiv2propertyKey) : propertyDisplayName,
                       isXmpString ? Exiv2Wrapper::XmpValue(value) : value);
        return true;
    }
    return false;
}

QModelIndex MetadataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        return createIndex(row, column, RowType::SectionRow);
    } else {
        // internalid param: row means nth section it belongs to.
        return createIndex(row, column, RowType::PropertyRow + parent.row());
    }
}

QModelIndex MetadataModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    if (child.internalId() == RowType::SectionRow) {
        return QModelIndex();
    } else {
        return createIndex(child.internalId() - RowType::PropertyRow, 0, SectionRow);
    }
}

int MetadataModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_sections.count();
    }

    if (parent.internalId() == RowType::SectionRow) {
        const QString & sectionKey = m_sections[parent.row()];
        return m_sectionProperties[sectionKey].second.count();
    }

    return 0;
}

int MetadataModel::columnCount(const QModelIndex &) const
{
    // Always key(display name) and value.
    return 2;
}

QVariant MetadataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.internalId() == RowType::SectionRow) {
        return (index.column() == 0) ? m_sectionProperties[m_sections[index.row()]].first
                                     : QVariant();
    } else {
        int sectionIndex = index.internalId() - RowType::PropertyRow;
        const QString & sectionKey = m_sections[sectionIndex];
        const QList<QString> & propertyList = m_sectionProperties[sectionKey].second;
        return (index.column() == 0) ? m_properties[propertyList[index.row()]].first
                                     : m_properties[propertyList[index.row()]].second;
    }
}

QVariant MetadataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) {
        return QVariant();
    }

    return section == 0 ? tr("Property") : tr("Value");
}
