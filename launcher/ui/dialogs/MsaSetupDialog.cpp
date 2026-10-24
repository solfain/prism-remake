// SPDX-License-Identifier: GPL-3.0-only
/*
 *  SOL Launcher - Minecraft Launcher
 *  Copyright (C) 2026 SOL Launcher Contributors
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
 */
#include "MsaSetupDialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGuiApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QStyle>
#include <QVBoxLayout>

#include "Application.h"
#include "BuildConfig.h"
#include "DesktopServices.h"
#include "settings/SettingsObject.h"

namespace {

const QRegularExpression& clientIdPattern()
{
    static const QRegularExpression pattern(
        QRegularExpression::anchoredPattern("[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}"));
    return pattern;
}

QLabel* makeLabel(const QString& html, QWidget* parent, const char* role = nullptr)
{
    auto* label = new QLabel(html, parent);
    label->setTextFormat(Qt::RichText);
    label->setWordWrap(true);
    label->setOpenExternalLinks(false);
    if (role) {
        label->setProperty("solRole", role);
    }
    return label;
}

QLabel* makeBadge(int number, QWidget* parent)
{
    auto* badge = new QLabel(QString::number(number), parent);
    badge->setProperty("solRole", "stepBadge");
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedSize(24, 24);
    return badge;
}

//! a read-only, selectable value with a button that copies it
QWidget* makeCopyRow(const QString& value, QWidget* parent)
{
    auto* row = new QWidget(parent);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* field = new QLineEdit(value, row);
    field->setReadOnly(true);
    field->setProperty("solRole", "code");
    field->setCursorPosition(0);
    layout->addWidget(field, 1);

    auto* copy = new QPushButton(MsaSetupDialog::tr("Copy"), row);
    QObject::connect(copy, &QPushButton::clicked, row, [value, copy] {
        QGuiApplication::clipboard()->setText(value);
        copy->setText(MsaSetupDialog::tr("Copied"));
    });
    layout->addWidget(copy);
    return row;
}

}  // namespace

QString MsaSetupDialog::normalizeClientId(const QString& text)
{
    QString id = text.trimmed();
    // people paste the ID from tables, JSON and shell snippets
    static const QRegularExpression wrapping(QStringLiteral(R"(^[\s"'{(<]+|[\s"'})>,;]+$)"));
    id.remove(wrapping);
    return id.toLower();
}

bool MsaSetupDialog::isValidClientId(const QString& text)
{
    return clientIdPattern().match(normalizeClientId(text)).hasMatch();
}

QStringList MsaSetupDialog::redirectUris()
{
    // keep in step with MSAStep: the loopback handler (any port on http://localhost) and the OOB handler's callback
    return { QStringLiteral("http://localhost"), BuildConfig.LAUNCHER_APP_BINARY_NAME + QStringLiteral("://oauth/microsoft") };
}

QUrl MsaSetupDialog::registrationUrl()
{
    return QUrl(QStringLiteral("https://entra.microsoft.com/#view/Microsoft_AAD_RegisteredApps/ApplicationsListBlade"));
}

QUrl MsaSetupDialog::approvalUrl()
{
    return QUrl(QStringLiteral("https://aka.ms/AppRegInfo"));
}

MsaSetupDialog::MsaSetupDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Set up Microsoft sign-in"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 22, 24, 18);
    root->setSpacing(14);

    auto* title = makeLabel(tr("Set up Microsoft sign-in"), this, "heading");
    root->addWidget(title);
    root->addWidget(makeLabel(tr("%1 signs you in with your Microsoft account using OAuth 2.0. Microsoft wants every app to have its own "
                                 "application ID, so you register one once. It is free and takes about five minutes. Builds that ship "
                                 "with an ID never show this window.")
                                  .arg(BuildConfig.LAUNCHER_DISPLAYNAME),
                              this, "hint"));

    auto* steps = new QGridLayout();
    steps->setHorizontalSpacing(12);
    steps->setVerticalSpacing(14);
    steps->setColumnStretch(1, 1);
    int row = 0;

    // 1 -----------------------------------------------------------------------------------------------------------
    steps->addWidget(makeBadge(1, this), row, 0, Qt::AlignTop);
    steps->addWidget(makeLabel(tr("Open the Microsoft Entra <b>App registrations</b> page and choose <b>New registration</b>."), this), row, 1);
    auto* open = new QPushButton(tr("Open page"), this);
    connect(open, &QPushButton::clicked, this, [] { DesktopServices::openUrl(registrationUrl()); });
    steps->addWidget(open, row, 2, Qt::AlignTop);
    ++row;

    // 2 -----------------------------------------------------------------------------------------------------------
    steps->addWidget(makeBadge(2, this), row, 0, Qt::AlignTop);
    auto* second = new QWidget(this);
    auto* secondLayout = new QVBoxLayout(second);
    secondLayout->setContentsMargins(0, 0, 0, 0);
    secondLayout->setSpacing(8);
    secondLayout->addWidget(makeLabel(tr("Name it <b>%1</b>. Under <b>Supported account types</b> pick <b>Personal Microsoft accounts only</b>. "
                                         "As the <b>Redirect URI</b> choose the platform <b>Public client/native (mobile &amp; desktop)</b> "
                                         "and enter:")
                                          .arg(BuildConfig.LAUNCHER_DISPLAYNAME.toHtmlEscaped()),
                                      second));
    secondLayout->addWidget(makeCopyRow(redirectUris().at(0), second));
    steps->addWidget(second, row, 1, 1, 2);
    ++row;

    // 3 -----------------------------------------------------------------------------------------------------------
    steps->addWidget(makeBadge(3, this), row, 0, Qt::AlignTop);
    auto* third = new QWidget(this);
    auto* thirdLayout = new QVBoxLayout(third);
    thirdLayout->setContentsMargins(0, 0, 0, 0);
    thirdLayout->setSpacing(8);
    thirdLayout->addWidget(makeLabel(tr("Open <b>Authentication</b> and add a second redirect URI, then switch <b>Allow public client flows</b> "
                                        "to <b>Yes</b> and save:"),
                                     third));
    thirdLayout->addWidget(makeCopyRow(redirectUris().at(1), third));
    steps->addWidget(third, row, 1, 1, 2);
    ++row;

    // 4 -----------------------------------------------------------------------------------------------------------
    steps->addWidget(makeBadge(4, this), row, 0, Qt::AlignTop);
    auto* fourth = new QWidget(this);
    auto* fourthLayout = new QVBoxLayout(fourth);
    fourthLayout->setContentsMargins(0, 0, 0, 0);
    fourthLayout->setSpacing(8);
    fourthLayout->addWidget(makeLabel(tr("Copy the <b>Application (client) ID</b> from the app's Overview page and paste it here:"), fourth));
    m_clientId = new QLineEdit(fourth);
    m_clientId->setProperty("solRole", "code");
    m_clientId->setPlaceholderText(QStringLiteral("00000000-0000-4000-8000-000000000000"));
    m_clientId->setClearButtonEnabled(true);
    fourthLayout->addWidget(m_clientId);
    m_state = makeLabel(QString(), fourth, "hint");
    fourthLayout->addWidget(m_state);
    steps->addWidget(fourth, row, 1, 1, 2);
    ++row;

    root->addLayout(steps);

    // the one thing nobody can do for you -------------------------------------------------------------------------
    auto* note = new QWidget(this);
    note->setProperty("solRole", "callout");
    auto* noteLayout = new QHBoxLayout(note);
    noteLayout->setContentsMargins(14, 12, 14, 12);
    noteLayout->setSpacing(12);
    noteLayout->addWidget(makeLabel(tr("<b>Minecraft access.</b> Mojang has to approve an app before Minecraft accepts its sign-ins. Until then "
                                       "sign-in completes at Microsoft but stops with <i>Invalid app registration</i>."),
                                    note),
                          1);
    auto* approval = new QPushButton(tr("Request approval"), note);
    connect(approval, &QPushButton::clicked, this, [] { DesktopServices::openUrl(approvalUrl()); });
    noteLayout->addWidget(approval, 0, Qt::AlignVCenter);
    root->addWidget(note);

    auto* buttons = new QDialogButtonBox(this);
    m_save = buttons->addButton(tr("Save and sign in"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    m_save->setDefault(true);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);

    connect(m_clientId, &QLineEdit::textChanged, this, &MsaSetupDialog::updateState);
    updateState();

    // Word-wrapped labels only know how tall they are once the width is final, and a top-level layout does not redo that for
    // the window by itself (the rows used to come out squashed). So fix the width and size the window to what the text needs.
    constexpr int dialogWidth = 680;
    setFixedWidth(dialogWidth);
    ensurePolished();
    setFixedHeight(layout()->totalHeightForWidth(dialogWidth));
}

QString MsaSetupDialog::clientId() const
{
    const QString id = normalizeClientId(m_clientId->text());
    return clientIdPattern().match(id).hasMatch() ? id : QString();
}

void MsaSetupDialog::updateState()
{
    const QString raw = m_clientId->text().trimmed();
    const bool valid = isValidClientId(raw);
    m_save->setEnabled(valid);

    // an empty field is not an error yet
    const char* role = raw.isEmpty() ? "hint" : (valid ? "ok" : "error");
    m_state->setProperty("solRole", role);
    m_state->setText(raw.isEmpty() ? tr("It looks like <code>00000000-0000-4000-8000-000000000000</code>.")
                                   : (valid ? tr("That looks like a valid application ID.")
                                            : tr("That is not an application ID. It has five groups of hex digits, like 00000000-0000-4000-8000-000000000000.")));
    // re-evaluate the stylesheet for the new role
    m_state->style()->unpolish(m_state);
    m_state->style()->polish(m_state);
}

bool MsaSetupDialog::ensureClientId(QWidget* parent)
{
    if (!APPLICATION->getMSAClientID().isEmpty()) {
        return true;
    }

    MsaSetupDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted || dialog.clientId().isEmpty()) {
        return false;
    }
    APPLICATION->settings()->set("MSAClientIDOverride", dialog.clientId());
    APPLICATION->updateCapabilities();
    return !APPLICATION->getMSAClientID().isEmpty();
}
