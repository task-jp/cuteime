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

#include "inputmethodcontroller.h"

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepluginmanager.h>
#include <cuteimeinputmethod.h>
#include <cuteimeconverter.h>
#include <cuteimeinterpreter.h>
#include <cuteimeengine.h>

#include <qbinding.h>

#include <QAction>
#include <QApplication>
#include <QSignalMapper>

class InputMethodController::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputMethodController *parent);
    ~Private();

private slots:
    void composingChanged(bool composing);
    void exec(int action);
    void inputMethodChanged(const QString &identifier);
    void setInputMethod(QAction* action);
    void converterChanged(const QString &identifier);
    void setConverter(QAction* action);
    void interpreterChanged(const QString &identifier);
    void setInterpreter(QAction* action);
    void engineChanged(const QString &identifier);
    void setEngine(QAction *action);

private:
    InputMethodController *q;

    CuteimeApplicationManager *applicationManager;
    CuteimeInputMethodManager *inputMethodManager;

    QAction *inputMethodAction;
    QAction *converterAction;
    QAction *interpreterAction;
    QAction *engineAction;
    QAction *userDictionaryAction;

public:
    QList<QAction*> actions;
};

InputMethodController::Private::Private(InputMethodController *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn();

    applicationManager = new CuteimeApplicationManager(this);
    applicationManager->init();
    connect(applicationManager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));

    inputMethodManager = new CuteimeInputMethodManager(this);
    inputMethodManager->init();
    connect(inputMethodManager, SIGNAL(identifierChanged(QString)), this, SLOT(inputMethodChanged(QString)));
    connect(inputMethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)));
    connect(inputMethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)));
    connect(inputMethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)));

    QSignalMapper *exec = new QSignalMapper(this);
    connect(exec, SIGNAL(mapped(int)), this, SLOT(exec(int)));

    // input method
    {
        inputMethodAction = new QAction(tr("Input Method"), this);
        QActionGroup *group = new QActionGroup(this);

        QList<CuteimeInputMethod *> inputMethods = CuteimePluginManager::objects<CuteimeInputMethod>();
        foreach(CuteimeInputMethod *inputMethod, inputMethods) {
            QAction *action = new QAction(inputMethodAction);
            new QBinding(inputMethod, "icon", action, "icon");
            new QBinding(inputMethod, "name", action, "text");
            action->setData(inputMethod->identifier());
            action->setCheckable(true);
            group->addAction(action);
        }
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setInputMethod(QAction*)));
        actions.append(inputMethodAction);
    }

    // converter
    {
        converterAction = new QAction(tr("Input Character"), this);
        QActionGroup *group = new QActionGroup(this);

        // Direct
        {
            QAction *action = new QAction(converterAction);
            action->setText(tr("Direct"));
            action->setCheckable(true);
            group->addAction(action);
        }

        new QAction(QLatin1String("-"), converterAction);

        QList<CuteimeConverter *> converters = CuteimePluginManager::objects<CuteimeConverter>();
        foreach(CuteimeConverter *converter, converters) {
            QAction *action = new QAction(converterAction);
            new QBinding(converter, "icon", action, "icon");
            new QBinding(converter, "name", action, "text");
            action->setData(converter->identifier());
            action->setCheckable(true);
            group->addAction(action);
        }
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setConverter(QAction*)));
        actions.append(converterAction);
    }

    // interpreter
    {
        interpreterAction = new QAction(tr("Input Style"), this);
        QActionGroup *group = new QActionGroup(this);

        QList<CuteimeInterpreter *> interpreters = CuteimePluginManager::objects<CuteimeInterpreter>();
        foreach(CuteimeInterpreter *interpreter, interpreters) {
            QAction *action = new QAction(interpreterAction);
            new QBinding(interpreter, "icon", action, "icon");
            new QBinding(interpreter, "name", action, "text");
            action->setData(interpreter->identifier());
            action->setCheckable(true);
            group->addAction(action);
        }
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setInterpreter(QAction*)));
        actions.append(interpreterAction);
    }

    // engine
    {
        engineAction = new QAction(tr("Conversion Engine"), this);
        QActionGroup *group = new QActionGroup(this);

        QList<CuteimeEngine *> engines = CuteimePluginManager::objects<CuteimeEngine>();
        foreach(CuteimeEngine *engine, engines) {
            QAction *action = new QAction(engineAction);
            new QBinding(engine, "icon", action, "icon");
            new QBinding(engine, "name", action, "text");
            action->setData(engine->identifier());
            action->setCheckable(true);
            group->addAction(action);
        }
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setEngine(QAction*)));
        actions.append(engineAction);
    }

    // user dictionary
    {
        userDictionaryAction = new QAction(QIcon(":/icons/dictionary.png"), tr("User Dictionary..."), this);
        connect(userDictionaryAction, SIGNAL(triggered()), exec, SLOT(map()));
        exec->setMapping(userDictionaryAction, CuteimeApplicationManager::ShowDictionary);
        actions.append(userDictionaryAction);
    }

    actions.append(new QAction(QLatin1String("-"), this));

    // settings
    {
        QAction *action = new QAction(QIcon(":/icons/configure.png"), tr("Settings..."), this);
        connect(action, SIGNAL(triggered()), exec, SLOT(map()));
        exec->setMapping(action, CuteimeApplicationManager::ShowSettings);
        actions.append(action);
    }

    actions.append(new QAction(QLatin1String("-"), this));

    // about cuteime
    {
        QAction *aboutCuteime = new QAction(qApp->windowIcon(), tr("About cuteime..."), this);
        connect(aboutCuteime, SIGNAL(triggered()), exec, SLOT(map()));
        exec->setMapping(aboutCuteime, CuteimeApplicationManager::ShowAboutCuteime);
        actions.append(aboutCuteime);
    }

    inputMethodChanged(inputMethodManager->identifier());
    converterChanged(inputMethodManager->converter());
    interpreterChanged(inputMethodManager->interpreter());
    engineChanged(inputMethodManager->engine());

    cuteimeDebugOut();
}

InputMethodController::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void InputMethodController::Private::composingChanged(bool composing)
{
    if (composing) {
        // select current converter
        converterChanged(inputMethodManager->converter());
    } else {
        // select direct
        converterChanged(QString());
    }
}

void InputMethodController::Private::exec(int action)
{
    applicationManager->exec((CuteimeApplicationManager::ActionType)action);
}

void InputMethodController::Private::inputMethodChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    foreach (QAction *action, inputMethodAction->findChildren<QAction*>()) {
        if (action->data().toString() == identifier) {
            action->setChecked(true);
            inputMethodAction->setIcon(action->icon());
            break;
        }
    }
    cuteimeDebugOut();
}

void InputMethodController::Private::setInputMethod(QAction* action)
{
    inputMethodManager->setIdentifier(action->data().toString());
}

void InputMethodController::Private::converterChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    foreach (QAction *action, converterAction->findChildren<QAction*>()) {
        if (action->data().toString() == identifier) {
            action->setChecked(true);
            converterAction->setIcon(action->icon());
            break;
        }
    }
    cuteimeDebugOut();
}

void InputMethodController::Private::setConverter(QAction* action)
{
    if (action->data().isNull()) {
        applicationManager->setComposing(false);
    } else {
        applicationManager->setComposing(true);
        inputMethodManager->setConverter(action->data().toString());
    }
}

void InputMethodController::Private::interpreterChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    foreach (QAction *action, interpreterAction->findChildren<QAction*>()) {
        if (action->data().toString() == identifier) {
            action->setChecked(true);
            interpreterAction->setIcon(action->icon());
            break;
        }
    }
    cuteimeDebugOut();
}

void InputMethodController::Private::setInterpreter(QAction* action)
{
    inputMethodManager->setInterpreter(action->data().toString());
}

void InputMethodController::Private::engineChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    foreach (QAction *action, engineAction->findChildren<QAction*>()) {
        if (action->data().toString() == identifier) {
            action->setChecked(true);
            if (!action->icon().isNull())
                engineAction->setIcon(action->icon());
            break;
        }
    }
    cuteimeDebugOut();
}

void InputMethodController::Private::setEngine(QAction* action)
{
    inputMethodManager->setEngine(action->data().toString());
}

InputMethodController::InputMethodController(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn();
    d = new Private(this);
    cuteimeDebugOut();
}

QList<QAction*> InputMethodController::actions() const
{
    return d->actions;
}

#include "inputmethodcontroller.moc"
