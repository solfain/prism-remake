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
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTest>

#include <algorithm>
#include <cmath>

#include "ui/themes/IconTheme.h"
#include "ui/themes/SolTheme.h"

namespace {

// WCAG 2.x relative luminance and contrast ratio
double channelLuminance(double channel)
{
    return channel <= 0.04045 ? channel / 12.92 : std::pow((channel + 0.055) / 1.055, 2.4);
}

double luminance(const QColor& color)
{
    return 0.2126 * channelLuminance(color.redF()) + 0.7152 * channelLuminance(color.greenF()) + 0.0722 * channelLuminance(color.blueF());
}

double contrast(const QColor& first, const QColor& second)
{
    const double a = luminance(first);
    const double b = luminance(second);
    return (std::max(a, b) + 0.05) / (std::min(a, b) + 0.05);
}

// Chroma of a colour in OKLCH (Bjorn Ottosson's OKLab). Unlike HSV saturation it is perceptual, so a near-black neutral such as #18161C
// reads as what it is (C = 0.012) instead of as "14 % saturated".
double oklchChroma(const QColor& color)
{
    const double r = channelLuminance(color.redF());
    const double g = channelLuminance(color.greenF());
    const double b = channelLuminance(color.blueF());
    const double l = std::cbrt(0.4122214708 * r + 0.5363325363 * g + 0.0514459929 * b);
    const double m = std::cbrt(0.2119034982 * r + 0.6806995451 * g + 0.1073969566 * b);
    const double s = std::cbrt(0.0883024619 * r + 0.2817188376 * g + 0.6299787005 * b);
    const double a = 1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s;
    const double bb = 0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s;
    return std::hypot(a, bb);
}

}  // namespace

class SolThemeTest : public QObject {
    Q_OBJECT

    QJsonObject tokens(const QString& variant) const
    {
        QFile file(":/sol/tokens.json");
        [&] { QVERIFY(file.open(QIODevice::ReadOnly)); }();
        return QJsonDocument::fromJson(file.readAll()).object().value(variant).toObject();
    }

    QColor token(const QString& variant, const QString& key) const { return QColor(tokens(variant).value(key).toString()); }

    static void addVariants()
    {
        QTest::addColumn<QString>("variant");
        QTest::newRow("light") << "light";
        QTest::newRow("dark") << "dark";
    }

   private slots:
    void initTestCase()
    {
        Q_INIT_RESOURCE(sol);
        Q_INIT_RESOURCE(sol_icons);
        Q_INIT_RESOURCE(flat);
    }

    void tokensAreComplete_data() { addVariants(); }
    void tokensAreComplete()
    {
        QFETCH(QString, variant);
        const QStringList required = { "bg",        "surface",     "surfaceAlt",       "sidebar",    "sidebarHover", "border",   "controlBorder",
                                       "ink",       "inkMuted",    "inkFaint",         "accent",     "accentHover",  "accentPress",
                                       "accentSoft", "accentSoftStrong", "accentTrack", "onAccent",   "success",      "danger",   "warning",
                                       "scroll",    "scrollHover", "tooltipBg",        "tooltipText", "variant" };
        const auto values = tokens(variant);
        for (const auto& key : required) {
            QVERIFY2(values.contains(key), qPrintable(QString("missing token '%1' in %2").arg(key, variant)));
            if (key != "variant") {
                QVERIFY2(QColor(values.value(key).toString()).isValid(), qPrintable(QString("invalid colour for '%1' in %2").arg(key, variant)));
            }
        }
        QCOMPARE(values.value("variant").toString(), variant);
    }

    void stylesheetHasNoUnresolvedPlaceholders_data() { addVariants(); }
    void stylesheetHasNoUnresolvedPlaceholders()
    {
        QFETCH(QString, variant);
        SolTheme theme(variant == "dark" ? SolTheme::Variant::Dark : SolTheme::Variant::Light);
        const QString sheet = theme.appStyleSheet();
        QVERIFY(!sheet.isEmpty());

        // strip comments, then no @token@ may remain
        QString withoutComments = sheet;
        withoutComments.remove(QRegularExpression("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption));
        const auto leftover = QRegularExpression("@[A-Za-z]+@").match(withoutComments);
        QVERIFY2(!leftover.hasMatch(), qPrintable("unresolved placeholder: " + leftover.captured()));
    }

    void stylesheetImagesExist_data() { addVariants(); }
    void stylesheetImagesExist()
    {
        QFETCH(QString, variant);
        SolTheme theme(variant == "dark" ? SolTheme::Variant::Dark : SolTheme::Variant::Light);
        const QString sheet = theme.appStyleSheet();

        int found = 0;
        auto it = QRegularExpression("url\\((:/sol/[^)]+)\\)").globalMatch(sheet);
        while (it.hasNext()) {
            const QString path = it.next().captured(1);
            QVERIFY2(path.startsWith(":/sol/" + variant + "/"), qPrintable("image of the wrong variant: " + path));
            QVERIFY2(QFile::exists(path), qPrintable("missing image resource: " + path));
            ++found;
        }
        QVERIFY(found > 0);
    }

    void paletteMatchesTokens_data() { addVariants(); }
    void paletteMatchesTokens()
    {
        QFETCH(QString, variant);
        SolTheme theme(variant == "dark" ? SolTheme::Variant::Dark : SolTheme::Variant::Light);
        const QPalette palette = theme.colorScheme();
        QCOMPARE(palette.color(QPalette::Active, QPalette::Window), token(variant, "bg"));
        QCOMPARE(palette.color(QPalette::Active, QPalette::Base), token(variant, "surface"));
        QCOMPARE(palette.color(QPalette::Active, QPalette::Text), token(variant, "ink"));
        QCOMPARE(palette.color(QPalette::Active, QPalette::Highlight), token(variant, "accent"));
        QCOMPARE(palette.color(QPalette::Active, QPalette::HighlightedText), token(variant, "onAccent"));
    }

    void textContrastMeetsWcagAa_data() { addVariants(); }
    void textContrastMeetsWcagAa()
    {
        QFETCH(QString, variant);
        const QList<QPair<QString, QString>> textPairs = {
            { "ink", "bg" },       { "ink", "surface" },        { "ink", "sidebar" },    { "inkMuted", "bg" },      { "inkMuted", "surface" },
            { "inkMuted", "sidebar" }, { "accent", "bg" },     { "accent", "surface" }, { "onAccent", "accent" },  { "onAccent", "accentHover" },
            { "ink", "accentSoft" }, { "ink", "accentSoftStrong" }, { "ink", "accentTrack" }, { "danger", "bg" },  { "success", "bg" },
            { "warning", "bg" },
        };
        for (const auto& [foreground, background] : textPairs) {
            const double ratio = contrast(token(variant, foreground), token(variant, background));
            QVERIFY2(ratio >= 4.5, qPrintable(QString("%1 on %2 (%3): %4:1").arg(foreground, background, variant).arg(ratio, 0, 'f', 2)));
        }
        // outlines of controls identify them, so they need 3:1 (WCAG 1.4.11)
        const double outline = contrast(token(variant, "controlBorder"), token(variant, "surface"));
        QVERIFY2(outline >= 3.0, qPrintable(QString("controlBorder on surface (%1): %2:1").arg(variant).arg(outline, 0, 'f', 2)));
    }

    void accentStaysDesaturated_data() { addVariants(); }
    void accentStaysDesaturated()
    {
        // The identity requirement: a muted violet, not a neon one. In OKLCH the SOL accents sit at C = 0.075 (light) and 0.084 (dark);
        // the saturated "default" violets and indigos are at 0.20 - 0.25 (Tailwind violet-600 #7C3AED: 0.247, indigo-500 #6366F1: 0.204).
        QFETCH(QString, variant);
        const double accent = oklchChroma(token(variant, "accent"));
        QVERIFY2(accent > 0.04, qPrintable(QString("accent has lost its hue in %1 (C = %2)").arg(variant).arg(accent, 0, 'f', 3)));
        QVERIFY2(accent <= 0.10, qPrintable(QString("accent too saturated in %1 (C = %2)").arg(variant).arg(accent, 0, 'f', 3)));

        // the large areas stay close to neutral: a whisper of violet, never a tint wash
        for (const char* key : { "bg", "surface", "sidebar" }) {
            const double chroma = oklchChroma(token(variant, key));
            QVERIFY2(chroma <= 0.02, qPrintable(QString("%1 too tinted in %2 (C = %3)").arg(key, variant).arg(chroma, 0, 'f', 3)));
        }
    }

    void iconThemesCoverTheFlatSet()
    {
        // Every icon of the "flat" theme needs a line-icon counterpart in both variants; the brand glyphs are the only
        // exceptions (they fall back to the flat theme, which the sol themes inherit from).
        const QStringList brandGlyphs = { "discord.svg", "reddit-alien.svg" };
        const QStringList flatIcons = QDir(":/icons/flat/scalable").entryList({ "*.svg" });
        QVERIFY(flatIcons.size() > 40);
        for (const QString& file : flatIcons) {
            if (brandGlyphs.contains(file)) {
                continue;
            }
            for (const QString& theme : { QString("sol"), QString("sol_white") }) {
                QVERIFY2(QFile::exists(QString(":/icons/%1/scalable/%2").arg(theme, file)), qPrintable(QString("%1 is missing in %2").arg(file, theme)));
            }
        }
    }

    void iconThemeMetadata()
    {
        IconTheme visible("sol", ":/icons/sol");
        QVERIFY(visible.load());
        QVERIFY(!visible.hidden());
        QCOMPARE(visible.name(), QString("SOL Line"));

        // the light-stroke half is only ever chosen automatically, so it stays out of the theme lists
        IconTheme internal("sol_white", ":/icons/sol_white");
        QVERIFY(internal.load());
        QVERIFY(internal.hidden());

        // a theme without the key is listed as before
        IconTheme flat("flat", ":/icons/flat");
        QVERIFY(flat.load());
        QVERIFY(!flat.hidden());
    }

    void iconStrokesComeFromTheTokens_data() { addVariants(); }
    void iconStrokesComeFromTheTokens()
    {
        QFETCH(QString, variant);
        const QString theme = variant == "dark" ? "sol_white" : "sol";
        const auto strokeOf = [&](const QString& icon) {
            QFile file(QString(":/icons/%1/scalable/%2.svg").arg(theme, icon));
            [&] { QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(file.fileName())); }();
            const auto match = QRegularExpression("stroke=\"(#[0-9A-Fa-f]{6})\"").match(QString::fromUtf8(file.readAll()));
            return match.hasMatch() ? QColor(match.captured(1)) : QColor();
        };
        // ordinary icons are muted ink; status icons carry their semantic colour; both must stay readable on the surface
        QCOMPARE(strokeOf("settings"), token(variant, "inkMuted"));
        QCOMPARE(strokeOf("status-bad"), token(variant, "danger"));
        QCOMPARE(strokeOf("status-good"), token(variant, "success"));
        QCOMPARE(strokeOf("status-yellow"), token(variant, "warning"));
        for (const QString& icon : { "settings", "new", "delete", "status-bad", "status-good", "status-yellow" }) {
            const double ratio = contrast(strokeOf(icon), token(variant, "surface"));
            QVERIFY2(ratio >= 3.0, qPrintable(QString("icon %1 (%2): %3:1 on surface").arg(icon, variant).arg(ratio, 0, 'f', 2)));
        }
    }

    void themeIdentity()
    {
        SolTheme light(SolTheme::Variant::Light);
        SolTheme dark(SolTheme::Variant::Dark);
        QCOMPARE(light.id(), QString("sol-light"));
        QCOMPARE(dark.id(), QString("sol-dark"));
        QCOMPARE(SolTheme::idFor(SolTheme::Variant::Light), light.id());
        QVERIFY(SolTheme::isSolThemeId(light.id()));
        QVERIFY(SolTheme::isSolThemeId(dark.id()));
        QVERIFY(!SolTheme::isSolThemeId("dark"));
        QVERIFY(!SolTheme::isSolThemeId("system"));
        QCOMPARE(light.qtTheme(), QString("Fusion"));
        QVERIFY(light.hasStyleSheet());
    }
};

QTEST_GUILESS_MAIN(SolThemeTest)

#include "SolTheme_test.moc"
