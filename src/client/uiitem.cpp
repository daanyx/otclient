/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "uiitem.h"
#include <framework/graphics/fontmanager.h>

UIItem::UIItem() { m_draggable = true; }

void UIItem::drawSelf(DrawPoolType drawPane)
{
    if (drawPane != DrawPoolType::FOREGROUND)
        return;

    // draw style components in order
    if (m_backgroundColor.aF() > Fw::MIN_ALPHA) {
        Rect backgroundDestRect = m_rect;
        backgroundDestRect.expand(-m_borderWidth.top, -m_borderWidth.right, -m_borderWidth.bottom, -m_borderWidth.left);
        drawBackground(m_rect);
    }

    drawImage(m_rect);

    if (m_itemVisible && m_item) {
        const auto& drawRect = getPaddingRect();
        const int exactSize = std::max<int>(SPRITE_SIZE, m_item->getExactSize());
        if (exactSize == 0)
            return;

        const float scaleFactor = std::min<float>(drawRect.width() / static_cast<float>(exactSize), drawRect.height() / static_cast<float>(exactSize));

        Point dest = drawRect.bottomRight() + Point(1);
        dest += (m_item->getDisplacement() - Point(SPRITE_SIZE)) * scaleFactor;

        float oldScaleFactor = g_drawPool.getScaleFactor();
        g_drawPool.setScaleFactor(scaleFactor);
        m_item->setColor(m_color);
        m_item->draw(dest, Otc::DrawThings, TextureType::SMOOTH);
        g_drawPool.setScaleFactor(oldScaleFactor);

        if (m_font && (m_item->isStackable() || m_item->isChargeable()) && m_item->getCountOrSubType() > 1) {
            const auto& count = std::to_string(m_item->getCountOrSubType());
            m_font->drawText(count, Rect(m_rect.topLeft(), m_rect.bottomRight() - Point(3, 0)), Color(231, 231, 231), Fw::AlignBottomRight);
        }

#ifdef FRAMEWORK_EDITOR
        if (m_showId)
            m_font->drawText(std::to_string(m_item->getServerId()), m_rect, Fw::AlignBottomRight);
#endif
    }

    drawBorder(m_rect);
    drawIcon(m_rect);
    drawText(m_rect);
}

void UIItem::setItemId(int id)
{
    if (!m_item && id != 0)
        m_item = Item::create(id);
    else {
        // remove item
        if (id == 0)
            m_item = nullptr;
        else
            m_item->setId(id);
    }
}

void UIItem::onStyleApply(const std::string_view styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for (const OTMLNodePtr& node : styleNode->children()) {
        if (node->tag() == "item-id")
            setItemId(node->value<int>());
        else if (node->tag() == "item-count")
            setItemCount(node->value<int>());
        else if (node->tag() == "item-visible")
            setItemVisible(node->value<bool>());
        else if (node->tag() == "virtual")
            setVirtual(node->value<bool>());
        else if (node->tag() == "show-id")
            m_showId = node->value<bool>();
    }
}
