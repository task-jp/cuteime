/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   cuteime                                                                  *
 *   Copyright (C) 2009-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "cuteimekeysequence.h"
#include "cuteimedebug.h"

#include <QKeySequence>

class CuteimeKeySequence::Private
{
public:
    enum PatchStatus
    {
        NotChecked,
        NotFound,
        Found,
    };

    Private();
    static PatchStatus b19899f81cb7ac3b35e5fad3af777e89f1627474;
    int keyCode;
    QString keyString;
};

CuteimeKeySequence::Private::PatchStatus CuteimeKeySequence::Private::b19899f81cb7ac3b35e5fad3af777e89f1627474 = CuteimeKeySequence::Private::NotChecked;

CuteimeKeySequence::Private::Private()
    : keyCode(0)
{
    cuteimeDebugIn();
    if (b19899f81cb7ac3b35e5fad3af777e89f1627474 == NotChecked) {
        QKeySequence ks(Qt::Key_Zenkaku_Hankaku);
        if (ks.toString() == QLatin1String("Zenkaku Hankaku"))
            b19899f81cb7ac3b35e5fad3af777e89f1627474 = Found;
        else
            b19899f81cb7ac3b35e5fad3af777e89f1627474 = NotFound;
    }
    cuteimeDebugOut() << this;
}

CuteimeKeySequence::CuteimeKeySequence()
{
    cuteimeDebugIn();
    d = new Private;
    cuteimeDebugOut();
}

CuteimeKeySequence::CuteimeKeySequence(const CuteimeKeySequence &other)
{
    cuteimeDebugIn();
    d = new Private;
    d->keyString = other.d->keyString;
    d->keyCode = other.d->keyCode;
    cuteimeDebugOut();
}

CuteimeKeySequence::CuteimeKeySequence(const QString &key)
{
    cuteimeDebugIn() << key;
    d = new Private;
    d->keyString = key;
    if (d->b19899f81cb7ac3b35e5fad3af777e89f1627474 == Private::NotFound) {
#define IF(c, n) \
        if (key == QLatin1String(n)) { \
            d->keyCode = Qt::Key_##c; \
        }
        IF(Kanji, "Kanji")
        else IF(Muhenkan, "Muhenkan")
        else IF(Henkan, "Henkan")
        else IF(Romaji, "Romaji")
        else IF(Hiragana, "Hiragana")
        else IF(Katakana, "Katakana")
        else IF(Hiragana_Katakana, "Hiragana Katakana")
        else IF(Zenkaku, "Zenkaku")
        else IF(Hankaku, "Hankaku")
        else IF(Zenkaku_Hankaku, "Zenkaku Hankaku")
        else {
            d->keyCode = QKeySequence(key)[0];
        }
    } else {
        d->keyCode = QKeySequence(key)[0];
    }
#undef IF
    cuteimeDebugOut();
}

CuteimeKeySequence::CuteimeKeySequence(int k1)
{
    cuteimeDebugIn() << k1;
    d = new Private;
    d->keyCode = k1;
    if (d->b19899f81cb7ac3b35e5fad3af777e89f1627474 == Private::NotFound) {
        switch (k1) {
#define CASE(c, n) \
        case Qt::Key_##c: \
            d->keyString = QLatin1String(n); \
            break
        CASE(Kanji, "Kanji");
        CASE(Muhenkan, "Muhenkan");
        CASE(Henkan, "Henkan");
        CASE(Romaji, "Romaji");
        CASE(Hiragana, "Hiragana");
        CASE(Katakana, "Katakana");
        CASE(Hiragana_Katakana, "Hiragana Katakana");
        CASE(Zenkaku, "Zenkaku");
        CASE(Hankaku, "Hankaku");
        CASE(Zenkaku_Hankaku, "Zenkaku Hankaku");
#undef CASE
        default:
            d->keyString = QKeySequence(k1).toString();
            break;
        }
    } else {
        d->keyString = QKeySequence(k1).toString();
    }
    cuteimeDebugOut();
}

CuteimeKeySequence::~CuteimeKeySequence()
{
    cuteimeDebugIn() << d;
    delete d;
    cuteimeDebugOut();
}

QString CuteimeKeySequence::toString() const
{
    cuteimeDebugIn();
    QString ret = d->keyString;
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeKeySequence &CuteimeKeySequence::operator=(const CuteimeKeySequence &other)
{
    d->keyString = other.d->keyString;
    d->keyCode = other.d->keyCode;
    return *this;
}

bool CuteimeKeySequence::operator==(const CuteimeKeySequence &other) const
{
#ifdef CUTEIME_DEBUG
    cuteimeDebugIn() << other.toString();
    bool ret = d->keyString == other.d->keyString;
    cuteimeDebugOut() << ret;
    return ret;
#else
    return d->keyString == other.d->keyString;
#endif
}

bool CuteimeKeySequence::operator<(const CuteimeKeySequence &other) const
{
#ifdef CUTEIME_DEBUG
    cuteimeDebugIn() << other.toString();
    bool ret = d->keyString < other.d->keyString;
    cuteimeDebugOut() << ret;
    return ret;
#else
    return d->keyString < other.d->keyString;
#endif
}

int CuteimeKeySequence::operator[](uint index) const
{
    cuteimeDebugIn() << index;
    if (index > 0) {
        cuteimeWarning() << index << "is not zero.";
    }
    cuteimeDebugOut() << d->keyCode;
    return d->keyCode;
}
