#pragma once

#include <QObject>

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    enum PlaylistType {
        PL_USERPLAYLIST, // Regular playlist, managed by user.
        PL_SAMEFOLDER // PlaylistManager managed playlist, loaded from files from same folder.
    };

    explicit PlaylistManager(PlaylistType type = PL_USERPLAYLIST, QObject *parent = nullptr);
    ~PlaylistManager();

    void setPlaylistType(PlaylistType type);
    PlaylistType playlistType() const;

    void clear();

    void setPlaylist(const QList<QUrl> & urls);
    void setCurrentFile(const QString & filePath);
    void setCurrentIndex(int index);
    int appendFile(const QString & filePath);
    int indexOf(const QString & filePath);

    std::tuple<int, QString> previousFile() const;
    std::tuple<int, QString> nextFile() const;
    std::tuple<int, QString> currentFile() const;
    std::tuple<int, QUrl> currentFileUrl() const;

    static QList<QUrl> convertToUrlList(const QStringList & files);

signals:
    void loaded(int length);

private:
    QList<QUrl> m_playlist;
    PlaylistType m_type;
    QString m_currentDir;
    int m_currentIndex = -1;
};

