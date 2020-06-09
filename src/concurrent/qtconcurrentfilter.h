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

#ifndef QTCONCURRENT_FILTER_H
#define QTCONCURRENT_FILTER_H

#include <QtConcurrent/qtconcurrent_global.h>

#if !defined(QT_NO_CONCURRENT) || defined(Q_CLANG_QDOC)

#include <QtConcurrent/qtconcurrentfilterkernel.h>
#include <QtConcurrent/qtconcurrentfunctionwrappers.h>

QT_BEGIN_NAMESPACE

namespace QtConcurrent {

//! [QtConcurrent-1]
template <typename Sequence, typename KeepFunctor, typename ReduceFunctor>
ThreadEngineStarter<void> filterInternal(QThreadPool *pool, Sequence &sequence,
                                         KeepFunctor keep, ReduceFunctor reduce)
{
    typedef FilterKernel<Sequence, KeepFunctor, ReduceFunctor> KernelType;
    return startThreadEngine(new KernelType(pool, sequence, keep, reduce));
}

// filter() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<void> filter(QThreadPool *pool, Sequence &sequence, KeepFunctor keep)
{
    return filterInternal(pool, sequence, keep, QtPrivate::PushBackWrapper());
}

template <typename Sequence, typename KeepFunctor>
QFuture<void> filter(Sequence &sequence, KeepFunctor keep)
{
    return filterInternal(QThreadPool::globalInstance(),
                          sequence, keep, QtPrivate::PushBackWrapper());
}

// filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, sequence, keep, reduce, options);
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                            sequence, keep, reduce, options);
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(), sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

#ifndef Q_CLANG_QDOC
template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, sequence, keep, reduce, options);
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                            sequence, keep, reduce, options);
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(), sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}
#endif

// filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
   return startFilteredReduced<ResultType>(pool, begin, end, keep, reduce, options);
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
   return startFilteredReduced<ResultType>(QThreadPool::globalInstance(), begin, end, keep, reduce,
           options);
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(), begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

#ifndef Q_CLANG_QDOC
template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
   return startFilteredReduced<ResultType>(pool, begin, end, keep, reduce, options);
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
   return startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                           begin, end, keep, reduce, options);
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(QThreadPool *pool,
                                    Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(pool, begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    InitialValueType &&initialValue,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                          | SequentialReduce))
{
    return startFilteredReduced<ResultType>(QThreadPool::globalInstance(), begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
}
#endif

// filtered() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<typename Sequence::value_type> filtered(QThreadPool *pool,
                                                const Sequence &sequence,
                                                KeepFunctor keep)
{
    return startFiltered(pool, sequence, keep);
}

template <typename Sequence, typename KeepFunctor>
QFuture<typename Sequence::value_type> filtered(const Sequence &sequence, KeepFunctor keep)
{
    return startFiltered(QThreadPool::globalInstance(), sequence, keep);
}

// filtered() on iterators
template <typename Iterator, typename KeepFunctor>
QFuture<typename qValueType<Iterator>::value_type> filtered(QThreadPool *pool,
                                                            Iterator begin,
                                                            Iterator end,
                                                            KeepFunctor keep)
{
    return startFiltered(pool, begin, end, keep);
}

template <typename Iterator, typename KeepFunctor>
QFuture<typename qValueType<Iterator>::value_type> filtered(Iterator begin,
                                                            Iterator end,
                                                            KeepFunctor keep)
{
    return startFiltered(QThreadPool::globalInstance(), begin, end, keep);
}

// blocking filter() on sequences
template <typename Sequence, typename KeepFunctor>
void blockingFilter(QThreadPool *pool, Sequence &sequence, KeepFunctor keep)
{
    QFuture<void> future = filterInternal(pool, sequence, keep, QtPrivate::PushBackWrapper());
    future.waitForFinished();
}

template <typename Sequence, typename KeepFunctor>
void blockingFilter(Sequence &sequence, KeepFunctor keep)
{
    QFuture<void> future = filterInternal(QThreadPool::globalInstance(), sequence, keep,
                                          QtPrivate::PushBackWrapper());
    future.waitForFinished();
}

// blocking filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, sequence, keep,
                                 reduce, options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                 sequence, keep, reduce, options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
        sequence, keep, reduce, ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

#ifndef Q_CLANG_QDOC
template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, sequence, keep,
                                 reduce, options);
    return future.result();
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                 sequence, keep, reduce, options);
    return future.result();
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, sequence, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename Sequence, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
        sequence, keep, reduce, ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}
#endif

// blocking filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, begin, end, keep,
                                 reduce, options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                 begin, end, keep, reduce, options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
        begin, end, keep, reduce, ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

#ifndef Q_CLANG_QDOC
template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, begin, end, keep,
                                                                  reduce, options);
    return future.result();
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
                                 begin, end, keep, reduce, options);
    return future.result();
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(QThreadPool *pool,
                                   Iterator begin,
                                   Iterator end, KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(pool, begin, end, keep, reduce,
        ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}

template <typename Iterator, typename KeepFunctor, typename ReduceFunctor,
          typename ResultType = typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType,
          typename InitialValueType,
          std::enable_if_t<std::is_convertible_v<InitialValueType, ResultType>, int> = 0>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   InitialValueType &&initialValue,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce
                                                                         | SequentialReduce))
{
    QFuture<ResultType> future = startFilteredReduced<ResultType>(QThreadPool::globalInstance(),
        begin, end, keep, reduce, ResultType(std::forward<InitialValueType>(initialValue)), options);
    return future.result();
}
#endif

// blocking filtered() on sequences
template <typename Sequence, typename KeepFunctor>
Sequence blockingFiltered(QThreadPool *pool, const Sequence &sequence, KeepFunctor keep)
{
    return blockingFilteredReduced<Sequence>(pool, sequence, keep, QtPrivate::PushBackWrapper(),
        OrderedReduce);
}

template <typename Sequence, typename KeepFunctor>
Sequence blockingFiltered(const Sequence &sequence, KeepFunctor keep)
{
    return blockingFilteredReduced<Sequence>(QThreadPool::globalInstance(), sequence, keep,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

// blocking filtered() on iterators
template <typename OutputSequence, typename Iterator, typename KeepFunctor>
OutputSequence blockingFiltered(QThreadPool *pool, Iterator begin, Iterator end, KeepFunctor keep)
{
    return blockingFilteredReduced<OutputSequence>(pool, begin, end, keep,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

template <typename OutputSequence, typename Iterator, typename KeepFunctor>
OutputSequence blockingFiltered(Iterator begin, Iterator end, KeepFunctor keep)
{
    return blockingFilteredReduced<OutputSequence>(QThreadPool::globalInstance(), begin, end, keep,
        QtPrivate::PushBackWrapper(), OrderedReduce);
}

} // namespace QtConcurrent

QT_END_NAMESPACE

#endif // QT_NO_CONCURRENT

#endif
