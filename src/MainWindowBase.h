/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/


/**
 * @file
 * @brief The MainWindow base-class that's shared between QtWidgets and QtQuick stack.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#ifndef KD_MAINWINDOW_BASE_H
#define KD_MAINWINDOW_BASE_H

#include "docks_export.h"
#include "KDDockWidgets.h"
#include "QWidgetAdapter.h"
#include "LayoutSaver_p.h"

#include <QVector>

namespace KDDockWidgets {

class DockWidgetBase;
class Frame;
class DropArea;
class MultiSplitter;
class DropAreaWithCentralFrame;
class SideBar;

/**
 * @brief The MainWindow base-class. MainWindow and MainWindowBase are only
 * split in two so we can share some code with the QtQuick implementation,
 * which also derives from MainWindowBase.
 *
 * Do not use instantiate directly in user code. Use MainWindow instead.
 */
#ifndef PYTHON_BINDINGS //Pyside bug: https://bugreports.qt.io/projects/PYSIDE/issues/PYSIDE-1327
class DOCKS_EXPORT MainWindowBase : public QMainWindowOrQuick
#else
class DOCKS_EXPORT MainWindowBase : public QMainWindow
#endif
{
    Q_OBJECT
public:
    typedef QVector<MainWindowBase*> List;
    explicit MainWindowBase(const QString &uniqueName, MainWindowOptions options = MainWindowOption_HasCentralFrame,
                            QWidgetOrQuick *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~MainWindowBase() override;

    /**
     * @brief Docks a DockWidget into the central frame, tabbed.
     * @warning Requires that the MainWindow was constructed with MainWindowOption_HasCentralFrame option.
     * @param dockwidget The dockwidget to dock.
     *
     * @sa DockWidgetBase::addDockWidgetAsTab()
     */
    void addDockWidgetAsTab(DockWidgetBase *dockwidget);

    /**
     * @brief Docks a DockWidget into this main window.
     * @param dockWidget The dock widget to add into this MainWindow
     * @param location the location where to dock
     * @param relativeTo In case we're docking in relation to another dock widget
     * @param option AddingOptions
     */
    void addDockWidget(DockWidgetBase *dockWidget,
                       KDDockWidgets::Location location,
                       DockWidgetBase *relativeTo = nullptr, AddingOption option = KDDockWidgets::AddingOption());

    /**
     * @brief Returns the unique name that was passed via constructor.
     *        Used internally by the save/restore mechanism.
     * @internal
     */
    QString uniqueName() const;


    /// @brief Returns the main window options that were passed via constructor.
    MainWindowOptions options() const;

    ///@internal
    ///@brief returns the drop area.
    DropAreaWithCentralFrame *dropArea() const;

    ///@internal
    ///@brief returns the MultiSplitter.
    MultiSplitter* multiSplitter() const;

    /**
     * @brief Sets the affinities names. Dock widgets can only dock into main windows of the same affinity.
     *
     * By default the affinity is empty and a dock widget can dock into any main window. Usually you
     * won't ever need to call this function, unless you have requirements where certain dock widgets
     * can only dock into certain main windows. @sa DockWidgetBase::setAffinities().
     *
     * Note: Call this function right after creating your main window, before docking any dock widgets
     * into a main window and before restoring any layout.
     *
     * Note: Currently you can only call this function once, to keep the code simple and avoid
     * edge cases. This will only be changed if a good use case comes up that requires changing
     * affinities multiple times.
     *
     * @p name The affinity names.
     */
    void setAffinities(const QStringList &names);

    /**
     * @brief Returns the list of affinity names. Empty by default.
     */
    QStringList affinities() const;

    /// @brief layouts all the widgets so they have an equal size within their parent container
    ///
    /// Note that the layout is a tree of nested horizontal and vertical container layouts. The
    /// nodes closer to the root will have more space.
    ///
    /// min/max constraints will still be honoured.
    void layoutEqually();

    /// @brief like layoutEqually() but starts with the container that has @p dockWidget.
    /// While layoutEqually() starts from the root of the layout tree this function starts on a
    /// sub-tree.
    void layoutParentContainerEqually(DockWidgetBase *dockWidget);

    ///@brief Minimizes dock widget @p into the side bar
    void minimizeDockWidget(DockWidgetBase *dw);

protected:
    void setUniqueName(const QString &uniqueName);

    virtual SideBar* sideBar() const = 0;

Q_SIGNALS:
    void uniqueNameChanged();

private:
    class Private;
    Private *const d;

    friend class LayoutSaver;
    bool deserialize(const LayoutSaver::MainWindow &);
    LayoutSaver::MainWindow serialize() const;
};

}

#endif
