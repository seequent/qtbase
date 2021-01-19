/****************************************************************************
**
** Copyright (C) 2014 BogDan Vatra <bogdan@kde.org>
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qandroidplatformwindow.h"
#include "qandroidplatformopenglcontext.h"
#include "qandroidplatformscreen.h"

#include "androidjnimain.h"

#include <qguiapplication.h>
#include <qpa/qwindowsysteminterface.h>
#include <private/qhighdpiscaling_p.h>

QT_BEGIN_NAMESPACE

static QBasicAtomicInt winIdGenerator = Q_BASIC_ATOMIC_INITIALIZER(0);

QAndroidPlatformWindow::QAndroidPlatformWindow(QWindow *window)
    : QPlatformWindow(window)
{
    m_windowFlags = Qt::Widget;
    m_windowState = Qt::WindowNoState;
    m_windowId = winIdGenerator.fetchAndAddRelaxed(1) + 1;
    setWindowState(window->windowStates());

    const bool forceMaximize = m_windowState & (Qt::WindowMaximized | Qt::WindowFullScreen);
    const QRect requestedGeometry = forceMaximize ? QRect() : window->geometry();
    const QRect availableGeometry = (window->parent()) ? window->parent()->geometry() : platformScreen()->availableGeometry();
    const QRect finalGeometry = QPlatformWindow::initialGeometry(window, requestedGeometry,
                                                                 availableGeometry.width(), availableGeometry.height());

   if (requestedGeometry != finalGeometry)
       setGeometry(QHighDpi::toNativePixels(finalGeometry, window));
}

void QAndroidPlatformWindow::lower()
{
    platformScreen()->lower(this);
}

void QAndroidPlatformWindow::raise()
{
    updateSystemUiVisibility();
    platformScreen()->raise(this);
}

QMargins QAndroidPlatformWindow::safeAreaMargins() const
{
    if ((m_windowState & Qt::WindowMaximized) && (window()->flags() & Qt::MaximizeUsingFullscreenGeometryHint)) {
        QRect availableGeometry = platformScreen()->availableGeometry();
        return QMargins(availableGeometry.left(), availableGeometry.top(),
                        availableGeometry.right(), availableGeometry.bottom());
    } else {
        return QPlatformWindow::safeAreaMargins();
    }
}

void QAndroidPlatformWindow::setGeometry(const QRect &rect)
{
    QPlatformWindow::setGeometry(rect);
    QWindowSystemInterface::handleGeometryChange(window(), rect);
}

void QAndroidPlatformWindow::setVisible(bool visible)
{
    if (visible)
        updateSystemUiVisibility();

    if (visible) {
        if ((m_windowState & Qt::WindowFullScreen)
                || ((m_windowState & Qt::WindowMaximized) && (window()->flags() & Qt::MaximizeUsingFullscreenGeometryHint))) {
            setGeometry(platformScreen()->geometry());
        } else if (m_windowState & Qt::WindowMaximized) {
            setGeometry(platformScreen()->availableGeometry());
        }
    }

    if (visible)
        platformScreen()->addWindow(this);
    else
        platformScreen()->removeWindow(this);

    QRect availableGeometry = screen()->availableGeometry();
    if (geometry().width() > 0 && geometry().height() > 0 && availableGeometry.width() > 0 && availableGeometry.height() > 0)
        QPlatformWindow::setVisible(visible);
}

void QAndroidPlatformWindow::setWindowState(Qt::WindowStates state)
{
    if (m_windowState == state)
        return;

    QPlatformWindow::setWindowState(state);
    m_windowState = state;

    if (window()->isVisible())
        updateSystemUiVisibility();
}

void QAndroidPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
    if (m_windowFlags == flags)
        return;

    m_windowFlags = flags;
}

Qt::WindowFlags QAndroidPlatformWindow::windowFlags() const
{
    return m_windowFlags;
}

void QAndroidPlatformWindow::setParent(const QPlatformWindow *window)
{
    Q_UNUSED(window);
}

QAndroidPlatformScreen *QAndroidPlatformWindow::platformScreen() const
{
    return static_cast<QAndroidPlatformScreen *>(window()->screen()->handle());
}

void QAndroidPlatformWindow::propagateSizeHints()
{
    //shut up warning from default implementation
}

void QAndroidPlatformWindow::requestActivateWindow()
{
    platformScreen()->topWindowChanged(window());
}

void QAndroidPlatformWindow::updateSystemUiVisibility()
{
    Qt::WindowFlags flags = window()->flags();
    bool isNonRegularWindow = flags & (Qt::Popup | Qt::Dialog | Qt::Sheet) & ~Qt::Window;
    if (!isNonRegularWindow) {
        if (m_windowState & Qt::WindowFullScreen)
            QtAndroid::setSystemUiVisibility(QtAndroid::SYSTEM_UI_VISIBILITY_FULLSCREEN);
        else if (flags & Qt::MaximizeUsingFullscreenGeometryHint)
            QtAndroid::setSystemUiVisibility(QtAndroid::SYSTEM_UI_VISIBILITY_TRANSLUCENT);
        else
            QtAndroid::setSystemUiVisibility(QtAndroid::SYSTEM_UI_VISIBILITY_NORMAL);
    }
}

bool QAndroidPlatformWindow::isExposed() const
{
    return qApp->applicationState() > Qt::ApplicationHidden
            && window()->isVisible()
            && !window()->geometry().isEmpty();
}

void QAndroidPlatformWindow::applicationStateChanged(Qt::ApplicationState)
{
    QRegion region;
    if (isExposed())
        region = QRect(QPoint(), geometry().size());

    QWindowSystemInterface::handleExposeEvent(window(), region);
    QWindowSystemInterface::flushWindowSystemEvents();
}

QT_END_NAMESPACE
