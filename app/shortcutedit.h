// SPDX-FileCopyrightText: 2024 Gary Wang <opensource@blumia.net>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QWidget>
#include <QList>
#include <QKeySequence>

class QLabel;
class QFormLayout;
class QToolButton;
class QKeySequenceEdit;
class ShortcutEdit;
class ShortcutEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutEditor(ShortcutEdit * shortcutEdit, QWidget * parent = nullptr);
    ~ShortcutEditor();

    void setDescription(const QString & desc);

    void reloadShortcuts();
    void applyShortcuts();

private:
    QLabel * m_descriptionLabel;
    ShortcutEdit * m_shortcutEdit;
    QFormLayout * m_shortcutLayout;
    QList<QKeySequenceEdit *> m_keySequenceEdits;
};

class ShortcutEdit : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QKeySequence> shortcuts MEMBER m_shortcuts WRITE setShortcuts NOTIFY shortcutsChanged)
public:
    explicit ShortcutEdit(QWidget * parent = nullptr);
    ~ShortcutEdit();

    QList<QKeySequence> shortcuts() const;
    void setShortcuts(const QList<QKeySequence> &shortcuts);

signals:
    void shortcutsChanged();
    void editButtonClicked();
    void applyShortcutsRequested(QList<QKeySequence> newShortcuts);

private:
    QList<QKeySequence> m_shortcuts;
    QLabel * m_shortcutsLabel;
    QToolButton * m_setShortcutButton;
};
