// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int currentIndex MEMBER m_currentIndex NOTIFY currentIndexChanged)

    enum PlaylistType {
        PL_USERPLAYLIST, // Regular playlist, managed by user.
        PL_SAMEFOLDER // PlaylistManager managed playlist, loaded from files from same folder.
    };

    explicit PlaylistManager(PlaylistType type = PL_USERPLAYLIST, QObject *parent = nullptr);
    ~PlaylistManager();

    void setPlaylistType(PlaylistType type);
    PlaylistType playlistType() const;

    QStringList autoLoadFilterSuffix() const;
    void setAutoLoadFilterSuffix(const QStringList &nameFilters);

    void clear();

    void setPlaylist(const QList<QUrl> & urls);
    void setCurrentFile(const QString & filePath);
    void setCurrentIndex(int index);
    int appendFile(const QString & filePath);
    int indexOf(const QString & filePath);

    int count() const;

    std::tuple<int, QString> previousFile() const;
    std::tuple<int, QString> nextFile() const;
    std::tuple<int, QString> currentFile() const;
    std::tuple<int, QUrl> currentFileUrl() const;

    static QList<QUrl> convertToUrlList(const QStringList & files);

signals:
    void loaded(int length);
    void currentIndexChanged(int index);

private:
    QList<QUrl> m_playlist;
    PlaylistType m_type;
    QString m_currentDir;
    int m_currentIndex = -1;
    QStringList m_autoLoadSuffix = {};
};

