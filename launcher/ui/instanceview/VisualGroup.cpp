// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *  Copyright (C) 2023 Tayou <git@tayou.org>
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

#include "VisualGroup.h"

#include <QApplication>
#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QtMath>
#include <utility>

#include "InstanceView.h"
#include "rainbow.h"

VisualGroup::VisualGroup(QString text, InstanceView* view) : view(view), text(std::move(text)), collapsed(false) {}

VisualGroup::VisualGroup(const VisualGroup* other) : view(other->view), text(other->text), collapsed(other->collapsed) {}

void VisualGroup::update()
{
    auto temp_items = items();
    auto itemsPerRow = view->itemsPerRow();

    int numRows = qMax(1, qCeil((qreal)temp_items.size() / (qreal)itemsPerRow));
    rows = QList<VisualRow>(numRows);

    int maxRowHeight = 0;
    int positionInRow = 0;
    int currentRow = 0;
    int offsetFromTop = 0;
    for (auto item : temp_items) {
        if (positionInRow == itemsPerRow) {
            rows[currentRow].height = maxRowHeight;
            rows[currentRow].top = offsetFromTop;
            currentRow++;
            if (currentRow >= rows.size()) {
                currentRow = rows.size() - 1;
            }
            offsetFromTop += maxRowHeight + 5;
            positionInRow = 0;
            maxRowHeight = 0;
        }
        QStyleOptionViewItem viewItemOption;
        view->initViewItemOption(&viewItemOption);

        auto itemHeight = view->itemDelegate()->sizeHint(viewItemOption, item).height();
        if (itemHeight > maxRowHeight) {
            maxRowHeight = itemHeight;
        }
        rows[currentRow].items.append(item);
        positionInRow++;
    }
    rows[currentRow].height = maxRowHeight;
    rows[currentRow].top = offsetFromTop;
}

QPair<int, int> VisualGroup::positionOf(const QModelIndex& index) const
{
    int y = 0;
    for (auto& row : rows) {
        for (auto x = 0; x < row.items.size(); x++) {
            if (row.items[x] == index) {
                return qMakePair(x, y);
            }
        }
        y++;
    }
    qWarning() << "Item" << index.row() << index.data(Qt::DisplayRole).toString() << "not found in visual group" << text;
    return qMakePair(0, 0);
}

int VisualGroup::rowTopOf(const QModelIndex& index) const
{
    auto position = positionOf(index);
    return rows[position.second].top;
}

int VisualGroup::rowHeightOf(const QModelIndex& index) const
{
    auto position = positionOf(index);
    return rows[position.second].height;
}

VisualGroup::HitResults VisualGroup::hitScan(const QPoint& pos) const
{
    VisualGroup::HitResults results = VisualGroup::NoHit;
    int y_start = verticalPosition();
    int body_start = y_start + headerHeight();
    int body_end = body_start + contentHeight();
    int y = pos.y();
    // int x = pos.x();
    if (y < y_start) {
        results = VisualGroup::NoHit;
    } else if (y < body_start) {
        results = VisualGroup::HeaderHit;
        int collapseSize = headerHeight() - 4;

        // the icon
        QRect iconRect = QRect(view->m_leftMargin + 2, 2 + y_start, view->width() - 4, collapseSize);
        if (iconRect.contains(pos)) {
            results |= VisualGroup::CheckboxHit;
        }
    } else if (y < body_end) {
        results |= VisualGroup::BodyHit;
    }
    return results;
}

namespace {

/// Group headers use a slightly smaller, semi-bold version of the application font.
QFont groupHeaderFont()
{
    QFont font(QApplication::font());
    if (font.pointSizeF() > 1.0) {
        font.setPointSizeF(font.pointSizeF() - 0.5);
    }
    font.setWeight(QFont::DemiBold);
    return font;
}

}  // namespace

void VisualGroup::drawHeader(QPainter* painter, const QStyleOptionViewItem& option) const
{
    const QRect optRect = option.rect;
    // the theme's palette, see ListViewDelegate::paint (the view's own palette is rewritten by stylesheets)
    const QPalette palette = QApplication::palette();
    const QFont font = groupHeaderFont();
    const QFontMetrics fontMetrics(font);

    // everything is derived from the palette: a muted label, a pill with the number of instances and a hairline
    QColor muted = palette.color(QPalette::WindowText);
    muted.setAlphaF(0.72F);

    const int centerY = optRect.top() + headerHeight() / 2;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // BEGIN: chevron
    {
        const qreal chevronX = optRect.left() + 12;
        const qreal chevronY = centerY;
        painter->setPen(QPen(muted, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (collapsed) {
            const QPointF points[3] = { QPointF(chevronX - 1.5, chevronY - 4.0), QPointF(chevronX + 2.5, chevronY),
                                        QPointF(chevronX - 1.5, chevronY + 4.0) };
            painter->drawPolyline(points, 3);
        } else {
            const QPointF points[3] = { QPointF(chevronX - 4.0, chevronY - 2.0), QPointF(chevronX, chevronY + 2.0),
                                        QPointF(chevronX + 4.0, chevronY - 2.0) };
            painter->drawPolyline(points, 3);
        }
    }
    // END: chevron

    // BEGIN: group name
    const int textLeft = optRect.left() + 26;
    const QString name = text.isEmpty() ? QObject::tr("Ungrouped") : text;
    const QString shownName = fontMetrics.elidedText(name, Qt::ElideRight, qMax(40, optRect.width() - textLeft - 70));
    const int nameWidth = fontMetrics.horizontalAdvance(shownName);
    painter->setFont(font);
    painter->setPen(muted);
    painter->drawText(QRect(textLeft, optRect.top(), nameWidth + 2, headerHeight()), Qt::AlignLeft | Qt::AlignVCenter, shownName);
    // END: group name

    // BEGIN: number of instances in the group
    const QString countText = QString::number(items().size());
    const QRect pill(textLeft + nameWidth + 8, centerY - 9, qMax(20, fontMetrics.horizontalAdvance(countText) + 14), 18);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Rainbow::mix(palette.color(QPalette::Window), palette.color(QPalette::Highlight), 0.14));
    painter->drawRoundedRect(pill, 9, 9);
    painter->setPen(muted);
    painter->drawText(pill, Qt::AlignCenter, countText);
    // END: number of instances in the group

    // BEGIN: hairline
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(QPen(palette.color(QPalette::Mid), 1));
    painter->drawLine(pill.right() + 12, centerY, optRect.right() - 4, centerY);
    // END: hairline

    painter->restore();
}

int VisualGroup::totalHeight() const
{
    return headerHeight() + contentHeight();
}

int VisualGroup::headerHeight()
{
    return QFontMetrics(groupHeaderFont()).height() + 16;
}

int VisualGroup::contentHeight() const
{
    if (collapsed) {
        return 0;
    }
    auto last = rows[numRows() - 1];
    return last.top + last.height;
}

int VisualGroup::numRows() const
{
    return (int)rows.size();
}

int VisualGroup::verticalPosition() const
{
    return m_verticalPosition;
}

QList<QModelIndex> VisualGroup::items() const
{
    QList<QModelIndex> indices;
    for (int i = 0; i < view->model()->rowCount(); ++i) {
        const QModelIndex index = view->model()->index(i, 0);
        if (index.data(InstanceViewRoles::GroupRole).toString() == text) {
            indices.append(index);
        }
    }
    return indices;
}
