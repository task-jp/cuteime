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

#include "cuteimeabstractpluginobject.h"
#include "cuteimedebug.h"
#include "cuteimesettingswidget.h"

#include <QSettings>
#include <QStringList>

class CuteimeAbstractPluginObject::Private
{
public:
    QString identifier;
    int priority;

    QString locale;
    QString language;

#ifndef CUTEIME_NO_GUI
    QIcon icon;
#endif
    QString name;
    QString description;
    QString author;
    QString url;
    QString translator;
    QString license;

    bool active;

    QStringList groups;
    CategoryType categoryType;
    QString categoryName;
};

CuteimeAbstractPluginObject::CuteimeAbstractPluginObject(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private;
    d->priority = 0;
    d->active = false;
    d->categoryType = Hidden;
    cuteimeDebugOut();
}

CuteimeAbstractPluginObject::~CuteimeAbstractPluginObject()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::identifier() const
{
    return d->identifier;
}

void CuteimeAbstractPluginObject::setIdentifier(const QString &identifier)
{
    if (d->identifier == identifier) return;
    d->identifier = identifier;
    emit identifierChanged(identifier);
}

int CuteimeAbstractPluginObject::priority() const
{
    return d->priority;
}

void CuteimeAbstractPluginObject::setPriority(int priority)
{
    if (d->priority == priority) return;
    d->priority = priority;
    emit priorityChanged(priority);
}


const QString &CuteimeAbstractPluginObject::locale() const
{
    return d->locale;
}

void CuteimeAbstractPluginObject::setLocale(const QString &locale)
{
    if (d->locale == locale) return;
    cuteimeDebugIn() << locale;
    d->locale = locale;
    emit localeChanged(d->locale);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::language() const
{
    return d->language;
}

void CuteimeAbstractPluginObject::setLanguage(const QString &language)
{
    if (d->language == language) return;
    cuteimeDebugIn() << language;
    d->language = language;
    emit languageChanged(d->language);
    cuteimeDebugOut();
}

#ifndef CUTEIME_NO_GUI
const QIcon &CuteimeAbstractPluginObject::icon() const
{
    return d->icon;
}

void CuteimeAbstractPluginObject::setIcon(const QIcon &icon)
{
    if (d->icon.cacheKey() == icon.cacheKey()) return;
    cuteimeDebugIn() << icon.availableSizes();
    d->icon = icon;
    emit iconChanged(d->icon);
    cuteimeDebugOut();
}
#endif

const QString &CuteimeAbstractPluginObject::name() const
{
    return d->name;
}

void CuteimeAbstractPluginObject::setName(const QString &name)
{
    if (d->name == name) return;
    cuteimeDebugIn() << name;
    d->name = name;
    emit nameChanged(d->name);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::description() const
{
    return d->description;
}

void CuteimeAbstractPluginObject::setDescription(const QString &description)
{
    if (d->description == description) return;
    cuteimeDebugIn() << description;
    d->description = description;
    emit descriptionChanged(d->description);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::author() const
{
    return d->author;
}

void CuteimeAbstractPluginObject::setAuthor(const QString &author)
{
    if (d->author == author) return;
    cuteimeDebugIn() << author;
    d->author = author;
    emit authorChanged(d->author);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::url() const
{
    return d->url;
}

void CuteimeAbstractPluginObject::setUrl(const QString &url)
{
    if (d->url == url) return;
    cuteimeDebugIn() << url;
    d->url = url;
    emit urlChanged(d->url);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::translator() const
{
    return d->translator;
}

void CuteimeAbstractPluginObject::setTranslator(const QString &translator)
{
    if (d->translator == translator) return;
    cuteimeDebugIn() << translator;
    d->translator = translator;
    emit translatorChanged(d->translator);
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::license() const
{
    return d->license;
}

void CuteimeAbstractPluginObject::setLicense(const QString &license)
{
    if (d->license == license) return;
    cuteimeDebugIn() << license;
    d->license = license;
    emit licenseChanged(d->license);
    cuteimeDebugOut();
}


bool CuteimeAbstractPluginObject::isEnabled() const
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    bool ret = settings.value("Enabled", true).toBool();
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimeAbstractPluginObject::setEnabled(bool enabled)
{
    if (isEnabled() == enabled) return;
    cuteimeDebugIn() << enabled;
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    settings.setValue("Enabled", enabled);
    emit enabledChanged(enabled);
    cuteimeDebugOut();
}

bool CuteimeAbstractPluginObject::isActive() const
{
    return d->active;
}

void CuteimeAbstractPluginObject::setActive(bool active)
{
    if (d->active == active) return;
    d->active = active;
    emit activeChanged(active);
}


const QStringList &CuteimeAbstractPluginObject::groups() const
{
    return d->groups;
}

void CuteimeAbstractPluginObject::setGroups(const QStringList &groups)
{
    if (d->groups == groups) return;
    cuteimeDebugIn() << groups;
    d->groups = groups;
    cuteimeDebugOut();
}

CuteimeAbstractPluginObject::CategoryType CuteimeAbstractPluginObject::categoryType() const
{
    return d->categoryType;
}

void CuteimeAbstractPluginObject::setCategoryType(CuteimeAbstractPluginObject::CategoryType categoryType)
{
    if (d->categoryType == categoryType) return;
    cuteimeDebugIn() << categoryType;
    d->categoryType = categoryType;
    cuteimeDebugOut();
}

const QString &CuteimeAbstractPluginObject::categoryName() const
{
    return d->categoryName;
}

void CuteimeAbstractPluginObject::setCategoryName(const QString &categoryName)
{
    if (d->categoryName == categoryName) return;
    cuteimeDebugIn() << categoryName;
    d->categoryName = categoryName;
    cuteimeDebugOut();
}

#ifndef CUTEIME_NO_GUI
CuteimeSettingsWidget *CuteimeAbstractPluginObject::settings(const QString &hint, QWidget *parent)
{
    cuteimeDebugIn() << parent;
    CuteimeSettingsWidget *ret = new CuteimeSettingsWidget(this, parent);
    cuteimeDebugOut() << ret;
    return ret;
}
#endif // CUTEIME_NO_GUI
