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

#include "settings.h"
#include "ui_settings.h"

#include <cuteimedebug.h>
#include <cuteimeabstractpluginobject.h>
#include <cuteimeapplicationmanager.h>

#include <QApplication>
#include <QDir>
#include <QTranslator>
#include <QRadioButton>
#include <QSettings>

using namespace System::Language;

class Settings::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeAbstractPluginObject *plugin, Settings *parent);
    ~Private();

    void save();

private:
    Settings *q;
    Ui::Settings ui;
    QMap<QRadioButton*, QString> map;
    CuteimeAbstractPluginObject *plugin;
    CuteimeApplicationManager applicationManager;
};

Settings::Private::Private(CuteimeAbstractPluginObject *p, Settings *parent)
    : QObject(parent)
    , q(parent)
    , plugin(p)
{
    cuteimeDebugIn() << p << parent;
    applicationManager.init();
    ui.setupUi(q);

    QString current = applicationManager.displayLanguage();
    QTranslator translator;

#ifdef CUTEIME_APPLICATION_DIR_PATH
    QDir rootDir(CUTEIME_MACRO_TO_STRING2(CUTEIME_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QApplication::applicationDirPath();
#endif
    // up to root dir
    for (int i = 0; i < QString(CUTEIME_MACRO_TO_STRING(CUTEIME_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_TRANSLATIONS_PATH)));

    cuteimeDebug() << rootDir;
    foreach(const QString &qm, rootDir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name)) {
        cuteimeDebug() << qm;
        if (translator.load(qm, rootDir.absolutePath())) {
            QString translation = translator.translate("System::Language::Settings::Private", QT_TR_NOOP("English"));
            QRadioButton *radio = new QRadioButton(translation.isEmpty() ? QLatin1String("English") : translation);
            map[radio] =  QFileInfo(qm).baseName().mid(QString("cuteime_").length());
            ui.languages->addWidget(radio);
            radio->setChecked(map[radio] == current);
            radio->show();
        }
    }
    ui.languages->addStretch();
    cuteimeDebugOut();
}

Settings::Private::~Private()
{
}

void Settings::Private::save()
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup("Application");
    foreach(QRadioButton *radio, map.keys()) {
        if (radio->isChecked()) {
            cuteimeDebug() << map[radio];
            applicationManager.setDisplayLanguage(map[radio]);
            settings.setValue("Language", map[radio]);
            break;
        }
    }
    cuteimeDebugOut();
}

Settings::Settings(CuteimeAbstractPluginObject *plugin, QWidget *parent)
    : CuteimeSettingsWidget(parent)
{
    d = new Private(plugin, this);
}

Settings::~Settings()
{
    delete d;
}

void Settings::save()
{
    d->save();
    CuteimeSettingsWidget::save();
}

#include "settings.moc"
