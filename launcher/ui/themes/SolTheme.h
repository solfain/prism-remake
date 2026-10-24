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

#include <QMap>
#include <QString>

#include "ui/themes/FusionTheme.h"

/**
 * SOL Launcher's built-in widget themes ("SOL Light" and "SOL Dark").
 *
 * The colours live in :/sol/tokens.json and the stylesheet template in :/sol/sol.qss.in, so the look can be tuned
 * without touching C++. See docs/sol/DESIGN.md for the design rationale (flat, lavender-tinted surfaces with a
 * deliberately low-chroma accent).
 */
class SolTheme : public FusionTheme {
   public:
    enum class Variant { Light, Dark };

    explicit SolTheme(Variant variant);
    ~SolTheme() override = default;

    static QString idFor(Variant variant);
    static bool isSolThemeId(const QString& id);

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
    LogColors logColorScheme() override;
    QStyle* createStyle() override;

   private:
    QColor token(const QString& key) const;

    Variant m_variant;
    QMap<QString, QString> m_tokens;
    QString m_styleSheet;
    QPalette m_palette;
};
