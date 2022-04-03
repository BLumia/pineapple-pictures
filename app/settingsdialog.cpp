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

    static QList< QPair<Settings::DoubleClickBehavior, QString> > _dc_options {
        { Settings::DoubleClickBehavior::Ignore, tr("Do nothing") },
        { Settings::DoubleClickBehavior::Close, tr("Close the window") },
        { Settings::DoubleClickBehavior::Maximize, tr("Toggle maximize") }
    };

    static QList< QPair<Settings::MouseWheelBehavior, QString> > _mw_options {
        { Settings::MouseWheelBehavior::Zoom, tr("Zoom in and out") },
        { Settings::MouseWheelBehavior::Switch, tr("View next or previous item") }
    };

    // TODO: copywriting
    static QList< QPair<Settings::WindowSizeBehavior, QString> > _iws_options {
        { Settings::WindowSizeBehavior::Auto, "Auto size" },
        { Settings::WindowSizeBehavior::Maximized, "Maximize" }
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

    settingsForm->addRow(tr("Stay on top when start-up"), m_stayOnTop);
    settingsForm->addRow(tr("Double-click behavior"), m_doubleClickBehavior);
    settingsForm->addRow(tr("Mouse wheel behavior"), m_mouseWheelBehavior);
    // TODO: copywriting
    // settingsForm->addRow("Init window size behavior", m_initWindowSizeBehavior);

    m_stayOnTop->setChecked(Settings::instance()->stayOnTop());
    m_doubleClickBehavior->setModel(new QStringListModel(dcbDropDown));
    Settings::DoubleClickBehavior dcb = Settings::instance()->doubleClickBehavior();
    m_doubleClickBehavior->setCurrentIndex(static_cast<int>(dcb));
    m_mouseWheelBehavior->setModel(new QStringListModel(mwbDropDown));
    Settings::MouseWheelBehavior mwb = Settings::instance()->mouseWheelBehavior();
    m_mouseWheelBehavior->setCurrentIndex(static_cast<int>(mwb));
    m_initWindowSizeBehavior->setModel(new QStringListModel(iwsbDropDown));
    Settings::WindowSizeBehavior iwsb = Settings::instance()->initWindowSizeBehavior();
    m_initWindowSizeBehavior->setCurrentIndex(static_cast<int>(iwsb));

    connect(m_stayOnTop, &QCheckBox::stateChanged, this, [ = ](int state){
        Settings::instance()->setStayOnTop(state == Qt::Checked);
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

    this->setMinimumSize(300, 90); // not sure why it complain "Unable to set geometry"
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

SettingsDialog::~SettingsDialog()
{

}
