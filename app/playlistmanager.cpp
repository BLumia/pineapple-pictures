// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#include "playlistmanager.h"

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

PlaylistModel::~PlaylistModel()
= default;

void PlaylistModel::setPlaylist(const QList<QUrl> &urls)
{
    beginResetModel();
    m_playlist = urls;
    endResetModel();
}

QModelIndex PlaylistModel::loadPlaylist(const QList<QUrl> & urls)
{
    if (urls.isEmpty()) return {};
    if (urls.count() == 1) {
        return loadPlaylist(urls.constFirst());
    } else {
        setPlaylist(urls);
        return index(0);
    }
}

QModelIndex PlaylistModel::loadPlaylist(const QUrl &url)
{
    QFileInfo info(url.toLocalFile());
    QDir dir(info.path());
    QString && currentFileName = info.fileName();

    if (dir.path() == m_currentDir) {
        int idx = indexOf(url);
        return idx == -1 ? appendToPlaylist(url) : index(idx);
    }

    QStringList entryList = dir.entryList(
        m_autoLoadSuffixes,
        QDir::Files | QDir::NoSymLinks, QDir::NoSort);

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(entryList.begin(), entryList.end(), collator);

    QList<QUrl> playlist;

    int idx = -1;
    for (int i = 0; i < entryList.count(); i++) {
        const QString & fileName = entryList.at(i);
        const QString & oneEntry = dir.absoluteFilePath(fileName);
        const QUrl & url = QUrl::fromLocalFile(oneEntry);
        playlist.append(url);
        if (fileName == currentFileName) {
            idx = i;
        }
    }
    if (idx == -1) {
        idx = playlist.count();
        playlist.append(url);
    }
    m_currentDir = dir.path();

    setPlaylist(playlist);

    return index(idx);
}

QModelIndex PlaylistModel::appendToPlaylist(const QUrl &url)
{
    const int lastIndex = rowCount();
    beginInsertRows(QModelIndex(), lastIndex, lastIndex);
    m_playlist.append(url);
    endInsertRows();
    return index(lastIndex);
}

bool PlaylistModel::removeAt(int index)
{
    if (index < 0 || index >= rowCount()) return false;
    beginRemoveRows(QModelIndex(), index, index);
    m_playlist.removeAt(index);
    endRemoveRows();
    return true;
}

int PlaylistModel::indexOf(const QUrl &url) const
{
    return m_playlist.indexOf(url);
}

QUrl PlaylistModel::urlByIndex(int index) const
{
    return m_playlist.value(index);
}

QStringList PlaylistModel::autoLoadFilterSuffixes() const
{
    return m_autoLoadSuffixes;
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractListModel::roleNames();
    result.insert(UrlRole, "url");
    return result;
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist.count();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};

    switch (role) {
    case Qt::DisplayRole:
        return m_playlist.at(index.row()).fileName();
    case UrlRole:
        return m_playlist.at(index.row());
    }

    return {};
}

PlaylistManager::PlaylistManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_model, &PlaylistModel::rowsRemoved, this,
            [this](const QModelIndex &, int, int) {
                if (m_model.rowCount() <= m_currentIndex) {
                    setProperty("currentIndex", m_currentIndex - 1);
                }
            });

    auto onRowCountChanged = [this](){
        emit totalCountChanged(m_model.rowCount());
    };

    connect(&m_model, &PlaylistModel::rowsInserted, this, onRowCountChanged);
    connect(&m_model, &PlaylistModel::rowsRemoved, this, onRowCountChanged);
    connect(&m_model, &PlaylistModel::modelReset, this, onRowCountChanged);
}

PlaylistManager::~PlaylistManager()
{

}

PlaylistModel *PlaylistManager::model()
{
    return &m_model;
}

void PlaylistManager::setPlaylist(const QList<QUrl> &urls)
{
    m_model.setPlaylist(urls);
}

QModelIndex PlaylistManager::loadPlaylist(const QList<QUrl> &urls)
{
    QModelIndex idx = m_model.loadPlaylist(urls);
    setProperty("currentIndex", idx.row());
    return idx;
}

QModelIndex PlaylistManager::loadPlaylist(const QUrl &url)
{
    QModelIndex idx = m_model.loadPlaylist(url);
    setProperty("currentIndex", idx.row());
    return idx;
}

int PlaylistManager::totalCount() const
{
    return m_model.rowCount();
}

QModelIndex PlaylistManager::previousIndex() const
{
    int count = totalCount();
    if (count == 0) return {};

    return m_model.index(isFirstIndex() ? count - 1 : m_currentIndex - 1);
}

QModelIndex PlaylistManager::nextIndex() const
{
    int count = totalCount();
    if (count == 0) return {};

    return m_model.index(isLastIndex() ? 0 : m_currentIndex + 1);
}

QModelIndex PlaylistManager::curIndex() const
{
    return m_model.index(m_currentIndex);
}

bool PlaylistManager::isFirstIndex() const
{
    return m_currentIndex == 0;
}

bool PlaylistManager::isLastIndex() const
{
    return m_currentIndex + 1 == totalCount();
}

void PlaylistManager::setCurrentIndex(const QModelIndex &index)
{
    if (index.isValid() && index.row() >= 0 && index.row() < totalCount()) {
        setProperty("currentIndex", index.row());
    }
}

QUrl PlaylistManager::urlByIndex(const QModelIndex &index)
{
    return m_model.urlByIndex(index.row());
}

QString PlaylistManager::localFileByIndex(const QModelIndex &index)
{
    return urlByIndex(index).toLocalFile();
}

bool PlaylistManager::removeAt(const QModelIndex &index)
{
    return m_model.removeAt(index.row());
}

void PlaylistManager::setAutoLoadFilterSuffixes(const QStringList &nameFilters)
{
    m_model.setProperty("autoLoadFilterSuffixes", nameFilters);
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
