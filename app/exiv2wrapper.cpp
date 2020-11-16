#include "exiv2wrapper.h"

#ifdef HAVE_EXIV2_VERSION
#include <exiv2/exiv2.hpp>
#else // HAVE_EXIV2_VERSION
namespace Exiv2 {
class Image {};
}
#endif // HAVE_EXIV2_VERSION

#include <sstream>

#include <QFile>
#include <QDebug>

Exiv2Wrapper::Exiv2Wrapper()
{

}

Exiv2Wrapper::~Exiv2Wrapper()
{

}

template<typename Collection, typename Iterator>
void Exiv2Wrapper::cacheSection(Collection collection)
{
    const Collection& exifData = collection;
    Iterator it = exifData.begin(), end = exifData.end();
    for (; it != end; ++it) {
        QString key = QString::fromUtf8(it->key().c_str());
        if (it->tagName().substr(0, 2) == "0x") continue;
        QString label = QString::fromLocal8Bit(it->tagLabel().c_str());
        std::ostringstream stream;
        stream << *it;
        QString value = QString::fromLocal8Bit(stream.str().c_str());
        m_metadataValue.insert(key, value);
        m_metadataLabel.insert(key, label);

        qDebug() << key << label << value;
    }
}

bool Exiv2Wrapper::load(const QString &filePath)
{
#ifdef HAVE_EXIV2_VERSION
    QByteArray filePathByteArray = QFile::encodeName(filePath);
    try {
        m_exivImage.reset(Exiv2::ImageFactory::open(filePathByteArray.constData()).release());
        m_exivImage->readMetadata();
    } catch (const Exiv2::Error& error) {
        m_errMsg = QString::fromUtf8(error.what());
        return false;
    }
    return true;
#else // HAVE_EXIV2_VERSION
    Q_UNUSED(filePath);
    return false;
#endif // HAVE_EXIV2_VERSION
}

void Exiv2Wrapper::cacheSections()
{
#ifdef HAVE_EXIV2_VERSION
    if (m_exivImage->checkMode(Exiv2::mdExif) & Exiv2::amRead) {
        cacheSection<Exiv2::ExifData, Exiv2::ExifData::const_iterator>(m_exivImage->exifData());
    }

    if (m_exivImage->checkMode(Exiv2::mdIptc) & Exiv2::amRead) {
        cacheSection<Exiv2::IptcData, Exiv2::IptcData::const_iterator>(m_exivImage->iptcData());
    }

    if (m_exivImage->checkMode(Exiv2::mdXmp) & Exiv2::amRead) {
        cacheSection<Exiv2::XmpData, Exiv2::XmpData::const_iterator>(m_exivImage->xmpData());
    }

//    qDebug() << m_metadataValue;
//    qDebug() << m_metadataLabel;
#endif // HAVE_EXIV2_VERSION
}

QString Exiv2Wrapper::comment() const
{
#ifdef HAVE_EXIV2_VERSION
    return m_exivImage->comment().c_str();
#else // HAVE_EXIV2_VERSION
    return QString();
#endif // HAVE_EXIV2_VERSION
}

QString Exiv2Wrapper::label(const QString &key) const
{
    return m_metadataLabel.value(key);
}

QString Exiv2Wrapper::value(const QString &key) const
{
    return m_metadataValue.value(key);
}

