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

#include "keyactionmanager.h"
#include "cuteimedebug.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QSettings>

using namespace ja_JP;

class KeyActionDataList::Private
{
public:
    Private(KeyActionDataList *parent);
    void readData(const QString &fileName);
    void saveData() const;

private:
    KeyActionDataList *q;
public:
    QString fileName;
};

KeyActionDataList::Private::Private(KeyActionDataList *parent)
    : q(parent)
{
    cuteimeDebugIn() << parent;
    cuteimeDebugOut();
}

KeyActionDataList::KeyActionDataList()
{
    cuteimeDebugIn();
    d = new Private(this);
    cuteimeDebugOut();
}

KeyActionDataList::KeyActionDataList(const QString &fileName)
{
    cuteimeDebugIn() << fileName;
    d = new Private(this);
    d->readData(fileName);
    cuteimeDebugOut();
}

void KeyActionDataList::Private::readData(const QString &fn)
{
    cuteimeDebugIn() << fn;
    fileName = QFileInfo(fn).fileName();
    KeyActionManager *actions = KeyActionManager::instance();
    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains('\t')) {
                QStringList fields = line.split('\t');
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("//")) continue;
                KeyActionData data;
                data.state = (State)(fields.takeFirst().toInt(0, 16));
                data.key = CuteimeKeySequence(fields.takeFirst());
                data.action = actions->action(fields.takeFirst());
                data.user = false;
                q->append(data);
            } else if (!line.isEmpty()) {
                cuteimeDebug() << line;
            }
        }
        file.close();
    } else {
        cuteimeWarning() << "No such file" << fileName;
    }
    cuteimeDebugOut();
}

void KeyActionDataList::Private::saveData() const
{
    cuteimeDebugIn();
    KeyActionManager::instance();
    QDir dir(QString("%1/.config/%2/%3/").arg(QDir::homePath()).arg(QCoreApplication::organizationName()).arg(QCoreApplication::applicationName()));

    foreach(const QString &str, dir.entryList(QStringList() << "*.txt")) {
        cuteimeDebug() << str;
    }
    QFile file(dir.absoluteFilePath(fileName));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        for (int i = 0; i < q->count(); i++) {
            KeyActionData data = q->at(i);
            QStringList fields;
            fields << QString("0x%1").arg(data.state, 2, 16, QChar('0'));
            fields << data.key.toString();
            fields << data.action->name();
//   cuteimeDebug() << fields.join( "\t" );
            stream << fields.join("\t") + "\n";
        }
        file.close();
        QSettings settings;
        settings.beginGroup("ja_JP::KeyActionManager");
        settings.setValue("DataFile", file.fileName());
    } else {
        cuteimeWarning() << "Cannot write" << fileName;
    }
    cuteimeDebugOut();
}

KeyActionDataList::~KeyActionDataList()
{
    delete d;
}

KeyActionDataList &KeyActionDataList::operator=(const KeyActionDataList & other)
{
    QList<KeyActionData>::operator=(other);
    d->fileName = other.d->fileName;
    return *this;
}

void KeyActionDataList::merge(const KeyActionDataList &other)
{
    for (int i = 0; i < other.count(); i++) {
        bool found = false;
        for (int j = 0; j < length(); j++) {
            if (at(j).key == other.at(i).key && at(j).state == other.at(i).state) {
                replace(j, other.at(i));
                found = true;
                break;
            }
        }
        if (!found) {
            append(other.at(i));
        }
    }
}

const QString &KeyActionDataList::fileName() const
{
    return d->fileName;
}

void KeyActionDataList::read(const QString &fileName)
{
    d->readData(fileName);
}

void KeyActionDataList::save() const
{
    d->saveData();
}

class KeyActionManager::Private
{
public:
    KeyActionList actions;
    mutable KeyActionDataList list;
};

KeyActionManager *KeyActionManager::instance()
{
    cuteimeDebugIn();
    static KeyActionManager *manager = 0;
    if (!manager)
        manager = new KeyActionManager;
    cuteimeDebugOut();
    return manager;
}

KeyActionManager::KeyActionManager(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private;
    cuteimeDebugOut();
}

KeyActionManager::~KeyActionManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

void KeyActionManager::addAction(KeyAction *action)
{
    cuteimeDebugIn() << action;
    d->actions.append(action);
    cuteimeDebugOut();
}

const KeyActionList &KeyActionManager::actions() const
{
    return d->actions;
}

const KeyActionDataList &KeyActionManager::list() const
{
    cuteimeDebugIn();
    if (d->list.isEmpty()) {
        QSettings settings;
        settings.beginGroup("ja_JP::KeyActionManager");
#ifdef CUTEIME_PLATFORM_MAEMO
        d->list.read(settings.value("DataFile", ":/keyassigns/maemo.txt").toString());
#else
        d->list.read(settings.value("DataFile", ":/keyassigns/msime.txt").toString());
#endif
    }
    cuteimeDebugOut() << d->list.count();
    return d->list;
}

void KeyActionManager::setList(const KeyActionDataList &list)
{
    cuteimeDebugIn() << list.count();
    d->list = list;
    d->list.save();
    emit listChanged(d->list);
    cuteimeDebugOut();
}

KeyAction *KeyActionManager::action(const QString &name) const
{
    cuteimeDebugIn() << name;
    KeyAction *ret = 0;
    foreach(KeyAction *a, d->actions) {
        if (name == a->name()) {
            ret = a;
            break;
        } else if (name == QCoreApplication::translate("Converter::Private", a->name())) {
            ret = a;
            break;
        }
    }
    if (!ret) {
        cuteimeWarning() << name << "not found";
    }
    cuteimeDebugOut() << ret;
    return ret;
}
