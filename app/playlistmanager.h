// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QUrl>
#include <QAbstractListModel>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PlaylistRole {
        UrlRole = Qt::UserRole
    };
    Q_ENUM(PlaylistRole)
    Q_PROPERTY(QStringList autoLoadFilterSuffixes MEMBER m_autoLoadSuffixes NOTIFY autoLoadFilterSuffixesChanged)

    explicit PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel() override;

    void setPlaylist(const QList<QUrl> & urls);
    QModelIndex loadPlaylist(const QList<QUrl> & urls);
    QModelIndex loadPlaylist(const QUrl & url);
    QModelIndex appendToPlaylist(const QUrl & url);
    bool removeAt(int index);
    int indexOf(const QUrl & url) const;
    QUrl urlByIndex(int index) const;
    QStringList autoLoadFilterSuffixes() const;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void autoLoadFilterSuffixesChanged(QStringList suffixes);

private:
    // model data
    QList<QUrl> m_playlist;
    // properties
    QStringList m_autoLoadSuffixes = {};
    // internal
    QString m_currentDir;
};

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int currentIndex MEMBER m_currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QStringList autoLoadFilterSuffixes WRITE setAutoLoadFilterSuffixes)
    Q_PROPERTY(PlaylistModel * model READ model CONSTANT)

    explicit PlaylistManager(QObject *parent = nullptr);
    ~PlaylistManager();

    PlaylistModel * model();

    void setPlaylist(const QList<QUrl> & url);
    Q_INVOKABLE QModelIndex loadPlaylist(const QList<QUrl> & urls);
    Q_INVOKABLE QModelIndex loadPlaylist(const QUrl & url);

    inline int totalCount() const;
    QModelIndex previousIndex() const;
    QModelIndex nextIndex() const;
    QModelIndex curIndex() const;
    inline bool isFirstIndex() const;
    inline bool isLastIndex() const;
    void setCurrentIndex(const QModelIndex & index);
    QUrl urlByIndex(const QModelIndex & index);
    QString localFileByIndex(const QModelIndex & index);
    bool removeAt(const QModelIndex & index);

    void setAutoLoadFilterSuffixes(const QStringList &nameFilters);

    static QList<QUrl> convertToUrlList(const QStringList & files);

signals:
    void currentIndexChanged(int index);
    void totalCountChanged(int count);

private:
    int m_currentIndex = -1;
    PlaylistModel m_model;
};
