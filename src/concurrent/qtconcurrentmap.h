/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtConcurrent module of the Qt Toolkit.
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

#ifndef QTCONCURRENT_MAP_H
#define QTCONCURRENT_MAP_H

#include <QtConcurrent/qtconcurrent_global.h>

#if !defined(QT_NO_CONCURRENT) || defined(Q_CLANG_QDOC)

#include <QtConcurrent/qtconcurrentmapkernel.h>
#include <QtConcurrent/qtconcurrentreducekernel.h>
#include <QtConcurrent/qtconcurrentfunctionwrappers.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE



namespace QtConcurrent {

// map() on sequences
template <typename Sequence, typename MapFunctor>
QFuture<void> map(QThreadPool *pool, Sequence &sequence, MapFunctor map)
{
    return startMap(pool, sequence.begin(), sequence.end(), map);
}

template <typename Sequence, typename MapFunctor>
QFuture<void> map(Sequence &sequence, MapFunctor map)
{
    return startMap(QThreadPool::globalInstance(), sequence.begin(), sequence.end(), map);
}

// map() on iterators
template <typename Iterator, typename MapFunctor>
QFuture<void> map(QThreadPool *pool, Iterator begin, Iterator end, MapFunctor map)
{
    return startMap(pool, begin, end, map);
}

template <typename Iterator, typename MapFunctor>
QFuture<void> map(Iterator begin, Iterator end, MapFunctor map)
{
    return startMap(QThreadPool::globalInstance(), begin, end, map);
}

// mappedReduced() for sequences.
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, options);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce, options);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)),  options);
}

template <typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, options);
}

template <typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> mappedReduced(
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce, options);
}

template <typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
}

template <typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
}

// mappedReduced() for iterators
template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, options);
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce, options);
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, options);
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce, options);
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(QThreadPool *pool,
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  InitialValueType &&initialValue,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                        | SequentialReduce))
{
    return startMappedReduced<QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
}

// mapped() for sequences
template <typename Sequence, typename MapFunctor>
QFuture<QtPrivate::MapResultType<Sequence, MapFunctor>> mapped(
                                  QThreadPool *pool,
                                  const Sequence &sequence,
                                  MapFunctor map)
{
    return startMapped<QtPrivate::MapResultType<Sequence, MapFunctor>>(pool, sequence, map);
}

template <typename Sequence, typename MapFunctor>
QFuture<QtPrivate::MapResultType<Sequence, MapFunctor>> mapped(
                                  const Sequence &sequence,
                                  MapFunctor map)
{
    return startMapped<QtPrivate::MapResultType<Sequence, MapFunctor>>
        (QThreadPool::globalInstance(), sequence, map);
}

// mapped() for iterator ranges.
template <typename Iterator, typename MapFunctor>
QFuture<QtPrivate::MapResultType<Iterator, MapFunctor>> mapped(
                                  QThreadPool *pool,
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map)
{
    return startMapped<QtPrivate::MapResultType<Iterator, MapFunctor>>(pool, begin, end, map);
}

template <typename Iterator, typename MapFunctor>
QFuture<QtPrivate::MapResultType<Iterator, MapFunctor>> mapped(
                                  Iterator begin,
                                  Iterator end,
                                  MapFunctor map)
{
    return startMapped<QtPrivate::MapResultType<Iterator, MapFunctor>>
        (QThreadPool::globalInstance(), begin, end, map);
}

// blockingMap() for sequences
template <typename Sequence, typename MapFunctor>
void blockingMap(QThreadPool *pool, Sequence &sequence, MapFunctor map)
{
    QFuture<void> future = startMap(pool, sequence.begin(), sequence.end(), map);
    future.waitForFinished();
}

template <typename Sequence, typename MapFunctor>
void blockingMap(Sequence &sequence, MapFunctor map)
{
    QFuture<void> future = startMap(QThreadPool::globalInstance(), sequence.begin(), sequence.end(), map);
    future.waitForFinished();
}

// blockingMap() for iterator ranges
template <typename Iterator, typename MapFunctor>
void blockingMap(QThreadPool *pool, Iterator begin, Iterator end, MapFunctor map)
{
    QFuture<void> future = startMap(pool, begin, end, map);
    future.waitForFinished();
}

template <typename Iterator, typename MapFunctor>
void blockingMap(Iterator begin, Iterator end, MapFunctor map)
{
    QFuture<void> future = startMap(QThreadPool::globalInstance(), begin, end, map);
    future.waitForFinished();
}

// blockingMappedReduced() for sequences
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce, options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename MapFunctor, typename ReduceFunctor, typename Sequence,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, options);
    return future.result();
}

template <typename MapFunctor, typename ReduceFunctor, typename Sequence,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce, options);
    return future.result();
}

template <typename MapFunctor, typename ReduceFunctor, typename Sequence,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (pool, sequence, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
    return future.result();
}

template <typename MapFunctor, typename ReduceFunctor, typename Sequence,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Sequence, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), sequence, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

// blockingMappedReduced() for iterator ranges
template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce, options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, options);
    return future.result();
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce, options);
    return future.result();
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(QThreadPool *pool,
                                 Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (pool, begin, end, map, reduce, ResultType(std::forward<InitialValueType>(initialValue)),
         options);
    return future.result();
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 InitialValueType &&initialValue,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                       | SequentialReduce))
{
    QFuture<ResultType> future = QtConcurrent::startMappedReduced
        <QtPrivate::MapResultType<Iterator, MapFunctor>, ResultType>
        (QThreadPool::globalInstance(), begin, end, map, reduce,
         ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

// mapped() for sequences with a different putput sequence type.
template <typename OutputSequence, typename InputSequence, typename MapFunctor>
OutputSequence blockingMapped(QThreadPool *pool, const InputSequence &sequence, MapFunctor map)
{
    return blockingMappedReduced<OutputSequence>(pool, sequence, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename OutputSequence, typename InputSequence, typename MapFunctor>
OutputSequence blockingMapped(const InputSequence &sequence, MapFunctor map)
{
    return blockingMappedReduced<OutputSequence>(QThreadPool::globalInstance(), sequence, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename MapFunctor, typename InputSequence>
auto blockingMapped(QThreadPool *pool, const InputSequence &sequence, MapFunctor map)
{
    using OutputSequence = typename QtPrivate::MapSequenceResultType<InputSequence,
                                               MapFunctor>::ResultType;
    return blockingMappedReduced<OutputSequence>(pool, sequence, map, QtPrivate::PushBackWrapper(),
        OrderedReduce);
}

template <typename MapFunctor, typename InputSequence>
auto blockingMapped(const InputSequence &sequence, MapFunctor map)
{
    using OutputSequence = typename QtPrivate::MapSequenceResultType<InputSequence,
                                               MapFunctor>::ResultType;
    return blockingMappedReduced<OutputSequence>(QThreadPool::globalInstance(), sequence, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

// mapped()  for iterator ranges
template <typename Sequence, typename Iterator, typename MapFunctor>
Sequence blockingMapped(QThreadPool *pool, Iterator begin, Iterator end, MapFunctor map)
{
    return blockingMappedReduced<Sequence>(pool, begin, end, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename Sequence, typename Iterator, typename MapFunctor>
Sequence blockingMapped(Iterator begin, Iterator end, MapFunctor map)
{
    return blockingMappedReduced<Sequence>(QThreadPool::globalInstance(), begin, end, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename Iterator, typename MapFunctor>
auto blockingMapped(QThreadPool *pool, Iterator begin, Iterator end, MapFunctor map)
{
    using OutputSequence = QtPrivate::MapResultType<Iterator, MapFunctor>;
    return blockingMappedReduced<OutputSequence>(pool, begin, end, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename Iterator, typename MapFunctor>
auto blockingMapped(Iterator begin, Iterator end, MapFunctor map)
{
    using OutputSequence = QtPrivate::MapResultType<Iterator, MapFunctor>;
    return blockingMappedReduced<OutputSequence>(QThreadPool::globalInstance(), begin, end, map,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

} // namespace QtConcurrent


QT_END_NAMESPACE

#endif // QT_NO_CONCURRENT

#endif
