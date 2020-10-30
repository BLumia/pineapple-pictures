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

    const QString & sizeString = QLocale().formattedDataSize(fileInfo.size());
    const QString & timeString = QLocale().toString(fileInfo.lastModified(), QLocale::LongFormat);
    const QString & imageSizeString = imageSize(imgReader.size());

    appendSection(QStringLiteral("General"), tr("General", "General info about the image, section name."));

    appendProperty(QStringLiteral("General"), QStringLiteral("General.Name"),
                   tr("File Name"), fileInfo.fileName());
    appendProperty(QStringLiteral("General"), QStringLiteral("General.Size"),
                   tr("File Size"), sizeString);
    appendProperty(QStringLiteral("General"), QStringLiteral("General.Time"),
                   tr("Last Modified"), timeString);
    appendProperty(QStringLiteral("General"), QStringLiteral("General.ImageSize"),
                   tr("Image Size"), imageSizeString);
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
