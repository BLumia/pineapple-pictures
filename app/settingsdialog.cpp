#include "settingsdialog.h"

#include "settings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QStringListModel>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_stayOnTop(new QCheckBox)
    , m_doubleClickBehavior(new QComboBox)
    , m_mouseWheelBehavior(new QComboBox)
    , m_initWindowSizeBehavior(new QComboBox)
{
    this->setWindowTitle(tr("Settings"));

    QFormLayout * settingsForm = new QFormLayout(this);

    static QMap<DoubleClickBehavior, QString> _dc_map {
        { ActionDoNothing, tr("Do nothing") },
        { ActionCloseWindow, tr("Close the window") },
        { ActionMaximizeWindow, tr("Toggle maximize") }
    };

    static QMap<MouseWheelBehavior, QString> _mw_map {
        { ActionZoomImage, tr("Zoom in and out") },
        { ActionPrevNextImage, tr("View next or previous item") }
    };

    static QMap<WindowSizeBehavior, QString> _iws_map {
        { ActionAutoSize, "Auto size" },
        { ActionMaximize, "Maximize" }
    };

    QStringList dcbDropDown;
    for (int dcb = DCActionStart; dcb <= DCActionEnd; dcb++) {
        dcbDropDown.append(_dc_map.value(static_cast<DoubleClickBehavior>(dcb)));
    }

    QStringList mwbDropDown;
    for (int mwb = MWActionStart; mwb <= MWActionEnd; mwb++) {
        mwbDropDown.append(_mw_map.value(static_cast<MouseWheelBehavior>(mwb)));
    }

    QStringList iwsbDropDown;
    for (int iwsb = IWSActionStart; iwsb <= IWSActionEnd; iwsb++) {
        iwsbDropDown.append(_iws_map.value(static_cast<WindowSizeBehavior>(iwsb)));
    }

    settingsForm->addRow(tr("Stay on top when start-up"), m_stayOnTop);
    settingsForm->addRow(tr("Double-click behavior"), m_doubleClickBehavior);
    settingsForm->addRow(tr("Mouse wheel behavior"), m_mouseWheelBehavior);
    settingsForm->addRow("Init window size behavior", m_initWindowSizeBehavior);

    m_stayOnTop->setChecked(Settings::instance()->stayOnTop());
    m_doubleClickBehavior->setModel(new QStringListModel(dcbDropDown));
    DoubleClickBehavior dcb = Settings::instance()->doubleClickBehavior();
    m_doubleClickBehavior->setCurrentIndex(static_cast<int>(dcb));
    m_mouseWheelBehavior->setModel(new QStringListModel(mwbDropDown));
    MouseWheelBehavior mwb = Settings::instance()->mouseWheelBehavior();
    m_mouseWheelBehavior->setCurrentIndex(static_cast<int>(mwb));
    m_initWindowSizeBehavior->setModel(new QStringListModel(iwsbDropDown));
    WindowSizeBehavior iwsb = Settings::instance()->initWindowSizeBehavior();
    m_initWindowSizeBehavior->setCurrentIndex(static_cast<int>(iwsb));

    connect(m_stayOnTop, &QCheckBox::stateChanged, this, [ = ](int state){
        Settings::instance()->setStayOnTop(state == Qt::Checked);
    });

    connect(m_doubleClickBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setDoubleClickBehavior(static_cast<DoubleClickBehavior>(index));
    });

    connect(m_mouseWheelBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setMouseWheelBehavior(static_cast<MouseWheelBehavior>(index));
    });

    connect(m_initWindowSizeBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setInitWindowSizeBehavior(static_cast<WindowSizeBehavior>(index));
    });

    this->setMinimumSize(300, 90); // not sure why it complain "Unable to set geometry"
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

SettingsDialog::~SettingsDialog()
{

}
