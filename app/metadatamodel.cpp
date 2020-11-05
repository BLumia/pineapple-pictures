#include "metadatamodel.h"

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

#if 0
    appendSection(QStringLiteral("Description"), tr("Description", "Section name."));
    appendSection(QStringLiteral("Origin"), tr("Origin", "Section name."));
#endif // 0
    appendSection(QStringLiteral("Image"), tr("Image", "Section name."));
#if 0
    appendSection(QStringLiteral("Camera"), tr("Camera", "Section name."));
    appendSection(QStringLiteral("AdvancedPhoto"), tr("Advanced photo", "Section name."));
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
