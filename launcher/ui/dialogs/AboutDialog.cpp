// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "AboutDialog.h"
#include <QIcon>
#include "Application.h"
#include "BuildConfig.h"
#include "Markdown.h"
#include "StringUtils.h"
#include "ui_AboutDialog.h"

#include <net/NetJob.h>

namespace {
QString getCreditsHtml()
{
    QFile dataFile(":/documents/credits.html");
    if (!dataFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file" << dataFile.fileName() << "for reading:" << dataFile.errorString();
        return {};
    }
    QString fileContent = QString::fromUtf8(dataFile.readAll());
    dataFile.close();

    // The developer list below the first heading is Prism Launcher's: SOL is based on their work and must not be
    // presented as the author of it, so it gets its own heading.
    return fileContent.arg(QObject::tr("%1 Contributors").arg(BuildConfig.LAUNCHER_DISPLAYNAME), QObject::tr("Prism Launcher Developers"),
                           QObject::tr("MultiMC Developers"), QObject::tr("With special thanks to"), QObject::tr("Icons"),
                           QObject::tr("Line icons by %1, used under the ISC licence.").arg(QStringLiteral("<a href=\"https://lucide.dev\">Lucide</a>")));
}

QString getLicenseHtml()
{
    QFile dataFile(":/documents/COPYING.md");
    if (dataFile.open(QIODevice::ReadOnly)) {
        QString output = markdownToHTML(dataFile.readAll());
        dataFile.close();
        return output;
    } else {
        qWarning() << "Failed to open file" << dataFile.fileName() << "for reading:" << dataFile.errorString();
        return QString();
    }
}

}  // namespace

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString launcherName = BuildConfig.LAUNCHER_DISPLAYNAME;

    setWindowTitle(tr("About %1").arg(launcherName));

    QString chtml = getCreditsHtml();
    ui->creditsText->setHtml(StringUtils::htmlListPatch(chtml));

    QString lhtml = getLicenseHtml();
    ui->licenseText->setHtml(StringUtils::htmlListPatch(lhtml));

    ui->urlLabel->setOpenExternalLinks(true);
    ui->aboutLabel->setOpenExternalLinks(true);
    ui->aboutLabel->setText(
        tr("<html><head/><body><p>A custom launcher that makes managing Minecraft easier by allowing you to have multiple instances of "
           "Minecraft at once.</p><p>%1 is an independent project based on the free software "
           "<a href=\"https://prismlauncher.org\">Prism Launcher</a>. It is not Prism Launcher and is not endorsed by or affiliated "
           "with the Prism Launcher project.</p></body></html>")
            .arg(launcherName));

    ui->icon->setPixmap(APPLICATION->logo().pixmap(64));
    ui->title->setText(launcherName);

    ui->versionLabel->setText(BuildConfig.printableVersionString());

    if (!BuildConfig.BUILD_PLATFORM.isEmpty())
        ui->platformLabel->setText(tr("Platform") + ": " + BuildConfig.BUILD_PLATFORM);
    else
        ui->platformLabel->setVisible(false);

    if (!BuildConfig.GIT_COMMIT.isEmpty())
        ui->commitLabel->setText(tr("Commit: %1").arg(BuildConfig.GIT_COMMIT));
    else
        ui->commitLabel->setVisible(false);

    if (!BuildConfig.BUILD_DATE.isEmpty())
        ui->buildDateLabel->setText(tr("Build date: %1").arg(BuildConfig.BUILD_DATE));
    else
        ui->buildDateLabel->setVisible(false);

    if (!BuildConfig.VERSION_CHANNEL.isEmpty())
        ui->channelLabel->setText(tr("Channel") + ": " + BuildConfig.VERSION_CHANNEL);
    else
        ui->channelLabel->setVisible(false);

    QString urlText("<html><head/><body><p><a href=\"%1\">%1</a></p></body></html>");
    ui->urlLabel->setText(urlText.arg(BuildConfig.LAUNCHER_GIT));

    ui->copyLabel->setText(BuildConfig.LAUNCHER_COPYRIGHT);

    connect(ui->closeButton, &QPushButton::clicked, this, &AboutDialog::close);
    // Enter should close this dialog, not open "About Qt": the first button of a dialog is its default one unless told otherwise
    ui->aboutQt->setAutoDefault(false);
    ui->closeButton->setDefault(true);

    connect(ui->aboutQt, &QPushButton::clicked, APPLICATION, &QApplication::aboutQt);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
