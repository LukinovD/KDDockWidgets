/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

/**
 * @file
 * @brief QMainWindow wrapper to enable KDDockWidgets support.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#include "Config.h"
#include "MainWindow.h"
#include "DropArea_p.h"
#include "Frame_p.h"
#include "Logging_p.h"
#include "private/widgets/SideBarWidget_p.h"
#include "DropAreaWithCentralFrame_p.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QPainter>

using namespace KDDockWidgets;

class MainWindow::Private
{
public:

    explicit Private(MainWindowOptions options, MainWindowBase *mainWindow)
        : m_supportsMinimize(Config::self().supportsAutoHide())
        , m_dropArea(new DropAreaWithCentralFrame(mainWindow, options))
        , m_sideBar(m_supportsMinimize ? new SideBarWidget(Qt::Horizontal, mainWindow)
                                       : nullptr)
    {
    }

    const bool m_supportsMinimize;
    DropAreaWithCentralFrame *const m_dropArea;
    SideBar *const m_sideBar;

};

namespace KDDockWidgets {
class MyCentralWidget : public QWidget
{
public:
    explicit MyCentralWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("MyCentralWidget"));
    }

    ~MyCentralWidget() override;

    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        QPen pen(QColor(184, 184, 184, 184));
        p.setPen(pen);
        p.drawLine(0, 0, width(), 0);
    }
};
}

MyCentralWidget::~MyCentralWidget() {}


MainWindow::MainWindow(const QString &name, MainWindowOptions options,
                       QWidget *parent, Qt::WindowFlags flags)
    : MainWindowBase(name, options, parent, flags)
    , d(new Private(options, this))
{
    auto centralWidget = new MyCentralWidget(this);
    auto layout = new QVBoxLayout(centralWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(1, 5, 1, 1);
    layout->addWidget(dropArea()); // 1 level of indirection so we can add some margins

    if (d->m_supportsMinimize)
        layout->addWidget(d->m_sideBar);

    setCentralWidget(centralWidget);

    // qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::setCentralWidget(QWidget *w)
{
    QMainWindow::setCentralWidget(w);
}

SideBar *MainWindow::sideBar() const
{
    return d->m_sideBar;
}
