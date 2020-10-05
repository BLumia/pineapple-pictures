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
{
    this->setWindowTitle(tr("Settings"));

    QFormLayout * settingsForm = new QFormLayout(this);

    static QMap<DoubleClickBehavior, QString> _map {
        { ActionDoNothing, tr("Do nothing") },
        { ActionCloseWindow, tr("Close the window") },
        { ActionMaximizeWindow, tr("Toggle maximize") }
    };

    QStringList dropDown;
    for (int dcb = ActionStart; dcb <= ActionEnd; dcb++) {
        dropDown.append(_map.value(static_cast<DoubleClickBehavior>(dcb)));
    }

    settingsForm->addRow(tr("Stay on top when start-up"), m_stayOnTop);
    settingsForm->addRow(tr("Double-click behavior"), m_doubleClickBehavior);

    m_stayOnTop->setChecked(Settings::instance()->stayOnTop());
    m_doubleClickBehavior->setModel(new QStringListModel(dropDown));
    DoubleClickBehavior dcb = Settings::instance()->doubleClickBehavior();
    m_doubleClickBehavior->setCurrentIndex(static_cast<int>(dcb));

    connect(m_stayOnTop, &QCheckBox::stateChanged, this, [ = ](int state){
        Settings::instance()->setStayOnTop(state == Qt::Checked);
    });

    connect(m_doubleClickBehavior, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [ = ](int index){
        Settings::instance()->setDoubleClickBehavior(static_cast<DoubleClickBehavior>(index));
    });

    this->setMinimumSize(300, 61); // not sure why it complain "Unable to set geometry"
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

SettingsDialog::~SettingsDialog()
{

}
