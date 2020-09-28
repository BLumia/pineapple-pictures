#include "aboutdialog.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(new QTabWidget)
    , m_buttonBox(new QDialogButtonBox)
    , m_helpTextEdit(new QTextBrowser)
    , m_aboutTextEdit(new QTextBrowser)
    , m_licenseTextEdit(new QTextBrowser)
{
    this->setWindowTitle(tr("About"));

    QStringList helpStr {
        tr("Launch application with image file path as argument to load the file."),
        tr("Drag and drop image file onto the window is also supported."),
        "",
        tr("Context menu option explanation:"),
        ("* " + QCoreApplication::translate("MainWindow", "Stay on top") + " : "
              + this->tr("Make window stay on top of all other windows.")),
        ("* " + QCoreApplication::translate("MainWindow", "Protected mode") + " : "
              + this->tr("Avoid close window accidentally. (eg. by double clicking the window)"))
    };

    QStringList aboutStr {
        QStringLiteral("<center><img width='128' height='128' src=':/icons/app-icon.svg'/><br/>"),
        qApp->applicationDisplayName(),
        "<hr/>",
        tr("Built with Qt %1 (%2)").arg(QT_VERSION_STR, QSysInfo::buildCpuArchitecture()),
        QStringLiteral("<br/><a href='%1'>%2</a>").arg("https://github.com/BLumia/pineapple-pictures", tr("Source code")),
        "</center>"
    };

    QString licenseDescStr(tr(
        "<p><i>%1</i> is released under the MIT License.</p>"
        "<p>This license grants people a number of freedoms:</p>"
        "<ul>"
        "<li>You are free to use <i>%1</i>, for any purpose</li>"
        "<li>You are free to distribute <i>%1</i></li>"
        "<li>You can study how <i>%1</i> works and change it</li>"
        "<li>You can distribute changed versions of <i>%1</i></li>"
        "</ul>"
        "<p>The MIT license guarantees you this freedom. Nobody is ever permitted to take it away.</p>"
    ));

    QStringList licenseStr {
        QStringLiteral("<h1 align='center'><b>%1</b></h1>").arg(tr("Your Rights")),
        licenseDescStr,
        QStringLiteral("<hr/><pre>%2</pre>")
    };

    QString mitLicense(QStringLiteral(R"(Expat/MIT License

Copyright (c) 2020 BLumia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
)"));

    m_helpTextEdit->setText(helpStr.join('\n'));

    m_aboutTextEdit->setText(aboutStr.join('\n'));
    m_aboutTextEdit->setOpenExternalLinks(true);

    m_licenseTextEdit->setText(licenseStr.join('\n').arg(qApp->applicationDisplayName(), mitLicense));

    m_tabWidget->addTab(m_helpTextEdit, tr("&Help"));
    m_tabWidget->addTab(m_aboutTextEdit, tr("&About"));
    m_tabWidget->addTab(m_licenseTextEdit, tr("&License"));

    m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
    connect(m_buttonBox, QOverload<QAbstractButton *>::of(&QDialogButtonBox::clicked), this, [this](){
        this->close();
    });

    QVBoxLayout * mainLayout = new QVBoxLayout;

    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);

    this->setLayout(mainLayout);
    this->setMinimumSize(361, 161); // not sure why it complain "Unable to set geometry"
    this->resize(520, 320);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

AboutDialog::~AboutDialog()
{

}
