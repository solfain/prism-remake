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
#include "SolTheme.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

#include "rainbow.h"
#include "ui/themes/ThemeManager.h"

namespace {

QByteArray readResource(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        themeWarningLog() << "Couldn't read resource" << path << ":" << file.errorString();
        return {};
    }
    return file.readAll();
}

QString variantKey(SolTheme::Variant variant)
{
    return variant == SolTheme::Variant::Dark ? QStringLiteral("dark") : QStringLiteral("light");
}

}  // namespace

SolTheme::SolTheme(Variant variant) : m_variant(variant)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(readResource(QStringLiteral(":/sol/tokens.json")), &parseError);
    if (parseError.error == QJsonParseError::NoError && document.isObject()) {
        const auto tokens = document.object().value(variantKey(variant)).toObject();
        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            m_tokens.insert(it.key(), it.value().toString());
        }
    } else {
        themeWarningLog() << "Couldn't parse the SOL colour tokens:" << parseError.errorString();
    }

    if (m_tokens.isEmpty()) {
        // Never leave the application with an unreadable palette: fall back to whatever Qt is currently using.
        m_palette = QApplication::palette();
        return;
    }

    QPalette palette;
    palette.setColor(QPalette::Window, token("bg"));
    palette.setColor(QPalette::WindowText, token("ink"));
    palette.setColor(QPalette::Base, token("surface"));
    palette.setColor(QPalette::AlternateBase, token("surfaceAlt"));
    palette.setColor(QPalette::ToolTipBase, token("tooltipBg"));
    palette.setColor(QPalette::ToolTipText, token("tooltipText"));
    palette.setColor(QPalette::Text, token("ink"));
    palette.setColor(QPalette::PlaceholderText, token("inkFaint"));
    palette.setColor(QPalette::Button, token("surface"));
    palette.setColor(QPalette::ButtonText, token("ink"));
    palette.setColor(QPalette::BrightText, token("danger"));
    palette.setColor(QPalette::Link, token("accent"));
    palette.setColor(QPalette::LinkVisited, Rainbow::mix(token("accent"), token("ink"), 0.35));
    palette.setColor(QPalette::Highlight, token("accent"));
    palette.setColor(QPalette::HighlightedText, token("onAccent"));
    // The 3D roles are not used by the flat stylesheet, but custom painted widgets (instance grid, sidebar) read them
    // as "subtle surface" (Midlight), "hairline" (Mid) and "control outline" (Dark).
    palette.setColor(QPalette::Light, token("surface"));
    palette.setColor(QPalette::Midlight, token("surfaceAlt"));
    palette.setColor(QPalette::Mid, token("border"));
    palette.setColor(QPalette::Dark, token("controlBorder"));
    palette.setColor(QPalette::Shadow, token("ink"));
    m_palette = fadeInactive(palette, fadeAmount(), fadeColor());

    auto sheet = QString::fromUtf8(readResource(QStringLiteral(":/sol/sol.qss.in")));
    for (auto it = m_tokens.cbegin(); it != m_tokens.cend(); ++it) {
        sheet.replace(QStringLiteral("@%1@").arg(it.key()), it.value());
    }
    m_styleSheet = sheet;
}

QString SolTheme::idFor(Variant variant)
{
    return QStringLiteral("sol-") + variantKey(variant);
}

bool SolTheme::isSolThemeId(const QString& id)
{
    return id.startsWith(QStringLiteral("sol-"));
}

QString SolTheme::id()
{
    return idFor(m_variant);
}

QString SolTheme::name()
{
    return m_variant == Variant::Dark ? QObject::tr("SOL Dark") : QObject::tr("SOL Light");
}

QString SolTheme::tooltip()
{
    return m_variant == Variant::Dark ? QObject::tr("SOL Launcher's dark theme: violet-tinted blacks with a soft lavender accent")
                                      : QObject::tr("SOL Launcher's light theme: lavender-tinted whites with a muted violet accent");
}

bool SolTheme::hasStyleSheet()
{
    return true;
}

QString SolTheme::appStyleSheet()
{
    return m_styleSheet;
}

QPalette SolTheme::colorScheme()
{
    return m_palette;
}

double SolTheme::fadeAmount()
{
    return 0.5;
}

QColor SolTheme::fadeColor()
{
    return token("bg");
}

LogColors SolTheme::logColorScheme()
{
    if (m_tokens.isEmpty()) {
        return defaultLogColors(m_palette);
    }

    LogColors result;
    // fatal errors keep the classic "white on black bar" treatment, tinted to match the theme
    result.background[MessageLevel::Fatal] = QColor(0x18, 0x16, 0x1C);
    result.foreground[MessageLevel::Launcher] = token("accent");
    result.foreground[MessageLevel::Debug] = token("success");
    result.foreground[MessageLevel::Warning] = token("warning");
    result.foreground[MessageLevel::Error] = token("danger");
    result.foreground[MessageLevel::Fatal] = QColor(0xE6, 0x84, 0x85);
    return result;
}

QColor SolTheme::token(const QString& key) const
{
    return QColor(m_tokens.value(key));
}
