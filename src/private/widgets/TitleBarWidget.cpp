/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "TitleBarWidget_p.h"
#include "DragController_p.h"
#include "Frame_p.h"
#include "FloatingWindow_p.h"
#include "Logging_p.h"
#include "WindowBeingDragged_p.h"
#include "Utils_p.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

using namespace KDDockWidgets;

Button::~Button() {}

TitleBarWidget::TitleBarWidget(Frame *parent)
    : TitleBar(parent)
    , m_layout(new QHBoxLayout(this))
{
    init();
}

TitleBarWidget::TitleBarWidget(FloatingWindow *parent)
    : TitleBar(parent)
    , m_layout(new QHBoxLayout(this))
{
    init();
}

void TitleBarWidget::init()
{
    qCDebug(creation) << "TitleBarWidget" << this;
    m_dockWidgetIcon = new QLabel(this);
    m_layout->addWidget(m_dockWidgetIcon);

    m_layout->addStretch();
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(2);

    m_maximizeButton = TitleBarWidget::createButton(this, style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    m_minimizeButton = TitleBarWidget::createButton(this, style()->standardIcon(QStyle::SP_TitleBarMinButton));
    m_floatButton = TitleBarWidget::createButton(this, style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    m_closeButton = TitleBarWidget::createButton(this, style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_autoHideButton = TitleBarWidget::createButton(this, style()->standardIcon(QStyle::SP_TitleBarMinButton)); // TODO change icon

    m_layout->addWidget(m_autoHideButton);
    m_layout->addWidget(m_minimizeButton);
    m_layout->addWidget(m_maximizeButton);
    m_layout->addWidget(m_floatButton);
    m_layout->addWidget(m_closeButton);

    m_minimizeButton->setVisible(false);

    connect(m_floatButton, &QAbstractButton::clicked, this, &TitleBarWidget::onFloatClicked);
    connect(m_closeButton, &QAbstractButton::clicked, this, &TitleBarWidget::onCloseClicked);
    connect(m_maximizeButton, &QAbstractButton::clicked, this, &TitleBarWidget::onMaximizeClicked);
    connect(m_minimizeButton, &QAbstractButton::clicked, this, &TitleBarWidget::onMinimizeClicked);

    updateCloseButton();
    updateFloatButton();
    updateMaximizeButton();
    updateMinimizeButton();

    connect(this, &TitleBar::titleChanged, this, [this] {
        update();
    });

    connect(this, &TitleBar::iconChanged, this, [this] {
        if (icon().isNull()) {
            m_dockWidgetIcon->setPixmap(QPixmap());
        } else {
            const QPixmap pix = icon().pixmap(QSize(28,28));
            m_dockWidgetIcon->setPixmap(pix);
        }
        update();
    });
}

QRect TitleBarWidget::iconRect() const
{
    if (icon().isNull()) {
        return QRect(0,0, 0,0);
    } else {
        return QRect(3, 3, 30, 30);
    }
}

int TitleBarWidget::buttonAreaWidth() const
{
    if (m_floatButton->isVisible())
        return width() - m_floatButton->x();
    else
        return width() - m_closeButton->x();
}

TitleBarWidget::~TitleBarWidget()
{
    // To avoid a crash
    for (auto button : { m_minimizeButton, m_floatButton, m_maximizeButton, m_closeButton }) {
        button->setParent(nullptr);
        button->deleteLater();
    }
}

void TitleBarWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        onDoubleClicked();
}

QWidget *TitleBarWidget::closeButton() const
{
    return m_closeButton;
}

void TitleBarWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QStyleOptionDockWidget titleOpt;
    titleOpt.title = title();
    titleOpt.rect = iconRect().isEmpty() ? rect().adjusted(2, 0, -buttonAreaWidth(), 0)
                                         : rect().adjusted(iconRect().right(), 0, -buttonAreaWidth(), 0);

    style()->drawControl(QStyle::CE_DockWidgetTitle, &titleOpt, &p, this);
}

void TitleBarWidget::updateFloatButton()
{
    m_floatButton->setVisible(supportsFloatingButton());
}

void TitleBarWidget::updateCloseButton()
{
    const bool anyNonClosable = frame() ? frame()->anyNonClosable()
                                        : (floatingWindow() ? floatingWindow()->anyNonClosable()
                                                            : false);

    qCDebug(closebutton) << Q_FUNC_INFO << "enabled=" << !anyNonClosable;
    m_closeButton->setEnabled(!anyNonClosable);
}

void TitleBarWidget::updateMinimizeButton()
{
    m_minimizeButton->setVisible(supportsMinimizeButton());
}

void TitleBarWidget::updateMaximizeButton()
{
    if (auto fw = floatingWindow()) {
        m_maximizeButton->setIcon(style()->standardIcon(fw->isMaximized() ? QStyle::SP_TitleBarNormalButton
                                                                          : QStyle::SP_TitleBarMaxButton));

        m_maximizeButton->setVisible(supportsMaximizeButton());
    } else {
        m_maximizeButton->setVisible(false);
    }
}

bool TitleBarWidget::isCloseButtonVisible() const
{
    return m_closeButton->isVisible();
}

bool TitleBarWidget::isCloseButtonEnabled() const
{
    return m_closeButton->isEnabled();
}

bool TitleBarWidget::isFloatButtonVisible() const
{
    return m_floatButton->isVisible();
}

bool TitleBarWidget::isFloatButtonEnabled() const
{
    return m_floatButton->isEnabled();
}

QAbstractButton *TitleBarWidget::createButton(QWidget *parent, const QIcon &icon)
{
    auto button = new Button(parent);
    button->setIcon(icon);
    return button;
}
