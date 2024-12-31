// SPDX-FileCopyrightText: 2024 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#include "fileopeneventhandler.h"

#include <QFileOpenEvent>

FileOpenEventHandler::FileOpenEventHandler(QObject *parent)
    : QObject(parent)
{
}

bool FileOpenEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
        emit fileOpen(fileOpenEvent->url());
        return true;
    }
    return QObject::eventFilter(obj, event);
}
