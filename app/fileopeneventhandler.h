// SPDX-FileCopyrightText: 2024 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>

class FileOpenEventHandler : public QObject
{
    Q_OBJECT

public:
    explicit FileOpenEventHandler(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void fileOpen(const QUrl &url);
};
