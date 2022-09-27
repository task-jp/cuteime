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

#ifndef CUTEIME_NO_GUI

#include "cuteimesettingswidget.h"
#include "ui_cuteimesettingswidget.h"
#include "cuteimeabstractpluginobject.h"
#include "cuteimedebug.h"

class CuteimeSettingsWidget::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeSettingsWidget *parent);
    void setPlugin(QWidget *widget, CuteimeAbstractPluginObject *plugin);

private:
    CuteimeSettingsWidget *q;

public:
    bool modified;
    Ui::CuteimeSettingsWidget ui;
};

CuteimeSettingsWidget::Private::Private(CuteimeSettingsWidget *parent)
    : QObject(parent)
    , q(parent)
    , modified(false)
{
    cuteimeDebugIn() << parent;
    cuteimeDebugOut();
}

void CuteimeSettingsWidget::Private::setPlugin(QWidget *widget, CuteimeAbstractPluginObject *plugin)
{
    cuteimeDebugIn() << widget << plugin;
    ui.setupUi(widget);
    QIcon icon(plugin->icon());
    if (!icon.isNull()) {
        ui.icon->setPixmap(icon.pixmap(32, 32));
    } else {
        ui.icon->hide();
    }
    ui.name->setText(plugin->name());
    if (plugin->description().isEmpty()) {
        ui.descriptionLabel->hide();
        ui.description->hide();
    } else {
        ui.description->setText(plugin->description());
    }
    if (plugin->author().isEmpty()) {
        ui.author->hide();
        ui.authorLabel->hide();
    } else {
        ui.author->setText(plugin->author());
    }
    if (plugin->url().isEmpty()) {
        ui.url->hide();
        ui.urlLabel->hide();
    } else {
        ui.url->setText(plugin->url());
    }
    if (plugin->translator().isEmpty()) {
        ui.translator->hide();
        ui.translatorLabel->hide();
    } else {
        ui.translator->setText(plugin->translator());
    }
    if (plugin->license().isEmpty()) {
        ui.license->hide();
        ui.licenseLabel->hide();
    } else {
        ui.license->setText(plugin->license());
    }
    cuteimeDebugOut();
}

CuteimeSettingsWidget::CuteimeSettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeSettingsWidget::CuteimeSettingsWidget(CuteimeAbstractPluginObject *plugin, QWidget *parent)
    : QWidget(parent)
{
    cuteimeDebugIn() << plugin << parent;
    d = new Private(this);
    d->setPlugin(this, plugin);
    cuteimeDebugOut();
}

CuteimeSettingsWidget::~CuteimeSettingsWidget()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

void CuteimeSettingsWidget::save()
{
    cuteimeDebugIn();
    emit saved();
    cuteimeDebugOut();
}

bool CuteimeSettingsWidget::isModified() const
{
    return d->modified;
}

void CuteimeSettingsWidget::setModified(bool modified)
{
    if (d->modified == modified) return;
    cuteimeDebugIn() << modified;
    d->modified = modified;
    emit modifiedChanged(d->modified);
    cuteimeDebugOut();
}

QWidget *CuteimeSettingsWidget::about(CuteimeAbstractPluginObject *plugin, const QString &spec, QWidget *parent)
{
    cuteimeDebugIn() << plugin << spec << parent;
    QWidget *ret = new QWidget(parent);
    d->setPlugin(ret, plugin);
    cuteimeDebugOut() << ret;
    return ret;
}

#include "cuteimesettingswidget.moc"

#endif // CUTEIME_NO_GUI
