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

#include "application.h"
#include <cuteimeserver.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimepluginmanager.h>
#include <cuteimedebug.h>

#include <QDir>
#ifndef CUTEIME_NO_GUI
#include <QIcon>
#endif
#include <QLibraryInfo>
#include <QStringList>
#include <QSettings>
#include <QTranslator>

class Application::Private : private QObject
{
    Q_OBJECT
public:
    Private(Application *parent);
    ~Private();

private slots:
    void displayLanguageChanged(const QString &displayLanguage);

private:
    void setPluginPath();

private:
    Application *q;
    CuteimeApplicationManager applicationManager;
};

Application::Private::Private(Application *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;
    q->setOrganizationName("QtQuick.Me");
    q->setOrganizationDomain("qtquick.me");
    q->setApplicationName("cuteime");
    q->setApplicationVersion(CUTEIME_MACRO_TO_STRING(CUTEIME_VERSION));

#ifndef CUTEIME_NO_GUI
    QIcon icon;
    QDir icons(":/icons/");
    foreach(const QString &size, icons.entryList(QStringList() << "*x*")) {
        icon.addPixmap(icons.absoluteFilePath(QStringLiteral("%1/cuteime.png").arg(size)));
    }
    q->setWindowIcon(icon);
#endif

    setPluginPath();

    QStringList confDir;
    confDir << QDir::homePath();
    confDir << QLatin1String(".config");
    confDir << QCoreApplication::organizationName();
    confDir << QCoreApplication::applicationName();
    QDir dir(confDir.join("/"));
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }

    QSettings settings;
    settings.beginGroup("Application");
    displayLanguageChanged(settings.value("Language", tr("en")).toString());

#ifndef CUTEIME_NO_GUI
    q->setQuitOnLastWindowClosed(false);
#endif

    new CuteimeServer(this);

    CuteimePluginManager::objects<CuteimeAbstractPluginObject>(this);

    applicationManager.init();
    connect(&applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(displayLanguageChanged(QString)));

    cuteimeDebugOut();
}

Application::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Application::Private::displayLanguageChanged(const QString &displayLanguage)
{
    cuteimeDebugIn() << displayLanguage;
#ifdef CUTEIME_APPLICATION_DIR_PATH
    QDir rootDir(CUTEIME_MACRO_TO_STRING2(CUTEIME_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    // up to root dir
    for (int i = 0; i < QString(CUTEIME_MACRO_TO_STRING(CUTEIME_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_TRANSLATIONS_PATH)));

    foreach (QTranslator *translator, findChildren<QTranslator*>()) {
        q->removeTranslator(translator);
        translator->deleteLater();
    }

    QTranslator *translator = new QTranslator(this);
    cuteimeDebug() << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    translator->load(QString("qt_%1").arg(displayLanguage), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    cuteimeDebug() << rootDir.absolutePath();
    translator->load(QString("cuteime_%1").arg(displayLanguage), rootDir.absolutePath());
    q->installTranslator(translator);
    cuteimeDebugOut();
}

void Application::Private::setPluginPath()
{
//    cuteimeDebugOn();
    cuteimeDebugIn();

#ifdef CUTEIME_APPLICATION_DIR_PATH
    QDir rootDir(CUTEIME_MACRO_TO_STRING2(CUTEIME_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    cuteimeDebug() << rootDir;

    // up to root dir
    QString appPath(CUTEIME_MACRO_TO_STRING(CUTEIME_APP_PATH));
    if (!appPath.isEmpty()) {
        for (int i = 0; i < appPath.count(QLatin1Char('/')) + 1; i++) {
            rootDir.cdUp();
        }
    }
    cuteimeDebug() << rootDir << QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_PLUGIN_PATH));

    rootDir.cd(QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_PLUGIN_PATH)));
    cuteimeDebug() << rootDir;

    cuteimeDebug() << rootDir.absolutePath();
    setCuteimePluginPath(rootDir.absolutePath());
#ifndef Q_OS_LINUX
    if (rootDir.exists(QLatin1String("inputmethods"))) {
        q->addLibraryPath(rootDir.absolutePath());
    }
#endif
    cuteimeDebugOut();
//    cuteimeDebugOff();
}

Application::Application(int &argc, char **argv)
#ifndef CUTEIME_NO_GUI
    : QApplication(argc, argv)
#else
    : QCoreApplication(argc, argv)
#endif
{
    if (!qgetenv("CUTEIME_DEBUG").isEmpty()) {
        cuteimeDebugOn();
    }
    cuteimeDebugIn();
    d = new Private(this);
    cuteimeDebugOut();
}

Application::~Application()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "application.moc"
