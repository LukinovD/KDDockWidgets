/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "DropIndicatorOverlayInterface_p.h"

#include "Frame_p.h"
#include "DropArea_p.h"
#include "FloatingWindow_p.h"

using namespace KDDockWidgets;

DropIndicatorOverlayInterface::DropIndicatorOverlayInterface(DropArea *dropArea)
    : QWidgetAdapter(dropArea)
    , m_dropArea(dropArea)
{
    setVisible(false);
    setObjectName(QStringLiteral("DropIndicatorOverlayInterface"));
}

void DropIndicatorOverlayInterface::setWindowBeingDragged(const FloatingWindow *window)
{
    if (window == m_windowBeingDragged)
        return;

    m_windowBeingDragged = window;
    if (m_windowBeingDragged) {
        setGeometry(m_dropArea->QWidgetAdapter::rect());
        raise();
    } else {
        setHoveredFrame(nullptr);
    }

    setVisible(m_windowBeingDragged != nullptr);
    updateVisibility();
}

QRect DropIndicatorOverlayInterface::hoveredFrameRect() const
{
    return m_hoveredFrameRect;
}

void DropIndicatorOverlayInterface::setHoveredFrame(Frame *frame)
{
    if (frame == m_hoveredFrame)
        return;

    if (m_hoveredFrame)
        disconnect(m_hoveredFrame, &QObject::destroyed, this, &DropIndicatorOverlayInterface::onFrameDestroyed);

    m_hoveredFrame = frame;
    if (m_hoveredFrame) {
        connect(frame, &QObject::destroyed, this, &DropIndicatorOverlayInterface::onFrameDestroyed);
        setHoveredFrameRect(m_hoveredFrame->QWidgetAdapter::geometry());
    } else {
        setHoveredFrameRect(QRect());
    }

    updateVisibility();
    Q_EMIT hoveredFrameChanged(m_hoveredFrame);
    onHoveredFrameChanged(m_hoveredFrame);
}

bool DropIndicatorOverlayInterface::isHovered() const
{
    return m_windowBeingDragged != nullptr;
}

DropIndicatorOverlayInterface::DropLocation DropIndicatorOverlayInterface::currentDropLocation() const
{
    return m_currentDropLocation;
}

KDDockWidgets::Location DropIndicatorOverlayInterface::multisplitterLocationFor(DropIndicatorOverlayInterface::DropLocation dropLoc)
{
    switch (dropLoc) {
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_None:
        return KDDockWidgets::Location_None;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Left:
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterLeft:
        return KDDockWidgets::Location_OnLeft;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterTop:
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Top:
        return KDDockWidgets::Location_OnTop;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterRight:
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Right:
        return KDDockWidgets::Location_OnRight;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterBottom:
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Bottom:
        return KDDockWidgets::Location_OnBottom;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Center:
        return KDDockWidgets::Location_None;
    }

    return KDDockWidgets::Location_None;
}

void DropIndicatorOverlayInterface::onFrameDestroyed()
{
    setHoveredFrame(nullptr);
}

void DropIndicatorOverlayInterface::onHoveredFrameChanged(Frame *)
{
}

void DropIndicatorOverlayInterface::setCurrentDropLocation(DropIndicatorOverlayInterface::DropLocation location)
{
    if (m_currentDropLocation != location) {
        m_currentDropLocation = location;
        Q_EMIT currentDropLocationChanged();
    }
}

void DropIndicatorOverlayInterface::hover(QPoint globalPos)
{
    hover_impl(globalPos);
}

void DropIndicatorOverlayInterface::setHoveredFrameRect(QRect rect)
{
    if (m_hoveredFrameRect != rect) {
        m_hoveredFrameRect = rect;
        Q_EMIT hoveredFrameRectChanged();
    }
}
