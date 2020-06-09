/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtconcurrentmap.h>
#include <qexception.h>

#include <qdebug.h>
#include <QThread>
#include <QMutex>

#include <QtTest/QtTest>

#include "functions.h"

class tst_QtConcurrentMap : public QObject
{
    Q_OBJECT
private slots:
    void map();
    void blocking_map();
    void mapped();
    void mappedThreadPool();
    void mappedReduced();
    void mappedReducedThreadPool();
    void mappedReducedDifferentType();
    void mappedReducedInitialValue();
    void mappedReducedInitialValueThreadPool();
    void mappedReducedDifferentTypeInitialValue();
    void assignResult();
    void functionOverloads();
    void noExceptFunctionOverloads();
#ifndef QT_NO_EXCEPTIONS
    void exceptions();
#endif
    void incrementalResults();
    void noDetach();
    void stlContainers();
    void stlContainersLambda();
    void qFutureAssignmentLeak();
    void stressTest();
    void persistentResultTest();
public slots:
    void throttling();
};

using namespace QtConcurrent;

void multiplyBy2Immutable(int x)
{
    x *= 2;
}

class MultiplyBy2Immutable
{
public:
    void operator()(int x)
    {
        x *= 2;
    }
};

void multiplyBy2InPlace(int &x)
{
    x *= 2;
}

class MultiplyBy2InPlace
{
public:
    void operator()(int &x)
    {
        x *= 2;
    }
};

Q_DECLARE_METATYPE(QList<Number>);

void tst_QtConcurrentMap::map()
{
    // functors take arguments by reference, modifying the sequence in place
    {
        QList<int> list;
        list << 1 << 2 << 3;

        // functor
        QtConcurrent::map(list, MultiplyBy2InPlace()).waitForFinished();
        QCOMPARE(list, QList<int>() << 2 << 4 << 6);
        QtConcurrent::map(list.begin(), list.end(), MultiplyBy2InPlace()).waitForFinished();
        QCOMPARE(list, QList<int>() << 4 << 8 << 12);

        // function
        QtConcurrent::map(list, multiplyBy2InPlace).waitForFinished();
        QCOMPARE(list, QList<int>() << 8 << 16 << 24);
        QtConcurrent::map(list.begin(), list.end(), multiplyBy2InPlace).waitForFinished();
        QCOMPARE(list, QList<int>() << 16 << 32 << 48);

        // bound function
        QtConcurrent::map(list, multiplyBy2InPlace).waitForFinished();
        QCOMPARE(list, QList<int>() << 32 << 64 << 96);
        QtConcurrent::map(list.begin(), list.end(), multiplyBy2InPlace).waitForFinished();
        QCOMPARE(list, QList<int>() << 64 << 128 << 192);

        // member function
        QList<Number> numberList;
        numberList << 1 << 2 << 3;
        QtConcurrent::map(numberList, &Number::multiplyBy2).waitForFinished();
        QCOMPARE(numberList, QList<Number>() << 2 << 4 << 6);
        QtConcurrent::map(numberList.begin(), numberList.end(), &Number::multiplyBy2).waitForFinished();
        QCOMPARE(numberList, QList<Number>() << 4 << 8 << 12);

        // lambda
        QtConcurrent::map(list, [](int &x){x *= 2;}).waitForFinished();
        QCOMPARE(list, QList<int>() << 128 << 256 << 384);
        QtConcurrent::map(list.begin(), list.end(), [](int &x){x *= 2;}).waitForFinished();
        QCOMPARE(list, QList<int>() << 256 << 512 << 768);
    }

    // functors don't take arguments by reference, making these no-ops
    {
        QList<int> list;
        list << 1 << 2 << 3;

        // functor
        QtConcurrent::map(list, MultiplyBy2Immutable()).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::map(list.begin(), list.end(), MultiplyBy2Immutable()).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // function
        QtConcurrent::map(list, multiplyBy2Immutable).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::map(list.begin(), list.end(), multiplyBy2Immutable).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // bound function
        QtConcurrent::map(list, multiplyBy2Immutable).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::map(list.begin(), list.end(), multiplyBy2Immutable).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // lambda
        QtConcurrent::map(list, [](int x){x *= 2;}).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::map(list.begin(), list.end(), [](int x){x *= 2;}).waitForFinished();
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
    }

#if 0
    // not allowed: map() with immutable sequences makes no sense
    {
        const QList<int> list = QList<int>() << 1 << 2 << 3;

        QtConcurrent::map(list, MultiplyBy2Immutable());
        QtConcurrent::map(list, multiplyBy2Immutable);
        QtConcurrent::map(list, multiplyBy2Immutable);
    }
#endif

#if 0
    // not allowed: in place modification of a temp copy (since temp copy goes out of scope)
    {
        QList<int> list;
        list << 1 << 2 << 3;

        QtConcurrent::map(QList<int>(list), MultiplyBy2InPlace());
        QtConcurrent::map(QList<int>(list), multiplyBy2);
        QtConcurrent::map(QList<int>(list), multiplyBy2InPlace);

        QList<Number> numberList;
        numberList << 1 << 2 << 3;
        QtConcurrent::map(QList<Number>(numberList), &Number::multiplyBy2);
    }
#endif

#if 0
    // not allowed: map() on a const list, where functors try to modify the items in the list
    {
        const QList<int> list = QList<int>() << 1 << 2 << 3;;

        QtConcurrent::map(list, MultiplyBy2InPlace());
        QtConcurrent::map(list, multiplyBy2InPlace);
        QtConcurrent::map(list, multiplyBy2InPlace);

        const QList<Number> numberList = QList<Number>() << 1 << 2 << 3;
        QtConcurrent::map(numberList, &Number::multiplyBy2);
    }
#endif
}

void tst_QtConcurrentMap::blocking_map()
{
    // functors take arguments by reference, modifying the sequence in place
    {
        QList<int> list;
        list << 1 << 2 << 3;

        // functor
        QtConcurrent::blockingMap(list, MultiplyBy2InPlace());
        QCOMPARE(list, QList<int>() << 2 << 4 << 6);
        QtConcurrent::blockingMap(list.begin(), list.end(), MultiplyBy2InPlace());
        QCOMPARE(list, QList<int>() << 4 << 8 << 12);

        // function
        QtConcurrent::blockingMap(list, multiplyBy2InPlace);
        QCOMPARE(list, QList<int>() << 8 << 16 << 24);
        QtConcurrent::blockingMap(list.begin(), list.end(), multiplyBy2InPlace);
        QCOMPARE(list, QList<int>() << 16 << 32 << 48);

        // bound function
        QtConcurrent::blockingMap(list, multiplyBy2InPlace);
        QCOMPARE(list, QList<int>() << 32 << 64 << 96);
        QtConcurrent::blockingMap(list.begin(), list.end(), multiplyBy2InPlace);
        QCOMPARE(list, QList<int>() << 64 << 128 << 192);

        // member function
        QList<Number> numberList;
        numberList << 1 << 2 << 3;
        QtConcurrent::blockingMap(numberList, &Number::multiplyBy2);
        QCOMPARE(numberList, QList<Number>() << 2 << 4 << 6);
        QtConcurrent::blockingMap(numberList.begin(), numberList.end(), &Number::multiplyBy2);
        QCOMPARE(numberList, QList<Number>() << 4 << 8 << 12);

        // lambda
        QtConcurrent::blockingMap(list, [](int &x) { x *= 2; });
        QCOMPARE(list, QList<int>() << 128 << 256 << 384);
        QtConcurrent::blockingMap(list.begin(), list.end(), [](int &x) { x *= 2; });
        QCOMPARE(list, QList<int>() << 256 << 512 << 768);
    }

    // functors don't take arguments by reference, making these no-ops
    {
        QList<int> list;
        list << 1 << 2 << 3;

        // functor
        QtConcurrent::blockingMap(list, MultiplyBy2Immutable());
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::blockingMap(list.begin(), list.end(), MultiplyBy2Immutable());
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // function
        QtConcurrent::blockingMap(list, multiplyBy2Immutable);
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::blockingMap(list.begin(), list.end(), multiplyBy2Immutable);
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // bound function
        QtConcurrent::blockingMap(list, multiplyBy2Immutable);
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::blockingMap(list.begin(), list.end(), multiplyBy2Immutable);
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);

        // lambda
        QtConcurrent::blockingMap(list, [](int x) { x *= 2; });
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
        QtConcurrent::blockingMap(list.begin(), list.end(), [](int x) { x *= 2; });
        QCOMPARE(list, QList<int>() << 1 << 2 << 3);
    }

#if 0
    // not allowed: map() with immutable sequences makes no sense
    {
        const QList<int> list = QList<int>() << 1 << 2 << 3;

        QtConcurrent::blockingMap(list, MultiplyBy2Immutable());
        QtConcurrent::blockkng::map(list, multiplyBy2Immutable);
        QtConcurrent::blockingMap(list, multiplyBy2Immutable);
    }
#endif

#if 0
    // not allowed: in place modification of a temp copy (since temp copy goes out of scope)
    {
        QList<int> list;
        list << 1 << 2 << 3;

        QtConcurrent::blockingMap(QList<int>(list), MultiplyBy2InPlace());
        QtConcurrent::blockingMap(QList<int>(list), multiplyBy2);
        QtConcurrent::blockingMap(QList<int>(list), multiplyBy2InPlace);

        QList<Number> numberList;
        numberList << 1 << 2 << 3;
        QtConcurrent::blockingMap(QList<Number>(numberList), &Number::multiplyBy2);
    }
#endif

#if 0
    // not allowed: map() on a const list, where functors try to modify the items in the list
    {
        const QList<int> list = QList<int>() << 1 << 2 << 3;;

        QtConcurrent::blockingMap(list, MultiplyBy2InPlace());
        QtConcurrent::blockingMap(list, multiplyBy2InPlace);
        QtConcurrent::blockingMap(list, multiplyBy2InPlace);

        const QList<Number> numberList = QList<Number>() << 1 << 2 << 3;
        QtConcurrent::blockingMap(numberList, &Number::multiplyBy2);
    }
#endif
}

int multiplyBy2(int x)
{
    int y = x * 2;
    return y;
}

class MultiplyBy2
{
public:
    int operator()(int x) const
    {
        int y = x * 2;
        return y;
    }
};

double intToDouble(int x)
{
    return double(x);
}

class IntToDouble
{
public:
    double operator()(int x) const
    {
        return double(x);
    }
};

int stringToInt(const QString &string)
{
    return string.toInt();
}

class StringToInt
{
public:
    int operator()(const QString &string) const
    {
        return string.toInt();
    }
};

#define CHECK_FAIL(message) \
do {\
    if (QTest::currentTestFailed())\
        QFAIL("failed one line above on " message);\
} while (false)

template <typename SourceObject, typename ResultObject, typename MapObject>
void testMapped(const QList<SourceObject> &sourceObjectList, const QList<ResultObject> &expectedResult, MapObject mapObject)
{
    const QList<ResultObject> result1 = QtConcurrent::mapped(
                sourceObjectList, mapObject).results();
    QCOMPARE(result1, expectedResult);

    const QList<ResultObject> result2 = QtConcurrent::mapped(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject).results();
    QCOMPARE(result2, expectedResult);

    const QList<ResultObject> result3 = QtConcurrent::blockingMapped(
                sourceObjectList, mapObject);
    QCOMPARE(result3, expectedResult);

    const QList<ResultObject> result4 = QtConcurrent::blockingMapped<QList<ResultObject>>(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject);
    QCOMPARE(result4, expectedResult);
}

void tst_QtConcurrentMap::mapped()
{
    const QList<int> intList {1, 2, 3};
    const QList<Number> numberList {1, 2, 3};
    const QList<QString> stringList {"1", "2", "3"};
    const QList<double> doubleList {1.0, 2.0, 3.0};

    const QList<int> intListMultipiedBy2 {2, 4, 6};
    const QList<Number> numberListMultipiedBy2 {2, 4, 6};

    auto lambdaMultiplyBy2 = [](int x) {
        return 2 * x;
    };
    auto lambdaIntToDouble = [](int x) {
        return double(x);
    };
    auto lambdaStringToInt = [](const QString &string) {
        return string.toInt();
    };

    testMapped(intList, intListMultipiedBy2, MultiplyBy2());
    CHECK_FAIL("functor");
    testMapped(intList, intListMultipiedBy2, multiplyBy2);
    CHECK_FAIL("function");
    testMapped(numberList, numberListMultipiedBy2, &Number::multipliedBy2);
    CHECK_FAIL("member");
    testMapped(intList, intListMultipiedBy2, lambdaMultiplyBy2);
    CHECK_FAIL("lambda");

    // change the value_type, same container
    testMapped(intList, doubleList, IntToDouble());
    CHECK_FAIL("functor");
    testMapped(intList, doubleList, intToDouble);
    CHECK_FAIL("function");
    testMapped(numberList, stringList, &Number::toString);
    CHECK_FAIL("member");
    testMapped(intList, doubleList, lambdaIntToDouble);
    CHECK_FAIL("lambda");

    // change the value_type
    testMapped(stringList, intList, StringToInt());
    CHECK_FAIL("functor");
    testMapped(stringList, intList, stringToInt);
    CHECK_FAIL("function");
    testMapped(numberList, intList, &Number::toInt);
    CHECK_FAIL("member");
    testMapped(stringList, intList, lambdaStringToInt);
    CHECK_FAIL("lambda");

    // not allowed: const member function where all arguments have default values
#if 0
    testMapped(stringList, intList, &QString::toInt);
    CHECK_FAIL("member");
#endif
}

static QSemaphore semaphore(1);
static QSet<QThread *> workingThreads;

void storeCurrentThread()
{
    semaphore.acquire();
    workingThreads.insert(QThread::currentThread());
    semaphore.release();
}

int threadCount()
{
    semaphore.acquire();
    const int count = workingThreads.size();
    semaphore.release();
    return count;
}

template <typename SourceObject, typename ResultObject, typename MapObject>
void testMappedThreadPool(QThreadPool *pool,
                          const QList<SourceObject> &sourceObjectList,
                          const QList<ResultObject> &expectedResult,
                          MapObject mapObject)
{
    const QList<ResultObject> result1 = QtConcurrent::mapped(pool,
                sourceObjectList, mapObject).results();
    QCOMPARE(result1, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const QList<ResultObject> result2 = QtConcurrent::mapped(pool,
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject).results();
    QCOMPARE(result2, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const QList<ResultObject> result3 = QtConcurrent::blockingMapped(pool,
                sourceObjectList, mapObject);
    QCOMPARE(result3, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const QList<ResultObject> result4 = QtConcurrent::blockingMapped<QList<ResultObject>>(pool,
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject);
    QCOMPARE(result4, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working
}

int multiplyBy3(int x)
{
    storeCurrentThread();
    return x * 3;
}

class MultiplyBy3
{
public:
    int operator()(int x) const
    {
        storeCurrentThread();
        return x * 3;
    }
};

void tst_QtConcurrentMap::mappedThreadPool()
{
    const QList<int> intList {1, 2, 3};
    const QList<int> intListMultipiedBy3 {3, 6, 9};

    auto lambdaMultiplyBy3 = [](int x) {
        storeCurrentThread();
        return x * 3;
    };

    QThreadPool pool;
    pool.setMaxThreadCount(1);
    QCOMPARE(semaphore.available(), 1);
    workingThreads.clear();

    testMappedThreadPool(&pool, intList, intListMultipiedBy3, MultiplyBy3());
    CHECK_FAIL("functor");
    testMappedThreadPool(&pool, intList, intListMultipiedBy3, multiplyBy3);
    CHECK_FAIL("function");
    testMappedThreadPool(&pool, intList, intListMultipiedBy3, lambdaMultiplyBy3);
    CHECK_FAIL("lambda");
}

int intSquare(int x)
{
    return x * x;
}

class IntSquare
{
public:
    int operator()(int x)
    {
        return x * x;
    }
};

Number numberSquare(Number x)
{
    return Number(x.toInt() * x.toInt());
}

class NumberSquare
{
public:
    Number operator()(Number x)
    {
        return Number(x.toInt() * x.toInt());
    }
};

template <typename SourceObject, typename ResultObject, typename MapObject, typename ReduceObject>
void testMappedReduced(const QList<SourceObject> &sourceObjectList, const ResultObject &expectedResult, MapObject mapObject, ReduceObject reduceObject)
{
    const ResultObject result1 = QtConcurrent::mappedReduced<ResultObject>(
                sourceObjectList, mapObject, reduceObject);
    QCOMPARE(result1, expectedResult);

    const ResultObject result2 = QtConcurrent::mappedReduced<ResultObject>(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject);
    QCOMPARE(result2, expectedResult);

    const ResultObject result3 = QtConcurrent::blockingMappedReduced<ResultObject>(
                sourceObjectList, mapObject, reduceObject);
    QCOMPARE(result3, expectedResult);

    const ResultObject result4 = QtConcurrent::blockingMappedReduced<ResultObject>(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject);
    QCOMPARE(result4, expectedResult);
}

template <typename SourceObject, typename ResultObject, typename MapObject, typename ReduceObject>
void testMappedReduced(const QList<SourceObject> &sourceObjectList, const ResultObject &expectedResult, MapObject mapObject, ReduceObject reduceObject, QtConcurrent::ReduceOptions options)
{
    const ResultObject result1 = QtConcurrent::mappedReduced(
                sourceObjectList, mapObject, reduceObject, options);
    QCOMPARE(result1, expectedResult);

    const ResultObject result2 = QtConcurrent::mappedReduced(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, options);
    QCOMPARE(result2, expectedResult);

    const ResultObject result3 = QtConcurrent::blockingMappedReduced(
                sourceObjectList, mapObject, reduceObject, options);
    QCOMPARE(result3, expectedResult);

    const ResultObject result4 = QtConcurrent::blockingMappedReduced(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, options);
    QCOMPARE(result4, expectedResult);
}

void tst_QtConcurrentMap::mappedReduced()
{
    const QList<int> intList {1, 2, 3};
    const QList<int> intListOfSquares {1, 4, 9};
    const QList<Number> numberList {1, 2, 3};
    const int sum = 6;
    const int sumOfSquares = 14;

    void (QVector<int>::*push_back)(const int &) = &QVector<int>::push_back;

    auto lambdaSquare = [](int x) {
        return x * x;
    };
    auto lambdaSumReduce = [](int &sum, int x) {
        sum += x;
    };

    // FUNCTOR-other
    testMappedReduced(intList, sumOfSquares, IntSquare(), IntSumReduce());
    CHECK_FAIL("functor-functor");
    testMappedReduced(intList, sumOfSquares, IntSquare(), intSumReduce);
    CHECK_FAIL("functor-function");
    testMappedReduced(intList, intListOfSquares, IntSquare(), push_back, OrderedReduce);
    CHECK_FAIL("functor-member");
    testMappedReduced(intList, sumOfSquares, IntSquare(), lambdaSumReduce);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReduced(intList, sumOfSquares, intSquare, IntSumReduce());
    CHECK_FAIL("function-functor");
    testMappedReduced(intList, sumOfSquares, intSquare, intSumReduce);
    CHECK_FAIL("function-function");
    testMappedReduced(intList, intListOfSquares, intSquare, push_back, OrderedReduce);
    CHECK_FAIL("function-member");
    testMappedReduced(intList, sumOfSquares, intSquare, lambdaSumReduce);
    CHECK_FAIL("function-lambda");

    // MEMBER-other
    testMappedReduced(numberList, sum, &Number::toInt, IntSumReduce());
    CHECK_FAIL("member-functor");
    testMappedReduced(numberList, sum, &Number::toInt, intSumReduce);
    CHECK_FAIL("member-function");
    testMappedReduced(numberList, intList, &Number::toInt, push_back, OrderedReduce);
    CHECK_FAIL("member-member");
    testMappedReduced(numberList, sum, &Number::toInt, lambdaSumReduce);
    CHECK_FAIL("member-lambda");

    // LAMBDA-other
    testMappedReduced(intList, sumOfSquares, lambdaSquare, IntSumReduce());
    CHECK_FAIL("lambda-functor");
    testMappedReduced(intList, sumOfSquares, lambdaSquare, intSumReduce);
    CHECK_FAIL("lambda-function");
    testMappedReduced(intList, intListOfSquares, lambdaSquare, push_back, OrderedReduce);
    CHECK_FAIL("lambda-member");
    testMappedReduced(intList, sumOfSquares, lambdaSquare, lambdaSumReduce);
    CHECK_FAIL("lambda-lambda");
}

template <typename SourceObject, typename ResultObject, typename MapObject, typename ReduceObject>
void testMappedReducedThreadPool(QThreadPool *pool,
                                 const QList<SourceObject> &sourceObjectList,
                                 const ResultObject &expectedResult,
                                 MapObject mapObject,
                                 ReduceObject reduceObject)
{
    const ResultObject result1 = QtConcurrent::mappedReduced<ResultObject>(pool,
                sourceObjectList, mapObject, reduceObject);
    QCOMPARE(result1, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result2 = QtConcurrent::mappedReduced<ResultObject>(pool,
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject);
    QCOMPARE(result2, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result3 = QtConcurrent::blockingMappedReduced<ResultObject>(pool,
                sourceObjectList, mapObject, reduceObject);
    QCOMPARE(result3, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result4 = QtConcurrent::blockingMappedReduced<ResultObject>(pool,
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject);
    QCOMPARE(result4, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working
}

int intCube(int x)
{
    storeCurrentThread();
    return x * x * x;
}

class IntCube
{
public:
    int operator()(int x)
    {
        storeCurrentThread();
        return x * x * x;
    }
};

void tst_QtConcurrentMap::mappedReducedThreadPool()
{
    const QList<int> intList {1, 2, 3};
    const int sumOfCubes = 36;

    auto lambdaCube = [](int x) {
        return x * x * x;
    };
    auto lambdaSumReduce = [](int &sum, int x) {
        sum += x;
    };

    QThreadPool pool;
    pool.setMaxThreadCount(1);
    QCOMPARE(semaphore.available(), 1);
    workingThreads.clear();

    // FUNCTOR-other
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, IntCube(), IntSumReduce());
    CHECK_FAIL("functor-functor");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, IntCube(), intSumReduce);
    CHECK_FAIL("functor-function");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, IntCube(), lambdaSumReduce);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, intCube, IntSumReduce());
    CHECK_FAIL("function-functor");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, intCube, intSumReduce);
    CHECK_FAIL("function-function");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, intCube, lambdaSumReduce);
    CHECK_FAIL("function-lambda");

    // LAMBDA-other
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, lambdaCube, IntSumReduce());
    CHECK_FAIL("lambda-functor");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, lambdaCube, intSumReduce);
    CHECK_FAIL("lambda-function");
    testMappedReducedThreadPool(&pool, intList, sumOfCubes, lambdaCube, lambdaSumReduce);
    CHECK_FAIL("lambda-lambda");
}

void tst_QtConcurrentMap::mappedReducedDifferentType()
{
    const QList<int> intList {1, 2, 3};
    const QList<Number> numberList {1, 2, 3};
    const int sumOfSquares = 14;

    auto lambdaSquare = [](Number x) {
        return Number(x.toInt() * x.toInt());
    };
    auto lambdaSumReduce = [](int &sum, Number x) {
        sum += x.toInt();
    };

    // Test the case where reduce function of the form:
    // V function(T &result, const U &intermediate)
    // has T and U types different.

    // FUNCTOR-other
    testMappedReduced(intList, sumOfSquares, NumberSquare(), NumberSumReduce());
    CHECK_FAIL("functor-functor");
    testMappedReduced(intList, sumOfSquares, NumberSquare(), numberSumReduce);
    CHECK_FAIL("functor-function");
    testMappedReduced(intList, sumOfSquares, NumberSquare(), lambdaSumReduce);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReduced(intList, sumOfSquares, numberSquare, NumberSumReduce());
    CHECK_FAIL("function-functor");
    testMappedReduced(intList, sumOfSquares, numberSquare, numberSumReduce);
    CHECK_FAIL("function-function");
    testMappedReduced(intList, sumOfSquares, numberSquare, lambdaSumReduce);
    CHECK_FAIL("function-lambda");

    // MEMBER-other
    testMappedReduced(numberList, sumOfSquares, &Number::squared, NumberSumReduce());
    CHECK_FAIL("member-functor");
    testMappedReduced(numberList, sumOfSquares, &Number::squared, numberSumReduce);
    CHECK_FAIL("member-function");
    testMappedReduced(numberList, sumOfSquares, &Number::squared, lambdaSumReduce);
    CHECK_FAIL("member-lambda");

    // LAMBDA-other
    testMappedReduced(intList, sumOfSquares, lambdaSquare, NumberSumReduce());
    CHECK_FAIL("lambda-functor");
    testMappedReduced(intList, sumOfSquares, lambdaSquare, numberSumReduce);
    CHECK_FAIL("lambda-function");
    testMappedReduced(intList, sumOfSquares, lambdaSquare, lambdaSumReduce);
    CHECK_FAIL("lambda-lambda");
}

template <typename SourceObject, typename ResultObject, typename InitialObject, typename MapObject, typename ReduceObject>
void testMappedReducedInitialValue(const QList<SourceObject> &sourceObjectList,
                                   const ResultObject &expectedResult,
                                   MapObject mapObject,
                                   ReduceObject reduceObject,
                                   InitialObject &&initialObject)
{
    const ResultObject result1 = QtConcurrent::mappedReduced<ResultObject>(
                sourceObjectList, mapObject, reduceObject, initialObject);
    QCOMPARE(result1, expectedResult);

    const ResultObject result2 = QtConcurrent::mappedReduced<ResultObject>(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, initialObject);
    QCOMPARE(result2, expectedResult);

    const ResultObject result3 = QtConcurrent::blockingMappedReduced<ResultObject>(
                sourceObjectList, mapObject, reduceObject, initialObject);
    QCOMPARE(result3, expectedResult);

    const ResultObject result4 = QtConcurrent::blockingMappedReduced<ResultObject>(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, initialObject);
    QCOMPARE(result4, expectedResult);
}

template <typename SourceObject, typename ResultObject, typename InitialObject, typename MapObject, typename ReduceObject>
void testMappedReducedInitialValue(const QList<SourceObject> &sourceObjectList,
                                   const ResultObject &expectedResult,
                                   MapObject mapObject,
                                   ReduceObject reduceObject,
                                   InitialObject &&initialObject,
                                   QtConcurrent::ReduceOptions options)
{
    const ResultObject result1 = QtConcurrent::mappedReduced(
                sourceObjectList, mapObject, reduceObject, initialObject, options);
    QCOMPARE(result1, expectedResult);

    const ResultObject result2 = QtConcurrent::mappedReduced(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, initialObject, options);
    QCOMPARE(result2, expectedResult);

    const ResultObject result3 = QtConcurrent::blockingMappedReduced(
                sourceObjectList, mapObject, reduceObject, initialObject, options);
    QCOMPARE(result3, expectedResult);

    const ResultObject result4 = QtConcurrent::blockingMappedReduced(
                sourceObjectList.constBegin(), sourceObjectList.constEnd(), mapObject, reduceObject, initialObject, options);
    QCOMPARE(result4, expectedResult);
}

void tst_QtConcurrentMap::mappedReducedInitialValue()
{
    // This is a copy of tst_QtConcurrentMap::mappedReduced with the initial value parameter added

    const QList<int> intList {1, 2, 3};
    const QList<int> intListInitial {10};
    const QList<int> intListAppended {10, 1, 2, 3};
    const QList<int> intListSquaresAppended {10, 1, 4, 9};
    const QList<Number> numberList {1, 2, 3};
    const int sum = 16;
    const int sumOfSquares = 24;
    const int intInitial = 10;

    void (QVector<int>::*push_back)(const int &) = &QVector<int>::push_back;

    auto lambdaSquare = [](int x) {
        return x * x;
    };
    auto lambdaSumReduce = [](int &sum, int x) {
        sum += x;
    };

    // FUNCTOR-other
    testMappedReducedInitialValue(intList, sumOfSquares, IntSquare(), IntSumReduce(), intInitial);
    CHECK_FAIL("functor-functor");
    testMappedReducedInitialValue(intList, sumOfSquares, IntSquare(), intSumReduce, intInitial);
    CHECK_FAIL("functor-function");
    testMappedReducedInitialValue(intList, intListSquaresAppended, IntSquare(), push_back, intListInitial, OrderedReduce);
    CHECK_FAIL("functor-member");
    testMappedReducedInitialValue(intList, sumOfSquares, IntSquare(), lambdaSumReduce, intInitial);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReducedInitialValue(intList, sumOfSquares, intSquare, IntSumReduce(), intInitial);
    CHECK_FAIL("function-functor");
    testMappedReducedInitialValue(intList, sumOfSquares, intSquare, intSumReduce, intInitial);
    CHECK_FAIL("function-function");
    testMappedReducedInitialValue(intList, intListSquaresAppended, intSquare, push_back, intListInitial, OrderedReduce);
    CHECK_FAIL("function-member");
    testMappedReducedInitialValue(intList, sumOfSquares, intSquare, lambdaSumReduce, intInitial);
    CHECK_FAIL("function-lambda");

    // MEMBER-other
    testMappedReducedInitialValue(numberList, sum, &Number::toInt, IntSumReduce(), intInitial);
    CHECK_FAIL("member-functor");
    testMappedReducedInitialValue(numberList, sum, &Number::toInt, intSumReduce, intInitial);
    CHECK_FAIL("member-function");
    testMappedReducedInitialValue(numberList, intListAppended, &Number::toInt, push_back, intListInitial, OrderedReduce);
    CHECK_FAIL("member-member");
    testMappedReducedInitialValue(numberList, sum, &Number::toInt, lambdaSumReduce, intInitial);
    CHECK_FAIL("member-lambda");

    // LAMBDA-other
    testMappedReducedInitialValue(intList, sumOfSquares, lambdaSquare, IntSumReduce(), intInitial);
    CHECK_FAIL("lambda-functor");
    testMappedReducedInitialValue(intList, sumOfSquares, lambdaSquare, intSumReduce, intInitial);
    CHECK_FAIL("lambda-function");
    testMappedReducedInitialValue(intList, intListSquaresAppended, lambdaSquare, push_back, intListInitial, OrderedReduce);
    CHECK_FAIL("lambda-member");
    testMappedReducedInitialValue(intList, sumOfSquares, lambdaSquare, lambdaSumReduce, intInitial);
    CHECK_FAIL("lambda-lambda");
}

template <typename SourceObject, typename ResultObject, typename InitialObject, typename MapObject, typename ReduceObject>
void testMappedReducedInitialValueThreadPool(QThreadPool *pool,
                                             const QList<SourceObject> &sourceObjectList,
                                             const ResultObject &expectedResult,
                                             MapObject mapObject,
                                             ReduceObject reduceObject,
                                             InitialObject &&initialObject)
{
    const ResultObject result1 = QtConcurrent::mappedReduced<ResultObject>(
                pool, sourceObjectList, mapObject, reduceObject, initialObject);
    QCOMPARE(result1, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result2 = QtConcurrent::mappedReduced<ResultObject>(
                pool, sourceObjectList.constBegin(), sourceObjectList.constEnd(),
                mapObject, reduceObject, initialObject);
    QCOMPARE(result2, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result3 = QtConcurrent::blockingMappedReduced<ResultObject>(
                pool, sourceObjectList, mapObject, reduceObject, initialObject);
    QCOMPARE(result3, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working

    const ResultObject result4 = QtConcurrent::blockingMappedReduced<ResultObject>(
                pool, sourceObjectList.constBegin(), sourceObjectList.constEnd(),
                mapObject, reduceObject, initialObject);
    QCOMPARE(result4, expectedResult);
    QCOMPARE(threadCount(), 1); // ensure the only one thread was working
}

void tst_QtConcurrentMap::mappedReducedInitialValueThreadPool()
{
    // This is a copy of tst_QtConcurrentMap::mappedReduced with the initial value parameter added

    const QList<int> intList {1, 2, 3};
    const int sumOfCubes = 46;
    const int intInitial = 10;

    auto lambdaCube = [](int x) {
        return x * x * x;
    };
    auto lambdaSumReduce = [](int &sum, int x) {
        sum += x;
    };

    QThreadPool pool;
    pool.setMaxThreadCount(1);
    QCOMPARE(semaphore.available(), 1);
    workingThreads.clear();

    // FUNCTOR-other
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, IntCube(),
                                            IntSumReduce(), intInitial);
    CHECK_FAIL("functor-functor");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, IntCube(),
                                            intSumReduce, intInitial);
    CHECK_FAIL("functor-function");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, IntCube(),
                                            lambdaSumReduce, intInitial);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, intCube,
                                            IntSumReduce(), intInitial);
    CHECK_FAIL("function-functor");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, intCube,
                                            intSumReduce, intInitial);
    CHECK_FAIL("function-function");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, intCube,
                                            lambdaSumReduce, intInitial);
    CHECK_FAIL("function-lambda");

    // LAMBDA-other
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, lambdaCube,
                                            IntSumReduce(), intInitial);
    CHECK_FAIL("lambda-functor");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, lambdaCube,
                                            intSumReduce, intInitial);
    CHECK_FAIL("lambda-function");
    testMappedReducedInitialValueThreadPool(&pool, intList, sumOfCubes, lambdaCube,
                                            lambdaSumReduce, intInitial);
    CHECK_FAIL("lambda-lambda");
}

void tst_QtConcurrentMap::mappedReducedDifferentTypeInitialValue()
{
    // This is a copy of tst_QtConcurrentMap::mappedReducedDifferentType
    // with the initial value parameter added

    const QList<Number> numberList {1, 2, 3};
    const int sumOfSquares = 24;
    const int intInitial = 10;

    auto lambdaSquare = [](Number x) {
        return Number(x.toInt() * x.toInt());
    };
    auto lambdaSumReduce = [](int &sum, Number x) {
        sum += x.toInt();
    };

    // FUNCTOR-other
    testMappedReducedInitialValue(numberList, sumOfSquares, NumberSquare(), NumberSumReduce(), intInitial);
    CHECK_FAIL("functor-functor");
    testMappedReducedInitialValue(numberList, sumOfSquares, NumberSquare(), numberSumReduce, intInitial);
    CHECK_FAIL("functor-function");
    testMappedReducedInitialValue(numberList, sumOfSquares, NumberSquare(), lambdaSumReduce, intInitial);
    CHECK_FAIL("functor-lambda");

    // FUNCTION-other
    testMappedReducedInitialValue(numberList, sumOfSquares, numberSquare, NumberSumReduce(), intInitial);
    CHECK_FAIL("function-functor");
    testMappedReducedInitialValue(numberList, sumOfSquares, numberSquare, numberSumReduce, intInitial);
    CHECK_FAIL("function-function");
    testMappedReducedInitialValue(numberList, sumOfSquares, numberSquare, lambdaSumReduce, intInitial);
    CHECK_FAIL("function-lambda");

    // MEMBER-other
    testMappedReducedInitialValue(numberList, sumOfSquares, &Number::squared, NumberSumReduce(), intInitial);
    CHECK_FAIL("member-functor");
    testMappedReducedInitialValue(numberList, sumOfSquares, &Number::squared, numberSumReduce, intInitial);
    CHECK_FAIL("member-function");
    testMappedReducedInitialValue(numberList, sumOfSquares, &Number::squared, lambdaSumReduce, intInitial);
    CHECK_FAIL("member-lambda");

    // LAMBDA-other
    testMappedReducedInitialValue(numberList, sumOfSquares, lambdaSquare, NumberSumReduce(), intInitial);
    CHECK_FAIL("lambda-functor");
    testMappedReducedInitialValue(numberList, sumOfSquares, lambdaSquare, numberSumReduce, intInitial);
    CHECK_FAIL("lambda-function");
    testMappedReducedInitialValue(numberList, sumOfSquares, lambdaSquare, lambdaSumReduce, intInitial);
    CHECK_FAIL("lambda-lambda");
}

int sleeper(int val)
{
    QTest::qSleep(100);
    return val;
}

void tst_QtConcurrentMap::assignResult()
{
    const QList<int> startList = QList<int>() << 0 << 1 << 2;
    QList<int> list = QtConcurrent::blockingMapped(startList, sleeper);
    QCOMPARE(list.at(0), 0);
    QCOMPARE(list.at(1), 1);
}

int fnConst(const int &i)
{
    return i;
}

int fn(int &i)
{
    return i;
}

int fnConstNoExcept(const int &i) noexcept
{
    return i;
}

int fnNoExcept(int &i) noexcept
{
    return i;
}

QString changeTypeConst(const int &)
{
    return QString();
}

QString changeType(int &)
{
    return QString();
}

QString changeTypeConstNoExcept(const int &) noexcept
{
    return QString();
}

QString changeTypeNoExcept(int &) noexcept
{
    return QString();
}

int changeTypeQStringListConst(const QStringList &)
{
    return 0;
}

int changeTypeQStringList(QStringList &)
{
    return 0;
}

int changeTypeQStringListConstNoExcept(const QStringList &) noexcept
{
    return 0;
}

int changeTypeQStringListNoExcept(QStringList &) noexcept
{
    return 0;
}

class MemFnTester
{
public:
    MemFnTester() {}

    MemFnTester fn()
    {
        return MemFnTester();
    }

    MemFnTester fnConst() const
    {
        return MemFnTester();
    }

    QString changeType()
    {
        return QString();
    }

    QString changeTypeConst() const
    {
        return QString();
    }

    MemFnTester fnNoExcept() noexcept
    {
        return MemFnTester();
    }

    MemFnTester fnConstNoExcept() const noexcept
    {
        return MemFnTester();
    }

    QString changeTypeNoExcept() noexcept
    {
        return QString();
    }

    QString changeTypeConstNoExcept() const noexcept
    {
        return QString();
    }
};

Q_DECLARE_METATYPE(QVector<MemFnTester>);

void tst_QtConcurrentMap::functionOverloads()
{
    QList<int> intList;
    const QList<int> constIntList;
    QList<MemFnTester> classList;
    const QList<MemFnTester> constMemFnTesterList;

    QtConcurrent::mapped(intList, fnConst);
    QtConcurrent::mapped(constIntList, fnConst);
    QtConcurrent::mapped(classList, &MemFnTester::fnConst);
    QtConcurrent::mapped(constMemFnTesterList, &MemFnTester::fnConst);

    QtConcurrent::blockingMapped<QVector<int> >(intList, fnConst);
    QtConcurrent::blockingMapped<QVector<int> >(constIntList, fnConst);
    QtConcurrent::blockingMapped<QVector<MemFnTester> >(classList, &MemFnTester::fnConst);
    QtConcurrent::blockingMapped<QVector<MemFnTester> >(constMemFnTesterList, &MemFnTester::fnConst);

    QtConcurrent::blockingMapped<QList<QString> >(intList, changeTypeConst);
    QtConcurrent::blockingMapped<QList<QString> >(constIntList, changeTypeConst);
    QtConcurrent::blockingMapped<QList<QString> >(classList, &MemFnTester::changeTypeConst);
    QtConcurrent::blockingMapped<QList<QString> >(constMemFnTesterList, &MemFnTester::changeTypeConst);
}

void tst_QtConcurrentMap::noExceptFunctionOverloads()
{
    QList<int> intList;
    const QList<int> constIntList;
    QList<MemFnTester> classList;
    const QList<MemFnTester> constMemFnTesterList;

    QtConcurrent::mapped(intList, fnConstNoExcept);
    QtConcurrent::mapped(constIntList, fnConstNoExcept);
    QtConcurrent::mapped(classList, &MemFnTester::fnConstNoExcept);
    QtConcurrent::mapped(constMemFnTesterList, &MemFnTester::fnConstNoExcept);

    QtConcurrent::blockingMapped<QVector<int> >(intList, fnConstNoExcept);
    QtConcurrent::blockingMapped<QVector<int> >(constIntList, fnConstNoExcept);
    QtConcurrent::blockingMapped<QVector<MemFnTester> >(classList, &MemFnTester::fnConstNoExcept);
    QtConcurrent::blockingMapped<QVector<MemFnTester> >(constMemFnTesterList, &MemFnTester::fnConstNoExcept);

    QtConcurrent::blockingMapped<QList<QString> >(intList, changeTypeConstNoExcept);
    QtConcurrent::blockingMapped<QList<QString> >(constIntList, changeTypeConstNoExcept);
    QtConcurrent::blockingMapped<QList<QString> >(classList, &MemFnTester::changeTypeConstNoExcept);
    QtConcurrent::blockingMapped<QList<QString> >(constMemFnTesterList, &MemFnTester::changeTypeConstNoExcept);
}

QAtomicInt currentInstanceCount;
QAtomicInt peakInstanceCount;
class InstanceCounter
{
public:
    inline InstanceCounter()
    { currentInstanceCount.fetchAndAddRelaxed(1); updatePeak(); }
    inline ~InstanceCounter()
    { currentInstanceCount.fetchAndAddRelaxed(-1);}
    inline InstanceCounter(const InstanceCounter &)
    { currentInstanceCount.fetchAndAddRelaxed(1); updatePeak(); }

    void updatePeak()
    {
        forever {
            const int localPeak = peakInstanceCount.loadRelaxed();
            const int localCurrent = currentInstanceCount.loadRelaxed();
            if (localCurrent <= localPeak)
                break;
            if (peakInstanceCount.testAndSetOrdered(localPeak, localCurrent))
                break;
        }
    }
};

InstanceCounter slowMap(const InstanceCounter &in)
{
    QTest::qSleep(2);
    return in;
}

InstanceCounter fastMap(const InstanceCounter &in)
{
    QTest::qSleep(QRandomGenerator::global()->bounded(2) + 1);
    return in;
}

void slowReduce(int &result, const InstanceCounter&)
{
    QTest::qSleep(QRandomGenerator::global()->bounded(4) + 1);
    ++result;
}

void fastReduce(int &result, const InstanceCounter&)
{
    ++result;
}

void tst_QtConcurrentMap::throttling()
{
    const int itemcount = 100;
    const int allowedTemporaries = QThread::idealThreadCount() * 40;

    {
        currentInstanceCount.storeRelaxed(0);
        peakInstanceCount.storeRelaxed(0);

        QList<InstanceCounter> instances;
        for (int i = 0; i < itemcount; ++i)
            instances.append(InstanceCounter());

        QCOMPARE(currentInstanceCount.loadRelaxed(), itemcount);

        int results = QtConcurrent::blockingMappedReduced(instances, slowMap, fastReduce);
        QCOMPARE(results, itemcount);
        QCOMPARE(currentInstanceCount.loadRelaxed(), itemcount);
        QVERIFY(peakInstanceCount.loadRelaxed() < itemcount + allowedTemporaries);
    }

    {
        QCOMPARE(currentInstanceCount.loadRelaxed(), 0);
        peakInstanceCount.storeRelaxed(0);

        QList<InstanceCounter> instances;
        for (int i = 0; i < itemcount; ++i)
            instances.append(InstanceCounter());

        QCOMPARE(currentInstanceCount.loadRelaxed(), itemcount);
        int results = QtConcurrent::blockingMappedReduced(instances, fastMap, slowReduce);

        QCOMPARE(results, itemcount);
        QCOMPARE(currentInstanceCount.loadRelaxed(), itemcount);
        QVERIFY(peakInstanceCount.loadRelaxed() < itemcount + allowedTemporaries);
    }
}

#ifndef QT_NO_EXCEPTIONS
void throwMapper(int &e)
{
    Q_UNUSED(e);
    throw QException();
}

void tst_QtConcurrentMap::exceptions()
{
    bool caught = false;
    try  {
        QList<int> list = QList<int>() << 1 << 2 << 3;
        QtConcurrent::map(list, throwMapper).waitForFinished();
    } catch (const QException &) {
        caught = true;
    }
    if (!caught)
        QFAIL("did not get exception");
}
#endif

int mapper(const int &i)
{
    QTest::qWait(1);
    return i;
}

void tst_QtConcurrentMap::incrementalResults()
{
    const int count = 200;
    QList<int> ints;
    for (int i=0; i < count; ++i)
        ints << i;

    QFuture<int> future = QtConcurrent::mapped(ints, mapper);

    QList<int> results;

    while (future.isFinished() == false) {
        for (int i = 0; i < future.resultCount(); ++i) {
            results += future.resultAt(i);
        }

        QTest::qWait(1);
    }

    QCOMPARE(future.isFinished(), true);
    QCOMPARE(future.resultCount(), count);
    QCOMPARE(future.results().count(), count);
}

/*
    Test that mapped does not cause deep copies when holding
    references to Qt containers.
*/
void tst_QtConcurrentMap::noDetach()
{
    {
        QList<int> l = QList<int>() << 1;
        QVERIFY(l.isDetached());

        QList<int> ll = l;
        QVERIFY(!l.isDetached());

        QtConcurrent::mapped(l, mapper).waitForFinished();

        QVERIFY(!l.isDetached());
        QVERIFY(!ll.isDetached());

        QtConcurrent::mappedReduced(l, mapper, intSumReduce).waitForFinished();

        QVERIFY(!l.isDetached());
        QVERIFY(!ll.isDetached());

        QtConcurrent::map(l, multiplyBy2Immutable).waitForFinished();
        QVERIFY(l.isDetached());
        QVERIFY(ll.isDetached());
    }
    {
        const QList<int> l = QList<int>() << 1;
        QVERIFY(l.isDetached());

        const QList<int> ll = l;
        QVERIFY(!l.isDetached());

        QtConcurrent::mapped(l, mapper).waitForFinished();

        QVERIFY(!l.isDetached());
        QVERIFY(!ll.isDetached());

        QtConcurrent::mappedReduced(l, mapper, intSumReduce).waitForFinished();

        QVERIFY(!l.isDetached());
        QVERIFY(!ll.isDetached());
    }

}

void tst_QtConcurrentMap::stlContainers()
{
    std::vector<int> vector;
    vector.push_back(1);
    vector.push_back(2);

    std::vector<int> vector2 =  QtConcurrent::blockingMapped<std::vector<int> >(vector, mapper);
    QCOMPARE(vector2.size(), (std::vector<int>::size_type)(2));

    std::list<int> list;
    list.push_back(1);
    list.push_back(2);

    std::list<int> list2 =  QtConcurrent::blockingMapped<std::list<int> >(list, mapper);
    QCOMPARE(list2.size(), (std::vector<int>::size_type)(2));

    QtConcurrent::mapped(list, mapper).waitForFinished();

    QtConcurrent::blockingMap(list, multiplyBy2Immutable);
}

void tst_QtConcurrentMap::stlContainersLambda()
{
    std::vector<int> vector;
    vector.push_back(1);
    vector.push_back(2);

    std::vector<int> vector2 = QtConcurrent::blockingMapped<std::vector<int> >(vector,
        [](const int &i) {
            return mapper(i);
        }
    );
    QCOMPARE(vector2.size(), (std::vector<int>::size_type)(2));

    std::list<int> list;
    list.push_back(1);
    list.push_back(2);

    std::list<int> list2 = QtConcurrent::blockingMapped<std::list<int> >(list,
        [](const int &i) {
            return mapper(i);
        }
    );
    QCOMPARE(list2.size(), (std::vector<int>::size_type)(2));

    QtConcurrent::mapped(list, [](const int &i) { return mapper(i); }).waitForFinished();

    QtConcurrent::blockingMap(list, [](int x) { x *= 2; });
}

InstanceCounter ic_fn(const InstanceCounter & ic)
{
    return InstanceCounter(ic);
}

// Verify that held results are deleted when a future is
// assigned over with operator ==
void tst_QtConcurrentMap::qFutureAssignmentLeak()
{
    currentInstanceCount.storeRelaxed(0);
    peakInstanceCount.storeRelaxed(0);
    QFuture<InstanceCounter> future;
    {
        QList<InstanceCounter> list;
        for (int i=0;i<1000;++i)
            list += InstanceCounter();
        future = QtConcurrent::mapped(list, ic_fn);
        future.waitForFinished();

        future = QtConcurrent::mapped(list, ic_fn);
        future.waitForFinished();

        future = QtConcurrent::mapped(list, ic_fn);
        future.waitForFinished();
    }

    // Use QTRY_COMPARE because QtConcurrent::ThreadEngine::asynchronousFinish()
    // deletes its internals after signaling finished, so it might still be holding
    // on to copies of InstanceCounter for a short while.
    QTRY_COMPARE(currentInstanceCount.loadRelaxed(), 1000);
    future = QFuture<InstanceCounter>();
    QTRY_COMPARE(currentInstanceCount.loadRelaxed(), 0);
}

inline void increment(int &num)
{
    ++num;
}

inline int echo(const int &num)
{
    return num;
}

void add(int &result, const int &sum)
{
    result += sum;
}

void tst_QtConcurrentMap::stressTest()
{
    const int listSize = 1000;
    const int sum = (listSize - 1) * (listSize / 2);
    QList<int> list;


    for (int i = 0; i < listSize; ++i) {
        list.append(i);
    }

    for (int i =0 ; i < 100; ++i) {
        QList<int> result = QtConcurrent::blockingMapped(list, echo);
        for (int j = 0; j < listSize; ++j)
            QCOMPARE(result.at(j), j);
    }

    for (int i = 0 ; i < 100; ++i) {
        int result = QtConcurrent::blockingMappedReduced(list, echo, add);
        QCOMPARE(result, sum);
    }

    for (int i = 0 ; i < 100; ++i) {
        QtConcurrent::map(list, increment).waitForFinished();
        for (int j = 0; j < listSize; ++j)
            QCOMPARE(list.at(j), i + j + 1);
    }
}

struct LockedCounter
{
    LockedCounter(QMutex *mutex, QAtomicInt *ai)
        : mtx(mutex),
          ref(ai) {}

    int operator()(int x)
    {
        QMutexLocker locker(mtx);
        ref->ref();
        return ++x;
    }

    QMutex *mtx;
    QAtomicInt *ref;
};

// The Thread engine holds the last reference
// to the QFuture, so this should not leak
// or fail.
void tst_QtConcurrentMap::persistentResultTest()
{
    QFuture<void> voidFuture;
    QMutex mtx;
    QAtomicInt ref;
    LockedCounter lc(&mtx, &ref);
    QList<int> list;
    {
        list << 1 << 2 << 3;
        mtx.lock();
        QFuture<int> future = QtConcurrent::mapped(list
                                                   ,lc);
        voidFuture = future;
    }
    QCOMPARE(ref.loadAcquire(), 0);
    mtx.unlock(); // Unblock
    voidFuture.waitForFinished();
    QCOMPARE(ref.loadAcquire(), 3);
}

QTEST_MAIN(tst_QtConcurrentMap)
#include "tst_qtconcurrentmap.moc"
