// SPDX-FileCopyrightText: 2024 Gary Wang <opensource@blumia.net>
//
// SPDX-License-Identifier: MIT

#include "shortcutedit.h"

#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QKeySequenceEdit>

ShortcutEditor::ShortcutEditor(ShortcutEdit * shortcutEdit, QWidget * parent)
    : QWidget(parent)
    , m_descriptionLabel(new QLabel)
    , m_shortcutEdit(shortcutEdit)
    , m_shortcutLayout(new QFormLayout)
{
    Q_CHECK_PTR(m_shortcutEdit);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Discard);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_descriptionLabel);
    layout->addLayout(m_shortcutLayout);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::clicked, this, [=](QAbstractButton *button){
        if ((QPushButton *)button == buttons->button(QDialogButtonBox::Apply)) {
            applyShortcuts();
        } else {
            reloadShortcuts();
        }
    });
    connect(shortcutEdit, &ShortcutEdit::shortcutsChanged, this, &ShortcutEditor::reloadShortcuts);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    reloadShortcuts();
}

ShortcutEditor::~ShortcutEditor()
{

}

void ShortcutEditor::setDescription(const QString &desc)
{
    m_descriptionLabel->setText(desc);
}

void ShortcutEditor::reloadShortcuts()
{
    if (!m_keySequenceEdits.isEmpty()) {
        for (QKeySequenceEdit * keyseqEdit : m_keySequenceEdits) {
            m_shortcutLayout->removeRow(keyseqEdit);
        }
        m_keySequenceEdits.clear();
    }

    QList<QKeySequence> shortcuts = m_shortcutEdit->shortcuts();
    shortcuts.append(QKeySequence());
    for (const QKeySequence & shortcut : shortcuts) {
        QKeySequenceEdit * keyseqEdit = new QKeySequenceEdit(this);
        keyseqEdit->setClearButtonEnabled(true);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        keyseqEdit->setMaximumSequenceLength(1);
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        keyseqEdit->setKeySequence(shortcut);
        m_keySequenceEdits.append(keyseqEdit);
    }

    for (int i = 0; i < m_keySequenceEdits.count(); i++) {
        m_shortcutLayout->addRow(tr("Shortcut #%1").arg(i + 1), m_keySequenceEdits.at(i));
    }
}

void ShortcutEditor::applyShortcuts()
{
    QList<QKeySequence> shortcuts;
    for (const QKeySequenceEdit * keyseqEdit : m_keySequenceEdits) {
        if (!keyseqEdit->keySequence().isEmpty() && !shortcuts.contains(keyseqEdit->keySequence())) {
            shortcuts.append(keyseqEdit->keySequence());
        }
    }
    emit m_shortcutEdit->applyShortcutsRequested(shortcuts);
}

// ----------------------------------------

ShortcutEdit::ShortcutEdit(QWidget *parent)
    : QWidget(parent)
    , m_shortcutsLabel(new QLabel(this))
    , m_setShortcutButton(new QToolButton(this))
{
    m_setShortcutButton->setText("...");

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_shortcutsLabel, 1);
    layout->addWidget(m_setShortcutButton);

    connect(this, &ShortcutEdit::shortcutsChanged, this, [=](){
        QStringList shortcutTexts;
        for (const QKeySequence & shortcut : std::as_const(m_shortcuts)) {
            shortcutTexts.append(shortcut.toString(QKeySequence::NativeText));
        }
        m_shortcutsLabel->setText(shortcutTexts.isEmpty() ? tr("No shortcuts") : shortcutTexts.join(", "));
        m_shortcutsLabel->setDisabled(shortcutTexts.isEmpty());
    });

    connect(m_setShortcutButton, &QToolButton::clicked, this, &ShortcutEdit::editButtonClicked);

    adjustSize();
}

ShortcutEdit::~ShortcutEdit()
{
}

QList<QKeySequence> ShortcutEdit::shortcuts() const
{
    return m_shortcuts;
}

void ShortcutEdit::setShortcuts(const QList<QKeySequence> &shortcuts)
{
    m_shortcuts = shortcuts;
    emit shortcutsChanged();
}
