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

#include "object.h"

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepluginmanager.h>
#include <cuteimelanguage.h>
#include <cuteimeinputmethod.h>
#include <cuteimeconverter.h>
#include <cuteimeinterpreter.h>
#include <cuteimeengine.h>

#include <QSettings>

using namespace InputMethodManager;

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void init();
    void localeChanged(const QString &locale);
    void inputMethodChanged(const QString &identifier);
    void converterChanged(const QString &identifier);
    void interpreterChanged(const QString &identifier);
    void engineChanged(const QString &identifier);

private:
    Object *q;
    CuteimeInputMethodManager inputMethodManager;
    CuteimeInputMethod *currentInputMethod;
    CuteimeConverter *currentConverter;
    CuteimeInterpreter *currentInterpreter;
    CuteimeEngine *currentEngine;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , currentInputMethod(0)
    , currentConverter(0)
    , currentInterpreter(0)
    , currentEngine(0)
{
    cuteimeDebugIn() << parent;
    q->setCategoryType(Hidden);
    q->setActive(true);

    inputMethodManager.init();
    connect(&inputMethodManager, SIGNAL(localeChanged(QString)), this, SLOT(localeChanged(QString)));
    connect(&inputMethodManager, SIGNAL(identifierChanged(QString)), this, SLOT(inputMethodChanged(QString)));
    connect(&inputMethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)));
    connect(&inputMethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)));
    connect(&inputMethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)));
    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    cuteimeDebugOut();
}

Object::Private::~Private()
{
}

void Object::Private::init()
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    QString locale = settings.value(QLatin1String("Locale")).toString();

    QList<CuteimeLanguage *> languages = CuteimePluginManager::objects<CuteimeLanguage>();
    foreach(CuteimeLanguage *language, languages) {
        if (locale.isNull() || locale == language->locale()) {
            inputMethodManager.setLocale(language->locale());
            break;
        }
    }
    cuteimeDebugOut();
}

void Object::Private::localeChanged(const QString &locale)
{
    cuteimeDebugIn() << locale;
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Locale", locale);

    settings.beginGroup(locale);
    QString identifier = settings.value("InputMethod").toString();
    if (identifier.isNull()) {
        QList<CuteimeInputMethod *> inputMethods = CuteimePluginManager::objects<CuteimeInputMethod>();
        foreach (const CuteimeInputMethod *inputMethod, inputMethods) {
            if (inputMethod->locale() == locale) {
                identifier = inputMethod->identifier();
                break;
            }
        }
    }
    inputMethodManager.setIdentifier(identifier);

    cuteimeDebugOut();
}

void Object::Private::inputMethodChanged(const QString &identifier)
{
    if (currentInputMethod && currentInputMethod->identifier() == identifier) return;
    cuteimeDebugIn() << identifier;
    if (currentInputMethod) {
        currentInputMethod->setActive(false);
    }

    QList<CuteimeInputMethod *> inputMethods = CuteimePluginManager::objects<CuteimeInputMethod>();
    foreach(CuteimeInputMethod *inputMethod, inputMethods) {
        if (inputMethod->identifier() == identifier) {
            inputMethod->setActive(true);
            currentInputMethod = inputMethod;
            break;
        }
    }

    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.beginGroup(inputMethodManager.locale());
    settings.setValue("InputMethod", identifier);
    settings.beginGroup(identifier);

    QString converterIdentifier = settings.value("Converter").toString();
    if (converterIdentifier.isNull()) {
        QList<CuteimeConverter *> converters = CuteimePluginManager::objects<CuteimeConverter>();
        foreach(CuteimeConverter *converter, converters) {
            if (currentInputMethod->identifier() == converter->language() && !converter->identifier().isNull()) {
                converterIdentifier = converter->identifier();
                break;
            }
        }
    }
    inputMethodManager.setConverter(converterIdentifier);

    QString interpreterIdentifier = settings.value("Interpreter").toString();
    if (interpreterIdentifier.isNull()) {
        QList<CuteimeInterpreter *> interpreters = CuteimePluginManager::objects<CuteimeInterpreter>();
        foreach(CuteimeInterpreter *interpreter, interpreters) {
            if (currentInputMethod->identifier() == interpreter->language() && !interpreter->identifier().isNull()) {
                interpreterIdentifier = interpreter->identifier();
                break;
            }
        }
    }
    inputMethodManager.setInterpreter(interpreterIdentifier);

    QString engineIdentifier = settings.value("Engine").toString();
    if (engineIdentifier.isNull()) {
        QList<CuteimeEngine *> engines = CuteimePluginManager::objects<CuteimeEngine>();
        foreach(CuteimeEngine *engine, engines) {
            if (currentInputMethod->identifier() == engine->language() && !engine->identifier().isNull()) {
                engineIdentifier = engine->identifier();
                break;
            }
        }
    }
    inputMethodManager.setEngine(engineIdentifier);

    cuteimeDebugOut();
}

void Object::Private::converterChanged(const QString &identifier)
{
    if (currentConverter && currentConverter->identifier() == identifier) return;
    cuteimeDebugIn() << identifier;
    cuteimeDebug() << currentConverter;
    if (currentConverter) {
        currentConverter->setActive(false);
    }

    QList<CuteimeConverter *> converters = CuteimePluginManager::objects<CuteimeConverter>();
    foreach(CuteimeConverter *converter, converters) {
        if (converter->identifier() == identifier) {
            converter->setActive(true);
            currentConverter = converter;
            break;
        }
    }

    if (!identifier.isNull()) {
        QSettings settings;
        settings.beginGroup(q->metaObject()->className());
        settings.beginGroup(inputMethodManager.locale());
        settings.beginGroup(inputMethodManager.identifier());
        settings.setValue("Converter", identifier);
    }
    cuteimeDebugOut() << currentConverter;
}

void Object::Private::interpreterChanged(const QString &identifier)
{
    if (currentInterpreter && currentInterpreter->identifier() == identifier) return;
    cuteimeDebugIn() << identifier;
    if (currentInterpreter) {
        currentInterpreter->setActive(false);
    }

    QList<CuteimeInterpreter *> interpreters = CuteimePluginManager::objects<CuteimeInterpreter>();
    foreach(CuteimeInterpreter *interpreter, interpreters) {
        if (interpreter->identifier() == identifier) {
            interpreter->setActive(true);
            currentInterpreter = interpreter;
            break;
        }
    }

    if (!identifier.isNull()) {
        QSettings settings;
        settings.beginGroup(q->metaObject()->className());
        settings.beginGroup(inputMethodManager.locale());
        settings.beginGroup(inputMethodManager.identifier());
        settings.setValue("Interpreter", identifier);
    }
    cuteimeDebugOut();
}

void Object::Private::engineChanged(const QString &identifier)
{
    if (currentEngine && currentEngine->identifier() == identifier) return;
    cuteimeDebugIn() << identifier;
    if (currentEngine) {
        currentEngine->setActive(false);
    }

    QList<CuteimeEngine *> engines = CuteimePluginManager::objects<CuteimeEngine>();
    foreach(CuteimeEngine *engine, engines) {
        if (engine->identifier() == identifier) {
            cuteimeDebug() << engine;
            engine->setActive(true);
            cuteimeDebug() << engine;
            currentEngine = engine;
            break;
        }
    }

    if (!identifier.isNull()) {
        QSettings settings;
        settings.beginGroup(q->metaObject()->className());
        settings.beginGroup(inputMethodManager.locale());
        settings.beginGroup(inputMethodManager.identifier());
        settings.setValue("Engine", identifier);
    }
    cuteimeDebugOut();
}

Object::Object(QObject *parent)
    : CuteimeAbstractPluginObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Object::~Object()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "object.moc"
