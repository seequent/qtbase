/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QABSTRACTFILEICONPROVIDER_H
#define QABSTRACTFILEICONPROVIDER_H

#include <QtGui/qtguiglobal.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qscopedpointer.h>
#include <QtGui/qicon.h>

QT_BEGIN_NAMESPACE

class QAbstractFileIconProviderPrivate;

class Q_GUI_EXPORT QAbstractFileIconProvider
{
public:
    enum IconType { Computer, Desktop, Trashcan, Network, Drive, Folder, File };
    enum Option {
        DontUseCustomDirectoryIcons = 0x00000001
    };
    Q_DECLARE_FLAGS(Options, Option)

    QAbstractFileIconProvider();
    virtual ~QAbstractFileIconProvider();

    virtual QIcon icon(IconType) const;
    virtual QIcon icon(const QFileInfo &) const;
    virtual QString type(const QFileInfo &) const;

    virtual void setOptions(Options);
    virtual Options options() const;

protected:
    QAbstractFileIconProvider(QAbstractFileIconProviderPrivate &dd);
    QScopedPointer<QAbstractFileIconProviderPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(QAbstractFileIconProvider)
    Q_DISABLE_COPY(QAbstractFileIconProvider)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractFileIconProvider::Options)

QT_END_NAMESPACE

#endif // QABSTRACTFILEICONPROVIDER_H
