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
#pragma once

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QUrl>

class QLabel;
class QLineEdit;
class QPushButton;

/*!
 * Microsoft sign-in is an OAuth 2.0 flow that needs an application (client) ID registered by whoever distributes the
 * launcher. Builds that ship with an ID never show this dialog. For every other build (a fresh clone, a fork, a portable
 * copy) it replaces the old dead end, where "Add Microsoft account" simply was not there, with a short guided setup:
 * what to register at Microsoft, the exact redirect URIs to enter, and a field for the resulting ID.
 */
class MsaSetupDialog : public QDialog {
    Q_OBJECT

   public:
    explicit MsaSetupDialog(QWidget* parent = nullptr);

    //! The entered application ID in its canonical form, or an empty string while the input is not a valid ID.
    QString clientId() const;

    //! Trims whitespace, quotes and braces around a pasted ID and lower-cases it.
    static QString normalizeClientId(const QString& text);
    //! Azure application IDs are version 4 UUIDs (the same shape Settings > APIs accepts).
    static bool isValidClientId(const QString& text);
    //! The redirect URIs the application registration has to contain: the loopback address and the launcher's own scheme.
    static QStringList redirectUris();
    static QUrl registrationUrl();
    static QUrl approvalUrl();

    /*!
     * Makes sure Microsoft sign-in can start: returns true at once when a client ID is configured, otherwise shows this
     * dialog and, if it is accepted, stores the ID (Settings > APIs shows and can change it) and returns true.
     */
    static bool ensureClientId(QWidget* parent);

   private slots:
    void updateState();

   private:
    QLineEdit* m_clientId = nullptr;
    QLabel* m_state = nullptr;
    QPushButton* m_save = nullptr;
};
