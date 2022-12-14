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

#include "cuteimecommandlineargs.h"

#include "cuteimedebug.h"

#include <QCoreApplication>
#include <QStringList>
#include <QVariantMap>
#include <QFileInfo>

CuteimeCommandlineArgs *CuteimeCommandlineArgs::instance()
{
    static CuteimeCommandlineArgs ret;
    return &ret;
}

CuteimeCommandlineArgs::CuteimeCommandlineArgs(QObject *parent)
    : QCommandLineArgs(parent)
{
    cuteimeDebugIn() << parent;
    addOption("Virtual Keyboard", "-k <qml>", "qml file for primary virual keyboard");
    addOption("Virtual Keyboard", "--keyboard <qml>", "qml file for primary virual keyboard");
    addOption("Virtual Keyboard(Sub)", "-s <qml>", "qml file for secondary virual keyboard");
    addOption("Virtual Keyboard(Sub)", "--sub-keyboard <qml>", "qml file for secondary virual keyboard");
    cuteimeDebugOut() << parent;
}

class QCommandLineArgs::Private
{
public:
    struct QCommandLineArgsOption
    {
        QString name;
        QString option;
        const char *description;
        QVariant defaultValue;
    };
    Private(QCommandLineArgs *parent);

    void addOption(const char *name, const char *option, const char *description = 0, const QVariant &defaultValue = QVariant());
    void help() const;
    void version() const;
    void error(const QString &message) const;

    static void parse(const QString &arg, QString &key, QString &value, QChar &sep);

private:
    QCommandLineArgs *q;

public:
    bool parsed;
    QList<QCommandLineArgsOption> knownOptions;
    QVariantMap valueMap;
};

QCommandLineArgs::Private::Private(QCommandLineArgs *parent)
    : q(parent)
    , parsed(false)
{
    cuteimeDebugIn() << parent;
    addOption("Help", "-h", "show this help");
    addOption("Help", "-help");
    addOption("Help", "--help");
    addOption("Version", "-v", "show version");
    addOption("Version", "--version");
    cuteimeDebugOut();
}

void QCommandLineArgs::Private::help() const
{
    cuteimeDebugIn();
    QString applicationName = QCoreApplication::applicationName();
    QString applicationVersion = QCoreApplication::applicationVersion();
    if (!applicationName.isEmpty()) {
        if (applicationVersion.isEmpty())
            fprintf(stderr, "%s\n", applicationName.toUtf8().constData());
        else
            fprintf(stderr, "%s(%s)\n", applicationName.toUtf8().constData(), applicationVersion.toUtf8().constData());
    }
    fprintf(stderr, "--- Options --------------------------------------------------------------------\n");
    foreach (const QCommandLineArgsOption &option, knownOptions) {

        if (option.description) {
            if (option.defaultValue.isNull())
                fprintf(stderr, "\t%s : %s\n\t\t%s\n", option.option.toUtf8().constData(), option.name.toUtf8().constData(), option.description);
            else
                fprintf(stderr, "\t%s : %s\n\t\t%s\n\t\tdefault: %s\n", option.option.toUtf8().constData(), option.name.toUtf8().constData(), option.description, option.defaultValue.toString().toUtf8().constData());
        }
    }

    cuteimeDebugOut();
}

void QCommandLineArgs::Private::version() const
{
    cuteimeDebugIn();
    qDebug() << QCoreApplication::applicationVersion();
    cuteimeDebugOut();
}

void QCommandLineArgs::Private::error(const QString &message) const
{
    cuteimeDebugIn();
    qDebug() << message;
    help();
    cuteimeDebugOut();
}

void QCommandLineArgs::Private::addOption(const char *name, const char *o, const char *description, const QVariant &defaultValue)
{
//    cuteimeDebugIn() << name << o << description;
    QCommandLineArgsOption option;
    option.name = QString::fromUtf8(name);
    option.option = QString::fromUtf8(o);
    option.description = description;
    option.defaultValue = defaultValue;
    knownOptions.append(option);
    if (!option.defaultValue.isNull()) {
        valueMap[option.name] = defaultValue;
    }
    // addOption("file", "-f");
    // addOption("file", "-f VALUE);
    // addOption("file", "--file=VALUE);
//    cuteimeDebugOut();
}

void QCommandLineArgs::Private::parse(const QString &arg, QString &key, QString &value, QChar &sep)
{
//    cuteimeDebugIn() << arg;
    int indexOfSpace = arg.indexOf(' ');
    int indexOfEqual = arg.indexOf('=');

    if (indexOfSpace < 0 && indexOfEqual < 0) {
        key = arg;
    } else {
        if(indexOfSpace < 0) {
            sep = QLatin1Char('=');
        } else if(indexOfEqual < 0) {
            sep = QLatin1Char(' ');
        } else if(indexOfSpace < indexOfEqual) {
            sep = QLatin1Char(' ');
        } else {
            sep = QLatin1Char('=');
        }
        key = arg.section(sep, 0, 0);
        value = arg.section(sep, 1);
    }
//    cuteimeDebugOut() << key << value << sep;
}

QCommandLineArgs::QCommandLineArgs(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

QCommandLineArgs::~QCommandLineArgs()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

void QCommandLineArgs::addOption(const char *name, const char *option, const char *description, const QVariant &defaultValue)
{
//    cuteimeDebugIn() << name << option << description;
    d->addOption(name, option, description, defaultValue);
//    cuteimeDebugOut();
}

bool QCommandLineArgs::parse(int &argc, char**argv)
{
    cuteimeDebugIn() << argc << argv;
    bool ret = true;
    int ret_argc = argc;
    for (int i = 1; i < argc && ret; i++) {
        QString key;
        QString value;
        QChar sep;
        d->parse(QString::fromUtf8(argv[i]), key, value, sep);
        cuteimeDebug() << key << value << sep;
        foreach (const Private::QCommandLineArgsOption& option, d->knownOptions) {
            QString o = option.option;
            QString k;
            QString v;
            QChar s;
            d->parse(o, k, v, s);

            if (key == k) {
                cuteimeDebug() << k << s.toLatin1() << ' ' << '=';
                switch (s.toLatin1()) {
                case 0:
                    d->valueMap[option.name] = true;
                    break;
                case ' ':
                    if (++i < argc) {
                        d->valueMap[option.name] = QString::fromUtf8(argv[i]);
                        ret_argc--;
                    } else {
                        d->error(QString("%1 needs value").arg(option.name));
                        ret = false;
                    }
                    break;
                case '=':
                    if (!value.isEmpty()) {
                        d->valueMap[option.name] = value;
                    } else {
                        d->error(QString("%1 needs value").arg(option.name));
                        ret = false;
                    }
                    break;
                default:
                    cuteimeWarning() << sep;
                }

                break;
            }
        }
    }
    cuteimeDebug() << d->valueMap;
    if (value("Help").toBool()) {
        d->help();
        ret = false;
    }

    if (value("Version").toBool()) {
        d->version();
        ret = false;
    }
    cuteimeDebugOut() << ret;
    return ret;
}

QVariant QCommandLineArgs::value(const char *name)
{
    return d->valueMap[QString::fromUtf8(name)];
}
