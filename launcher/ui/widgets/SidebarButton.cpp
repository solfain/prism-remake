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
#include "SidebarButton.h"

#include <QAction>
#include <QMenu>
#include <QSize>
#include <QSizePolicy>

SidebarButton::SidebarButton(QAction* action, QWidget* parent) : QToolButton(parent), m_action(action)
{
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setIconSize(QSize(18, 18));
    // like the buttons QToolBar creates: never take keyboard focus away from the instance view
    setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);

    setDefaultAction(action);
    connect(action, &QAction::changed, this, &SidebarButton::syncWithAction);
    syncWithAction();
}

void SidebarButton::syncWithAction()
{
    setHidden(!m_action->isVisible());

    // entries with a menu open it right away instead of waiting for a long press
    if (m_action->menu() != nullptr) {
        setPopupMode(QToolButton::InstantPopup);
    }
}
