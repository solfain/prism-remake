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

#include "InstanceDelegate.h"
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QPainter>
#include <QTextLayout>
#include <QTextOption>
#include <QtMath>

#include <QIcon>
#include <QTextEdit>
#include "BaseInstance.h"
#include "InstanceList.h"
#include "rainbow.h"

namespace {

// Card geometry. The grid lays items out in cells of a fixed width (InstanceView::m_itemWidth, keep the two in sync) and
// takes the cell height from sizeHint(). Every card therefore has the same size, whatever the instance is called.
constexpr int CardCellWidth = 112;
constexpr int CardCornerRadius = 10;
constexpr int CardIconSide = 48;
constexpr int CardPaddingTop = 12;
constexpr int CardPaddingBottom = 10;
constexpr int CardPaddingSide = 8;
constexpr int CardTextGap = 8;
constexpr int CardNameLines = 2;
constexpr int CardBadgeSide = 16;
constexpr int CardBadgeInset = 6;

/// Wraps `text` at word boundaries (or anywhere, for very long words) into at most `maxLines` lines of `width` pixels.
/// If the text does not fit, the last line is elided.
QStringList wrapInstanceName(const QString& text, const QFont& font, int width, int maxLines)
{
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    QTextLayout layout(text, font);
    layout.setTextOption(textOption);

    QStringList lines;
    layout.beginLayout();
    for (;;) {
        QTextLine line = layout.createLine();
        if (!line.isValid()) {
            break;
        }
        line.setLineWidth(width);
        lines.append(text.mid(line.textStart(), line.textLength()));
    }
    layout.endLayout();

    if (lines.size() > maxLines) {
        QString remainder;
        for (qsizetype i = maxLines - 1; i < lines.size(); ++i) {
            remainder += lines.at(i);
        }
        lines = lines.mid(0, maxLines - 1);
        lines.append(QFontMetrics(font).elidedText(remainder.trimmed(), Qt::ElideRight, width));
    }
    for (QString& line : lines) {
        line = line.trimmed();
    }
    return lines;
}

}  // namespace

ListViewDelegate::ListViewDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void ListViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const bool selected = opt.state.testFlag(QStyle::State_Selected);
    const bool hovered = opt.state.testFlag(QStyle::State_MouseOver);
    const bool enabled = opt.state.testFlag(QStyle::State_Enabled);
    // The theme's palette, not the view's: a stylesheet that gives the view a background (SOL does, to show the page
    // colour) rewrites the Base / Window roles of the view's own palette.
    const QPalette palette = QApplication::palette();

    painter->save();
    painter->setClipRect(opt.rect);
    painter->setRenderHint(QPainter::Antialiasing, true);

    // The card: a flat surface with a hairline. Everything is derived from the palette, so it follows any theme.
    {
        const QColor surface = palette.color(QPalette::Base);
        const QColor accent = palette.color(QPalette::Highlight);
        QColor fill = surface;
        QColor outline = palette.color(QPalette::Mid);
        qreal outlineWidth = 1.0;
        if (selected) {
            fill = Rainbow::mix(surface, accent, 0.14);
            outline = accent;
            outlineWidth = 1.5;
        } else if (hovered) {
            outline = palette.color(QPalette::Dark);
        }
        painter->setBrush(fill);
        painter->setPen(QPen(outline, outlineWidth));
        painter->drawRoundedRect(QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5), CardCornerRadius, CardCornerRadius);
    }

    // icon mode and state. A selected item keeps its true colours (the card is tinted instead of the icon).
    const QIcon::Mode iconMode = enabled ? QIcon::Normal : QIcon::Disabled;
    const QIcon::State iconState = opt.state.testFlag(QStyle::State_Open) ? QIcon::On : QIcon::Off;

    // the icon
    const QRect iconRect(opt.rect.center().x() - CardIconSide / 2, opt.rect.top() + CardPaddingTop, CardIconSide, CardIconSide);
    opt.icon.paint(painter, iconRect, Qt::AlignCenter, iconMode, iconState);

    // the name: centred, at most two lines, elided
    {
        QPalette::ColorGroup colorGroup = enabled ? QPalette::Normal : QPalette::Disabled;
        if (colorGroup == QPalette::Normal && !opt.state.testFlag(QStyle::State_Active)) {
            colorGroup = QPalette::Inactive;
        }
        painter->setPen(palette.color(colorGroup, QPalette::Text));
        painter->setFont(opt.font);

        const int lineHeight = QFontMetrics(opt.font).lineSpacing();
        const int textWidth = opt.rect.width() - 2 * CardPaddingSide;
        const int textTop = iconRect.bottom() + 1 + CardTextGap;
        const QStringList lines = wrapInstanceName(index.data().toString(), opt.font, textWidth, CardNameLines);
        for (qsizetype i = 0; i < lines.size(); ++i) {
            const QRect lineRect(opt.rect.left() + CardPaddingSide, textTop + static_cast<int>(i) * lineHeight, textWidth, lineHeight);
            painter->drawText(lineRect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, lines.at(i));
        }
    }

    // FIXME: this really has no business of being here. Make generic.
    // status badge in the top right corner of the card
    auto* instance = static_cast<BaseInstance*>(index.data(InstanceList::InstancePointerRole).value<void*>());
    if (instance != nullptr) {
        QString badge;
        if (instance->isRunning()) {
            badge = QStringLiteral("status-running");
        } else if (instance->hasCrashed() || instance->hasVersionBroken()) {
            badge = QStringLiteral("status-bad");
        }
        if (!badge.isEmpty()) {
            // FIXME: inject this.
            const QRect badgeRect(opt.rect.right() - CardBadgeInset - CardBadgeSide + 1, opt.rect.top() + CardBadgeInset, CardBadgeSide,
                                  CardBadgeSide);
            QIcon::fromTheme(badge).paint(painter, badgeRect, Qt::AlignCenter, iconMode, iconState);
        }
    }

    painter->restore();
}

QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const int lineHeight = QFontMetrics(opt.font).lineSpacing();
    return { CardCellWidth, CardPaddingTop + CardIconSide + CardTextGap + CardNameLines * lineHeight + CardPaddingBottom };
}

class NoReturnTextEdit : public QTextEdit {
    Q_OBJECT
   public:
    explicit NoReturnTextEdit(QWidget* parent) : QTextEdit(parent)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    }
    bool event(QEvent* event) override
    {
        auto eventType = event->type();
        if (eventType == QEvent::KeyPress || eventType == QEvent::KeyRelease) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            auto key = keyEvent->key();
            if ((key == Qt::Key_Return || key == Qt::Key_Enter) && eventType == QEvent::KeyPress) {
                emit editingDone();
                return true;
            }
            if (key == Qt::Key_Tab) {
                return true;
            }
        }
        return QTextEdit::event(event);
    }
   signals:
    void editingDone();
};

void ListViewDelegate::updateEditorGeometry(QWidget* editor,
                                            const QStyleOptionViewItem& option,
                                            [[maybe_unused]] const QModelIndex& index) const
{
    // cover the name area of the card
    const int top = option.rect.top() + CardPaddingTop + CardIconSide + 2;
    editor->setGeometry(QRect(option.rect.left() + 4, top, option.rect.width() - 8, option.rect.bottom() - 4 - top));
}

void ListViewDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto text = index.data(Qt::EditRole).toString();
    QTextEdit* realEditor = qobject_cast<NoReturnTextEdit*>(editor);
    realEditor->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    realEditor->append(text);
    realEditor->selectAll();
    realEditor->document()->clearUndoRedoStacks();
}

void ListViewDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QTextEdit* realEditor = qobject_cast<NoReturnTextEdit*>(editor);
    QString text = realEditor->toPlainText();
    const auto before = model->data(index).toString();
    if (model->setData(index, text)) {
        emit textChanged(before, model->data(index).toString());
    }
}

QWidget* ListViewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, [[maybe_unused]] const QModelIndex& index) const
{
    auto editor = new NoReturnTextEdit(parent);
    // the editor sits inside the card, so it drops the roomy padding the theme gives regular text boxes
    editor->setStyleSheet(
        QStringLiteral("QTextEdit { padding: 1px 2px; border: 1px solid %1; border-radius: 6px; }").arg(option.palette.color(QPalette::Highlight).name()));
    connect(editor, &NoReturnTextEdit::editingDone, this, &ListViewDelegate::editingDone);
    return editor;
}

void ListViewDelegate::editingDone()
{
    NoReturnTextEdit* editor = qobject_cast<NoReturnTextEdit*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

#include "InstanceDelegate.moc"
