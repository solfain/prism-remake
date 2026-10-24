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

#include <QToolButton>

class QAction;

/**
 * A full-width navigation button for the main window's sidebar.
 *
 * It mirrors a QAction (text, icon, tooltip, enabled / checked state and menu) the way a QToolButton with a default
 * action does, and additionally follows the action's visibility, which a plain QToolButton does not. All the logic
 * stays with the action: clicking the button simply triggers it.
 *
 * It exists because QToolBar sizes the buttons it creates for its actions to their preferred width and centres them,
 * while a sidebar wants every entry to span the full width with left-aligned content.
 */
class SidebarButton : public QToolButton {
    Q_OBJECT

   public:
    explicit SidebarButton(QAction* action, QWidget* parent = nullptr);
    ~SidebarButton() override = default;

   private slots:
    void syncWithAction();

   private:
    QAction* m_action;
};
