// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "playlistmanager.h"

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

PlaylistManager::PlaylistManager(PlaylistType type, QObject *parent)
    : QObject(parent)
    , m_type(type)
{

}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::setPlaylistType(PlaylistManager::PlaylistType type)
{
    m_type = type;
}

PlaylistManager::PlaylistType PlaylistManager::playlistType() const
{
    return m_type;
}

QStringList PlaylistManager::autoLoadFilterSuffix() const
{
    return m_autoLoadSuffix;
}

void PlaylistManager::setAutoLoadFilterSuffix(const QStringList & nameFilters)
{
    m_autoLoadSuffix = nameFilters;
}

void PlaylistManager::clear()
{
    m_currentIndex = -1;
    m_playlist.clear();
}

void PlaylistManager::setPlaylist(const QList<QUrl> &urls)
{
    m_playlist = urls;
}

void PlaylistManager::setCurrentFile(const QString & filePath)
{
    QFileInfo info(filePath);
    QDir dir(info.path());
    QString && currentFileName = info.fileName();

    switch (playlistType()) {
    case PL_SAMEFOLDER: {
        if (dir.path() == m_currentDir) {
            int index = indexOf(filePath);
            m_currentIndex = index == -1 ? appendFile(filePath) : index;
        } else {
            QStringList entryList = dir.entryList(
                        m_autoLoadSuffix,
                        QDir::Files | QDir::NoSymLinks, QDir::NoSort);

            QCollator collator;
            collator.setNumericMode(true);

            std::sort(entryList.begin(), entryList.end(), collator);

            clear();

            int index = -1;
            for (int i = 0; i < entryList.count(); i++) {
                const QString & fileName = entryList.at(i);
                const QString & oneEntry = dir.absoluteFilePath(fileName);
                const QUrl & url = QUrl::fromLocalFile(oneEntry);
                m_playlist.append(url);
                if (fileName == currentFileName) {
                    index = i;
                }
            }
            m_currentIndex = index == -1 ? appendFile(filePath) : index;
            m_currentDir = dir.path();
        }
        break;
    }
    case PL_USERPLAYLIST:{
        int index = indexOf(filePath);
        m_currentIndex = index == -1 ? appendFile(filePath) : index;
        break;
    }
    default:
        break;
    }

    emit currentIndexChanged(m_currentIndex);
    emit loaded(m_playlist.count());
}

void PlaylistManager::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_playlist.count()) return;
    m_currentIndex = index;
    emit currentIndexChanged(m_currentIndex);
}

int PlaylistManager::appendFile(const QString &filePath)
{
    int index = m_playlist.length();
    m_playlist.append(QUrl::fromLocalFile(filePath));

    return index;
}

// Note: this will only remove file out of the list, this will NOT delete the file
void PlaylistManager::removeFileAt(int index)
{
    m_playlist.removeAt(index);

    if (m_playlist.count() <= m_currentIndex) {
        m_currentIndex--;
    }
}

int PlaylistManager::indexOf(const QString &filePath)
{
    const QUrl & url = QUrl::fromLocalFile(filePath);
    return m_playlist.indexOf(url);
}

int PlaylistManager::count() const
{
    return m_playlist.count();
}

std::tuple<int, QString> PlaylistManager::previousFile() const
{
    int count = m_playlist.count();
    if (count == 0) return std::make_tuple(-1, QString());

    int index = m_currentIndex - 1 < 0 ? count - 1 : m_currentIndex - 1;
    return std::make_tuple(index, m_playlist.at(index).toLocalFile());
}

std::tuple<int, QString> PlaylistManager::nextFile() const
{
    int count = m_playlist.count();
    if (count == 0) return std::make_tuple(-1, QString());

    int index = m_currentIndex + 1 == count ? 0 : m_currentIndex + 1;
    return std::make_tuple(index, m_playlist.at(index).toLocalFile());
}

std::tuple<int, QString> PlaylistManager::currentFile() const
{
    if (m_playlist.count() == 0) return std::make_tuple(-1, QString());

    return std::make_tuple(m_currentIndex, m_playlist.at(m_currentIndex).toLocalFile());
}

std::tuple<int, QUrl> PlaylistManager::currentFileUrl() const
{
    if (m_playlist.count() == 0) return std::make_tuple(-1, QUrl());

    return std::make_tuple(m_currentIndex, m_playlist.at(m_currentIndex));
}

QList<QUrl> PlaylistManager::convertToUrlList(const QStringList &files)
{
    QList<QUrl> urlList;
    for (const QString & str : std::as_const(files)) {
        QUrl url = QUrl::fromLocalFile(str);
        if (url.isValid()) {
            urlList.append(url);
        }
    }

    return urlList;
}
