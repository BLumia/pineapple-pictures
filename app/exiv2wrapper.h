// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef EXIV2WRAPPER_H
#define EXIV2WRAPPER_H

#include <memory>

#include <QString>
#include <QMap>

namespace Exiv2 {
class Image;
}

class Exiv2Wrapper
{
public:
    Exiv2Wrapper();
    ~Exiv2Wrapper();

    bool load(const QString& filePath);
    void cacheSections();

    QString comment() const;
    QString label(const QString & key) const;
    QString value(const QString & key) const;

    static QString XmpValue(const QString &rawValue);
    static QString XmpValue(const QString &rawValue, QString & language);

private:
    std::unique_ptr<Exiv2::Image> m_exivImage;
    QMap<QString, QString> m_metadataValue;
    QMap<QString, QString> m_metadataLabel;
    QString m_errMsg;

    template<typename Collection, typename Iterator>
    void cacheSection(Collection collection);
};

#endif // EXIV2WRAPPER_H
