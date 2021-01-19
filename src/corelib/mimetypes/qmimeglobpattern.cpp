/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qmimeglobpattern_p.h"

#if QT_CONFIG(regularexpression)
#include <QRegularExpression>
#endif
#include <QStringList>
#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QMimeGlobMatchResult
    \inmodule QtCore
    \brief The QMimeGlobMatchResult class accumulates results from glob matching.

    Handles glob weights, and preferring longer matches over shorter matches.
*/

void QMimeGlobMatchResult::addMatch(const QString &mimeType, int weight, const QString &pattern, int knownSuffixLength)
{
    if (m_allMatchingMimeTypes.contains(mimeType))
        return;
    // Is this a lower-weight pattern than the last match? Skip this match then.
    if (weight < m_weight) {
        m_allMatchingMimeTypes.append(mimeType);
        return;
    }
    bool replace = weight > m_weight;
    if (!replace) {
        // Compare the length of the match
        if (pattern.length() < m_matchingPatternLength)
            return; // too short, ignore
        else if (pattern.length() > m_matchingPatternLength) {
            // longer: clear any previous match (like *.bz2, when pattern is *.tar.bz2)
            replace = true;
        }
    }
    if (replace) {
        m_matchingMimeTypes.clear();
        // remember the new "longer" length
        m_matchingPatternLength = pattern.length();
        m_weight = weight;
    }
    if (!m_matchingMimeTypes.contains(mimeType)) {
        m_matchingMimeTypes.append(mimeType);
        if (replace)
            m_allMatchingMimeTypes.prepend(mimeType); // highest-weight first
        else
            m_allMatchingMimeTypes.append(mimeType);
        m_knownSuffixLength = knownSuffixLength;
    }
}

/*!
    \internal
    \class QMimeGlobPattern
    \inmodule QtCore
    \brief The QMimeGlobPattern class contains the glob pattern for file names for MIME type matching.

    \sa QMimeType, QMimeDatabase, QMimeMagicRuleMatcher, QMimeMagicRule
*/

bool QMimeGlobPattern::matchFileName(const QString &inputFilename) const
{
    // "Applications MUST match globs case-insensitively, except when the case-sensitive
    // attribute is set to true."
    // The constructor takes care of putting case-insensitive patterns in lowercase.
    const QString filename = m_caseSensitivity == Qt::CaseInsensitive ? inputFilename.toLower() : inputFilename;

    const int pattern_len = m_pattern.length();
    if (!pattern_len)
        return false;
    const int len = filename.length();

    const int starCount = m_pattern.count(QLatin1Char('*'));

    // Patterns like "*~", "*.extension"
    if (m_pattern[0] == QLatin1Char('*') && m_pattern.indexOf(QLatin1Char('[')) == -1 && starCount == 1)
    {
        if (len + 1 < pattern_len) return false;

        const QChar *c1 = m_pattern.unicode() + pattern_len - 1;
        const QChar *c2 = filename.unicode() + len - 1;
        int cnt = 1;
        while (cnt < pattern_len && *c1-- == *c2--)
            ++cnt;
        return cnt == pattern_len;
    }

    // Patterns like "README*" (well this is currently the only one like that...)
    if (starCount == 1 && m_pattern.at(pattern_len - 1) == QLatin1Char('*')) {
        if (len + 1 < pattern_len) return false;
        if (m_pattern.at(0) == QLatin1Char('*'))
            return filename.indexOf(QStringView{m_pattern}.mid(1, pattern_len - 2)) != -1;

        const QChar *c1 = m_pattern.unicode();
        const QChar *c2 = filename.unicode();
        int cnt = 1;
        while (cnt < pattern_len && *c1++ == *c2++)
           ++cnt;
        return cnt == pattern_len;
    }

    // Names without any wildcards like "README"
    if (m_pattern.indexOf(QLatin1Char('[')) == -1 && starCount == 0 && m_pattern.indexOf(QLatin1Char('?')))
        return (m_pattern == filename);

    // Other (quite rare) patterns, like "*.anim[1-9j]": use slow but correct method
#if QT_CONFIG(regularexpression)
    auto rx = QRegularExpression::fromWildcard(m_pattern);
    return rx.match(filename).hasMatch();
#else
    return false;
#endif
}

static bool isSimplePattern(const QString &pattern)
{
   // starts with "*.", has no other '*'
   return pattern.lastIndexOf(QLatin1Char('*')) == 0
      && pattern.length() > 1
      && pattern.at(1) == QLatin1Char('.') // (other dots are OK, like *.tar.bz2)
      // and contains no other special character
      && !pattern.contains(QLatin1Char('?'))
      && !pattern.contains(QLatin1Char('['))
      ;
}

static bool isFastPattern(const QString &pattern)
{
   // starts with "*.", has no other '*' and no other '.'
   return pattern.lastIndexOf(QLatin1Char('*')) == 0
      && pattern.lastIndexOf(QLatin1Char('.')) == 1
      // and contains no other special character
      && !pattern.contains(QLatin1Char('?'))
      && !pattern.contains(QLatin1Char('['))
      ;
}

void QMimeAllGlobPatterns::addGlob(const QMimeGlobPattern &glob)
{
    const QString &pattern = glob.pattern();
    Q_ASSERT(!pattern.isEmpty());

    // Store each patterns into either m_fastPatternDict (*.txt, *.html etc. with default weight 50)
    // or for the rest, like core.*, *.tar.bz2, *~, into highWeightPatternOffset (>50)
    // or lowWeightPatternOffset (<=50)

    if (glob.weight() == 50 && isFastPattern(pattern) && !glob.isCaseSensitive()) {
        // The bulk of the patterns is *.foo with weight 50 --> those go into the fast patterns hash.
        const QString extension = pattern.mid(2).toLower();
        QStringList &patterns = m_fastPatterns[extension]; // find or create
        if (!patterns.contains(glob.mimeType()))
            patterns.append(glob.mimeType());
    } else {
        if (glob.weight() > 50) {
            if (!m_highWeightGlobs.hasPattern(glob.mimeType(), glob.pattern()))
                m_highWeightGlobs.append(glob);
        } else {
            if (!m_lowWeightGlobs.hasPattern(glob.mimeType(), glob.pattern()))
                m_lowWeightGlobs.append(glob);
        }
    }
}

void QMimeAllGlobPatterns::removeMimeType(const QString &mimeType)
{
    for (auto &x : m_fastPatterns)
        x.removeAll(mimeType);
    m_highWeightGlobs.removeMimeType(mimeType);
    m_lowWeightGlobs.removeMimeType(mimeType);
}

void QMimeGlobPatternList::match(QMimeGlobMatchResult &result,
                                 const QString &fileName) const
{

    QMimeGlobPatternList::const_iterator it = this->constBegin();
    const QMimeGlobPatternList::const_iterator endIt = this->constEnd();
    for (; it != endIt; ++it) {
        const QMimeGlobPattern &glob = *it;
        if (glob.matchFileName(fileName)) {
            const QString pattern = glob.pattern();
            const int suffixLen = isSimplePattern(pattern) ? pattern.length() - 2 : 0;
            result.addMatch(glob.mimeType(), glob.weight(), pattern, suffixLen);
        }
    }
}

void QMimeAllGlobPatterns::matchingGlobs(const QString &fileName, QMimeGlobMatchResult &result) const
{
    // First try the high weight matches (>50), if any.
    m_highWeightGlobs.match(result, fileName);

    // Now use the "fast patterns" dict, for simple *.foo patterns with weight 50
    // (which is most of them, so this optimization is definitely worth it)
    const int lastDot = fileName.lastIndexOf(QLatin1Char('.'));
    if (lastDot != -1) { // if no '.', skip the extension lookup
        const int ext_len = fileName.length() - lastDot - 1;
        const QString simpleExtension = fileName.right(ext_len).toLower();
        // (toLower because fast patterns are always case-insensitive and saved as lowercase)

        const QStringList matchingMimeTypes = m_fastPatterns.value(simpleExtension);
        const QString simplePattern = QLatin1String("*.") + simpleExtension;
        for (const QString &mime : matchingMimeTypes)
            result.addMatch(mime, 50, simplePattern, simpleExtension.size());
        // Can't return yet; *.tar.bz2 has to win over *.bz2, so we need the low-weight mimetypes anyway,
        // at least those with weight 50.
    }

    // Finally, try the low weight matches (<=50)
    m_lowWeightGlobs.match(result, fileName);
}

void QMimeAllGlobPatterns::clear()
{
    m_fastPatterns.clear();
    m_highWeightGlobs.clear();
    m_lowWeightGlobs.clear();
}

QT_END_NAMESPACE
