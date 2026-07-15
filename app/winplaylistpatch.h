// SPDX-FileCopyrightText: 2026 Tad Young <yyc12321@outlook.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QtGlobal>

#ifdef Q_OS_WIN

#include <QUrl>
class WinPlaylistPatch
{
public:
    static bool loadPlaylist(const QUrl &url,
                             const QStringList &allowedSuffixes,
                             QList<QUrl> &playlist,
                             std::optional<qsizetype> &indexHint);
};

#endif // Q_OS_WIN
