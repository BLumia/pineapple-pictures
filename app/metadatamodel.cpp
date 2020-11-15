#include "metadatamodel.h"
#include "exiv2wrapper.h"

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
#if 0
    appendSection(QStringLiteral("GPS"), tr("GPS", "Section name."));
#endif // 0
    appendSection(QStringLiteral("File"), tr("File", "Section name."));

    appendProperty(QStringLiteral("Image"), QStringLiteral("Image.Dimensions"),
                   tr("Dimensions"), imageDimensionsString);
    appendProperty(QStringLiteral("Image"), QStringLiteral("Image.SizeRatio"),
                   tr("Aspect Ratio"), imageRatioString);

    appendProperty(QStringLiteral("File"), QStringLiteral("File.Name"),
                   tr("Name"), fileInfo.fileName());
    appendProperty(QStringLiteral("File"), QStringLiteral("File.ItemType"),
                   tr("Item type"), itemTypeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.Path"),
                   tr("Folder path"), fileInfo.path());
    appendProperty(QStringLiteral("File"), QStringLiteral("File.Size"),
                   tr("Size"), sizeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.CreatedTime"),
                   tr("Date Created"), birthTimeString);
    appendProperty(QStringLiteral("File"), QStringLiteral("File.LastModified"),
                   tr("Date Modified"), lastModifiedTimeString);

    Exiv2Wrapper wrapper;
    if (wrapper.load(imageFilePath)) {
        wrapper.cacheSections();

        appendProperty(QStringLiteral("Description"), QStringLiteral("Description.Comments"),
                       tr("Comments"), wrapper.comment());

        appendExivPropertyIfExist(wrapper, QStringLiteral("Origin"),
                                  QStringLiteral("Exif.Image.Software"), tr("Program name"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Image"),
                                  QStringLiteral("Exif.Photo.ColorSpace"), tr("Colour representation"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Image.Make"), tr("Camera maker"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Image.Model"), tr("Camera model"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.ISOSpeedRatings"), tr("ISO speed"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("Camera"),
                                  QStringLiteral("Exif.Photo.FocalLength"), tr("Focal length"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.DigitalZoomRatio"), tr("Digital zoom"));
        appendExivPropertyIfExist(wrapper, QStringLiteral("AdvancedPhoto"),
                                  QStringLiteral("Exif.Photo.ExifVersion"), tr("EXIF version"));
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

bool MetadataModel::appendSection(const QString &sectionKey, const QString &sectionDisplayName)
{
    if (m_sections.contains(sectionKey)) {
        return false;
    }

    m_sections.append(sectionKey);
    m_sectionProperties[sectionKey] = qMakePair<QString, QList<QString> >(sectionDisplayName, {});

    return true;
}

bool MetadataModel::appendProperty(const QString &sectionKey, const QString &propertyKey, const QString &propertyDisplayName, const QString &propertyValue)
{
    if (!m_sections.contains(sectionKey)) {
        return false;
    }

    QList<QString> & propertyList = m_sectionProperties[sectionKey].second;
    if (!propertyList.contains(propertyKey)) {
        propertyList.append(propertyKey);
    }

    m_properties[propertyKey] = qMakePair<QString, QString>(propertyDisplayName, propertyValue);

    return true;
}

bool MetadataModel::updateProperty(const QString &propertyKey, const QString &propertyValue)
{
    if (m_properties.contains(propertyKey)) {
        m_properties[propertyKey].second = propertyValue;
        return true;
    }

    return false;
}

bool MetadataModel::appendExivPropertyIfExist(const Exiv2Wrapper &wrapper, const QString &sectionKey, const QString &exiv2propertyKey, const QString &propertyDisplayName)
{
    const QString & value = wrapper.value(exiv2propertyKey);
    if (!value.isEmpty()) {
        appendProperty(sectionKey, exiv2propertyKey,
                       propertyDisplayName.isEmpty() ? wrapper.label(exiv2propertyKey) : propertyDisplayName,
                       value);
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
