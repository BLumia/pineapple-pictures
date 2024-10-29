// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "settingsdialog.h"

#include "settings.h"
#include "shortcutedit.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QScrollArea>
#include <QSplitter>
#include <QStringListModel>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_stayOnTop(new QCheckBox)
    , m_useLightCheckerboard(new QCheckBox)
    , m_doubleClickBehavior(new QComboBox)
    , m_mouseWheelBehavior(new QComboBox)
    , m_initWindowSizeBehavior(new QComboBox)
    , m_hiDpiRoundingPolicyBehavior(new QComboBox)
{
    this->setWindowTitle(tr("Settings"));

    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    QTabWidget * settingsTabs = new QTabWidget(this);
    mainLayout->addWidget(settingsTabs);

    QWidget * settingsFormHolder = new QWidget;
    QFormLayout * settingsForm = new QFormLayout(settingsFormHolder);
    settingsTabs->addTab(settingsFormHolder, tr("Options"));

    QSplitter * shortcutEditorSplitter = new QSplitter;
    shortcutEditorSplitter->setOrientation(Qt::Vertical);
    shortcutEditorSplitter->setChildrenCollapsible(false);
    QScrollArea * shortcutScrollArea = new QScrollArea;
    shortcutEditorSplitter->addWidget(shortcutScrollArea);
    shortcutScrollArea->setWidgetResizable(true);
    shortcutScrollArea->setMinimumHeight(200);
    QWidget * shortcutsFormHolder = new QWidget;
    QFormLayout * shortcutsForm = new QFormLayout(shortcutsFormHolder);
    shortcutScrollArea->setWidget(shortcutsFormHolder);
    settingsTabs->addTab(shortcutEditorSplitter, tr("Shortcuts"));

    for (const QAction * action : parent->actions()) {
        ShortcutEdit * shortcutEdit = new ShortcutEdit;
        shortcutEdit->setObjectName(QLatin1String("shortcut_") + action->objectName());
        shortcutEdit->setShortcuts(action->shortcuts());
        shortcutsForm->addRow(action->text(), shortcutEdit);
        connect(shortcutEdit, &ShortcutEdit::editButtonClicked, this, [=](){
            if (shortcutEditorSplitter->count() == 1) shortcutEditorSplitter->addWidget(new QWidget);
            ShortcutEditor * shortcutEditor = new ShortcutEditor(shortcutEdit);
            shortcutEditor->setDescription(tr("Editing shortcuts for action \"%1\":").arg(action->text()));
            QWidget * oldEditor = shortcutEditorSplitter->replaceWidget(1, shortcutEditor);
            shortcutEditorSplitter->setSizes({shortcutEditorSplitter->height(), 1});
            oldEditor->deleteLater();
        });
        connect(shortcutEdit, &ShortcutEdit::shortcutsChanged, this, [=](){
            Settings::instance()->setShortcutsForAction(parent, shortcutEdit->objectName().mid(9),
                                                        shortcutEdit->shortcuts());
        });
    }

    static QList< QPair<Settings::DoubleClickBehavior, QString> > _dc_options {
        { Settings::DoubleClickBehavior::Ignore, tr("Do nothing") },
        { Settings::DoubleClickBehavior::Close, tr("Close the window") },
        { Settings::DoubleClickBehavior::Maximize, tr("Toggle maximize") }
    };

    static QList< QPair<Settings::MouseWheelBehavior, QString> > _mw_options {
        { Settings::MouseWheelBehavior::Zoom, tr("Zoom in and out") },
        { Settings::MouseWheelBehavior::Switch, tr("View next or previous item") }
    };

    static QList< QPair<Settings::WindowSizeBehavior, QString> > _iws_options {
        { Settings::WindowSizeBehavior::Auto, tr("Auto size") },
        { Settings::WindowSizeBehavior::Maximized, tr("Maximized") }
    };

    static QList< QPair<Qt::HighDpiScaleFactorRoundingPolicy, QString> > _hidpi_options {
        { Qt::HighDpiScaleFactorRoundingPolicy::Round, tr("Round (Integer scaling)", "This option means round up for .5 and above") },
        { Qt::HighDpiScaleFactorRoundingPolicy::Ceil, tr("Ceil (Integer scaling)", "This option means always round up") },
        { Qt::HighDpiScaleFactorRoundingPolicy::Floor, tr("Floor (Integer scaling)", "This option means always round down") },
        { Qt::HighDpiScaleFactorRoundingPolicy::PassThrough, tr("Follow system (Fractional scaling)", "This option means don't round") }
    };

    QStringList dcbDropDown;
    for (const QPair<Settings::DoubleClickBehavior, QString> & dcOption : _dc_options) {
        dcbDropDown.append(dcOption.second);
    }

    QStringList mwbDropDown;
    for (const QPair<Settings::MouseWheelBehavior, QString> & mwOption : _mw_options) {
        mwbDropDown.append(mwOption.second);
    }

    QStringList iwsbDropDown;
    for (const QPair<Settings::WindowSizeBehavior, QString> & iwsOption : _iws_options) {
        iwsbDropDown.append(iwsOption.second);
    }

    QStringList hidpiDropDown;
    for (const QPair<Qt::HighDpiScaleFactorRoundingPolicy, QString> & hidpiOption : _hidpi_options) {
        hidpiDropDown.append(hidpiOption.second);
    }

    settingsForm->addRow(tr("Stay on top when start-up"), m_stayOnTop);
    settingsForm->addRow(tr("Use light-color checkerboard"), m_useLightCheckerboard);
    settingsForm->addRow(tr("Double-click behavior"), m_doubleClickBehavior);
    settingsForm->addRow(tr("Mouse wheel behavior"), m_mouseWheelBehavior);
    settingsForm->addRow(tr("Default window size"), m_initWindowSizeBehavior);
    settingsForm->addRow(tr("HiDPI scale factor rounding policy"), m_hiDpiRoundingPolicyBehavior);

    m_stayOnTop->setChecked(Settings::instance()->stayOnTop());
    m_useLightCheckerboard->setChecked(Settings::instance()->useLightCheckerboard());
    m_doubleClickBehavior->setModel(new QStringListModel(dcbDropDown));
    Settings::DoubleClickBehavior dcb = Settings::instance()->doubleClickBehavior();
    m_doubleClickBehavior->setCurrentIndex(static_cast<int>(dcb));
    m_mouseWheelBehavior->setModel(new QStringListModel(mwbDropDown));
    Settings::MouseWheelBehavior mwb = Settings::instance()->mouseWheelBehavior();
    m_mouseWheelBehavior->setCurrentIndex(static_cast<int>(mwb));
    m_initWindowSizeBehavior->setModel(new QStringListModel(iwsbDropDown));
    Settings::WindowSizeBehavior iwsb = Settings::instance()->initWindowSizeBehavior();
    m_initWindowSizeBehavior->setCurrentIndex(static_cast<int>(iwsb));
    m_hiDpiRoundingPolicyBehavior->setModel(new QStringListModel(hidpiDropDown));
    Qt::HighDpiScaleFactorRoundingPolicy hidpi = Settings::instance()->hiDpiScaleFactorBehavior();
    for (int i = 0; i < _hidpi_options.count(); i++) {
        if (_hidpi_options.at(i).first == hidpi) {
            m_hiDpiRoundingPolicyBehavior->setCurrentIndex(i);
            break;
        }
    }

    connect(m_stayOnTop, &QCheckBox::stateChanged, this, [ = ](int state){
        Settings::instance()->setStayOnTop(state == Qt::Checked);
    });

    connect(m_useLightCheckerboard, &QCheckBox::stateChanged, this, [ = ](int state){
        Settings::instance()->setUseLightCheckerboard(state == Qt::Checked);
    });

    connect(m_doubleClickBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setDoubleClickBehavior(_dc_options.at(index).first);
    });

    connect(m_mouseWheelBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setMouseWheelBehavior(_mw_options.at(index).first);
    });

    connect(m_initWindowSizeBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setInitWindowSizeBehavior(_iws_options.at(index).first);
    });

    connect(m_hiDpiRoundingPolicyBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setHiDpiScaleFactorBehavior(_hidpi_options.at(index).first);
    });

    adjustSize();
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

SettingsDialog::~SettingsDialog()
{

}
