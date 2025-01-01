// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "aboutdialog.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QFile>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(new QTabWidget)
    , m_buttonBox(new QDialogButtonBox)
    , m_helpTextEdit(new QTextBrowser)
    , m_aboutTextEdit(new QTextBrowser)
    , m_specialThanksTextEdit(new QTextBrowser)
    , m_licenseTextEdit(new QTextBrowser)
    , m_3rdPartyLibsTextEdit(new QTextBrowser)
{
    this->setWindowTitle(tr("About"));

    const QStringList helpStr {
        QStringLiteral("<p>%1</p>").arg(tr("Launch application with image file path as argument to load the file.")),
        QStringLiteral("<p>%1</p>").arg(tr("Drag and drop image file onto the window is also supported.")),
        QStringLiteral("<p>%1</p>").arg(tr("None of the operations in this application will alter the pictures on disk.")),
        QStringLiteral("<p>%1</p>").arg(tr("Context menu option explanation:")),
        QStringLiteral("<ul>"),
        // blumia: Chain two arg() here since it seems lupdate will remove one of them if we use
        //         the old `arg(QCoreApp::translate(), tr())` way, but it's worth to mention
        //         `arg(QCoreApp::translate(), this->tr())` works, but lupdate will complain about the usage.
        QStringLiteral("<li><b>%1</b>:<br/>%2</li>")
                .arg(QCoreApplication::translate("MainWindow", "Stay on top"))
                .arg(tr("Make window stay on top of all other windows.")),
        QStringLiteral("<li><b>%1</b>:<br/>%2</li>")
                .arg(QCoreApplication::translate("MainWindow", "Protected mode"))
                .arg(tr("Avoid close window accidentally. (eg. by double clicking the window)")),
        QStringLiteral("<li><b>%1</b>:<br/>%2</li>")
                .arg(QCoreApplication::translate("MainWindow", "Keep transformation", "The 'transformation' means the flip/rotation status that currently applied to the image view"))
                .arg(tr("Avoid resetting the zoom/rotation/flip state that was applied to the image view when switching between images.")),
        QStringLiteral("</ul>")
    };

    const QStringList aboutStr {
        QStringLiteral("<center><img width='128' height='128' src=':/icons/app-icon.svg'/><br/>"),
        qApp->applicationDisplayName(),
        (QStringLiteral("<br/>") + tr("Version: %1").arg(
#ifdef GIT_DESCRIBE_VERSION_STRING
            GIT_DESCRIBE_VERSION_STRING
#else
            qApp->applicationVersion()
#endif // GIT_DESCRIBE_VERSION_STRING
        )),
        QStringLiteral("<hr/>"),
        tr("Copyright (c) %1 %2", "%1 is year, %2 is the name of copyright holder(s)")
            .arg(QStringLiteral("2025"), QStringLiteral("<a href='https://github.com/BLumia'>@BLumia</a>")),
        QStringLiteral("<br/>"),
        tr("Logo designed by %1").arg(QStringLiteral("<a href='https://github.com/Lovelyblack'>@Lovelyblack</a>")),
        QStringLiteral("<hr/>"),
        tr("Built with Qt %1 (%2)").arg(QT_VERSION_STR, QSysInfo::buildCpuArchitecture()),
        QStringLiteral("<br/><a href='%1'>%2</a>").arg("https://github.com/BLumia/pineapple-pictures", tr("Source code")),
        QStringLiteral("</center>")
    };

    QFile translaterHtml(":/plain/translators.html");
    bool canOpenFile = translaterHtml.open(QIODevice::ReadOnly);
    const QByteArray & translatorList = canOpenFile ? translaterHtml.readAll() : QByteArrayLiteral("");

    const QStringList specialThanksStr {
        QStringLiteral("<h1 align='center'>%1</h1><a href='%2'>%3</a><p>%4</p>").arg(
            tr("Contributors"),
            QStringLiteral("https://github.com/BLumia/pineapple-pictures/graphs/contributors"),
            tr("List of contributors on GitHub"),
            tr("Thanks to all people who contributed to this project.")
        ),

        QStringLiteral("<h1 align='center'>%1</h1><p>%2</p>%3").arg(
            tr("Translators"),
            tr("I would like to thank the following people who volunteered to translate this application."),
            translatorList
        )
    };

    const QStringList licenseStr {
        QStringLiteral("<h1 align='center'><b>%1</b></h1>").arg(tr("Your Rights")),
        QStringLiteral("<p>%1</p><p>%2</p><ul><li>%3</li><li>%4</li><li>%5</li><li>%6</li></ul>").arg(
            tr("%1 is released under the MIT License."), // %1
            tr("This license grants people a number of freedoms:"), // %2
            tr("You are free to use %1, for any purpose"), // %3
            tr("You are free to distribute %1"), // %4
            tr("You can study how %1 works and change it"), // %5
            tr("You can distribute changed versions of %1") // %6
        ).arg(QStringLiteral("<i>%1</i>")),
        QStringLiteral("<p>%1</p>").arg(tr("The MIT license guarantees you this freedom. Nobody is ever permitted to take it away.")),
        QStringLiteral("<hr/><pre>%2</pre>")
    };

    const QString mitLicense(QStringLiteral(R"(Expat/MIT License

Copyright (c) 2025 BLumia

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

    const QStringList thirdPartyLibsStr {
        QStringLiteral("<h1 align='center'><b>%1</b></h1>").arg(tr("Third-party Libraries used by %1")),
        tr("%1 is built on the following free software libraries:", "Free as in freedom"),
        QStringLiteral("<ul>"),
#ifdef HAVE_EXIV2_VERSION
        QStringLiteral("<li><a href='%1'>%2</a>: %3</li>").arg("https://www.exiv2.org/", "Exiv2", "GPLv2"),
#endif // EXIV2_VERSION
        QStringLiteral("<li><a href='%1'>%2</a>: %3</li>").arg("https://www.qt.io/", "Qt", "GPLv2 + GPLv3 + LGPLv2.1 + LGPLv3"),
        QStringLiteral("</ul>")
    };

    m_helpTextEdit->setText(helpStr.join('\n'));

    m_aboutTextEdit->setText(aboutStr.join('\n'));
    m_aboutTextEdit->setOpenExternalLinks(true);

    m_specialThanksTextEdit->setText(specialThanksStr.join('\n'));
    m_specialThanksTextEdit->setOpenExternalLinks(true);

    m_licenseTextEdit->setText(licenseStr.join('\n').arg(qApp->applicationDisplayName(), mitLicense));

    m_3rdPartyLibsTextEdit->setText(thirdPartyLibsStr.join('\n').arg(QStringLiteral("<i>%1</i>").arg(qApp->applicationDisplayName())));
    m_3rdPartyLibsTextEdit->setOpenExternalLinks(true);

    m_tabWidget->addTab(m_helpTextEdit, tr("&Help"));
    m_tabWidget->addTab(m_aboutTextEdit, tr("&About"));
    m_tabWidget->addTab(m_specialThanksTextEdit, tr("&Special Thanks"));
    m_tabWidget->addTab(m_licenseTextEdit, tr("&License"));
    m_tabWidget->addTab(m_3rdPartyLibsTextEdit, tr("&Third-party Libraries"));

    m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
    connect(m_buttonBox, QOverload<QAbstractButton *>::of(&QDialogButtonBox::clicked), this, [this](){
        this->close();
    });

    setLayout(new QVBoxLayout);

    layout()->addWidget(m_tabWidget);
    layout()->addWidget(m_buttonBox);

    setMinimumSize(361, 161); // not sure why it complain "Unable to set geometry"
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

AboutDialog::~AboutDialog()
{

}

QSize AboutDialog::sizeHint() const
{
    return QSize(520, 350);
}
