#ifndef METADATAMODEL_H
#define METADATAMODEL_H

#include <QAbstractItemModel>

class Exiv2Wrapper;
class MetadataModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit MetadataModel(QObject *parent = nullptr);
    ~MetadataModel();

    void setFile(const QString & imageFilePath);
    static QString imageSize(const QSize &size);
    static QString imageSizeRatio(const QSize &size);
    bool appendSection(const QString & sectionKey, QStringView sectionDisplayName);
    bool appendPropertyIfNotEmpty(const QString & sectionKey, const QString & propertyKey,
                                  const QString & propertyDisplayName, const QString & propertyValue = QString());
    bool appendProperty(const QString & sectionKey, const QString & propertyKey,
                        QStringView propertyDisplayName, QStringView propertyValue = QString());
    bool appendExivPropertyIfExist(const Exiv2Wrapper & wrapper, const QString & sectionKey,
                                   const QString & exiv2propertyKey, const QString & propertyDisplayName = QString(),
                                   bool isXmpString = false);

private:
    enum RowType : quintptr {
        SectionRow,
        PropertyRow,
    };

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // [SECTION_KEY]
    QList<QString> m_sections;
    // {SECTION_KEY: (SECTION_DISPLAY_NAME, [PROPERTY_KEY])}
    QMap<QString, QPair<QString, QList<QString> > > m_sectionProperties;
    // {PROPERTY_KEY: (PROPERTY_DISPLAY_NAME, PROPERTY_VALUE)}
    QMap<QString, QPair<QString, QString> > m_properties;
};

#endif // METADATAMODEL_H
