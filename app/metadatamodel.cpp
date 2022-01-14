#include "metadatamodel.h"
#include "exiv2wrapper.h"

#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QImageReader>

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

    appendSection(QStringLiteral("Description"), tr("Description", "Section name."));
    appendSection(QStringLiteral("Origin"), tr("Origin", "Section name."));
    appendSection(QStringLiteral("Image"), tr("Image", "Section name."));
    appendSection(QStringLiteral("Camera"), tr("Camera", "Section name."));
    appendSection(QStringLiteral("AdvancedPhoto"), tr("Advanced photo", "Section name."));
    appendSection(QStringLiteral("GPS"), tr("GPS", "Section name."));
    appendSection(QStringLiteral("File"), tr("File", "Section name."));

    if (imgReader.supportsOption(QImageIOHandler::Size)) {
        appendProperty(QStringLiteral("Image"), QStringLiteral("Image.Dimensions"),
                       tr("Dimensions"), imageDimensionsString);
        appendProperty(QStringLiteral("Image"), QStringLiteral("Image.SizeRatio"),
                       tr("Aspect ratio"), imageRatioString);
    }
    if (imgReader.supportsAnimation() && imgReader.imageCount() > 1) {
        appendProperty(QStringLiteral("Image"), QStringLiteral("Image.FrameCount"),
                       tr("Frame count"), QString::number(imgReader.imageCount()));
    }

    appendProperty(QStringLiteral("File"), QStringLiteral("File.Name"),
                   tr("Name"), fileInfo.fileName());
    appendProperty(QStringLiteral("File"), QStringLiteral("File.ItemType"),
                   tr("Item type"), itemTypeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.Path"),
                   tr("Folder path"), QDir::toNativeSeparators(fileInfo.path()));
    appendProperty(QStringLiteral("File"), QStringLiteral("File.Size"),
                   tr("Size"), sizeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.CreatedTime"),
                   tr("Date created"), birthTimeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.LastModified"),
                   tr("Date modified"), lastModifiedTimeString);

    Exiv2Wrapper wrapper;
    if (wrapper.load(imageFilePath)) {
        wrapper.cacheSections();

        appendExivPropertyIfExist(wrapper, QStringLiteral("Description"),
                                  QStringLiteral("Xmp.dc.title"), tr("Title"), true);
        appendExivPropertyIfExist(wrapper, QStringLiteral("Description"),
                                  QStringLiteral("Exif.Image.ImageDescription"), tr("Subject"), true);
        appendExivPropertyIfExist(wrapper, QStringLiteral("Description"),
                                  QStringLiteral("Exif.Image.Rating"), tr("Rating"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Description"),
                                  QStringLiteral("Xmp.dc.subject"), tr("Tags"));
        appendPropertyIfNotEmpty(QStringLiteral("Description"), QStringLiteral("Description.Comments"),
                                 tr("Comments"), wrapper.comment());

        appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
                                  QStringLiteral("Exif.Image.Artist"), tr("Authors"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
                                  QStringLiteral("Exif.Photo.DateTimeOriginal"), tr("Date taken"));
        // FIXME: We may fetch the same type of metadata from different metadata collection.
        //        Current implementation is not pretty and may need to do a rework...
        // appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
        //                                  QStringLiteral("Xmp.xmp.CreatorTool"), tr("Program name"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
                                  QStringLiteral("Exif.Image.Software"), tr("Program name"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
                                  QStringLiteral("Exif.Image.Copyright"), tr("Copyright"));

        appendExivPropertyIfExist(wrapper, QStringLiteral("Image"),
                                  QStringLiteral("Exif.Image.XResolution"), tr("Horizontal resolution"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Image"),
                                  QStringLiteral("Exif.Image.YResolution"), tr("Vertical resolution"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Image"),
                                  QStringLiteral("Exif.Image.ResolutionUnit"), tr("Resolution unit"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Image"),
                                  QStringLiteral("Exif.Photo.ColorSpace"), tr("Colour representation"));

        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Image.Make"), tr("Camera maker"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Image.Model"), tr("Camera model"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.FNumber"), tr("F-stop"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.ExposureTime"), tr("Exposure time"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.ISOSpeedRatings"), tr("ISO speed"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.ExposureBiasValue"), tr("Exposure bias"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.FocalLength"), tr("Focal length"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.MaxApertureValue"), tr("Max aperture"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.MeteringMode"), tr("Metering mode"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.Flash"), tr("Flash mode"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.FocalLengthIn35mmFilm"), tr("35mm focal length"));

        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.LensModel"), tr("Lens model"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.BrightnessValue"), tr("Brightness"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.ExposureProgram"), tr("Exposure program"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.Saturation"), tr("Saturation"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.Sharpness"), tr("Sharpness"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.WhiteBalance"), tr("White balance"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.DigitalZoomRatio"), tr("Digital zoom"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.ExifVersion"), tr("EXIF version"));

        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSLatitudeRef"), tr("Latitude reference"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSLatitude"), tr("Latitude"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSLongitudeRef"), tr("Longitude reference"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSLongitude"), tr("Longitude"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSAltitudeRef"), tr("Altitude reference"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("GPS"),
                                  QStringLiteral("Exif.GPSInfo.GPSAltitude"), tr("Altitude"));

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
