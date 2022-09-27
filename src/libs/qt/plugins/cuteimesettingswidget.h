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

#ifndef CUTEIMESETTINGSWIDGET_H
#define CUTEIMESETTINGSWIDGET_H

#ifndef CUTEIME_NO_GUI

#include <QtWidgets/QWidget>
#include "cuteimeglobal.h"

class CuteimeAbstractPluginObject;

class CUTEIME_EXPORT CuteimeSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    CuteimeSettingsWidget(QWidget *parent = 0);
    CuteimeSettingsWidget(CuteimeAbstractPluginObject *plugin, QWidget *parent = 0);
    ~CuteimeSettingsWidget();

    bool isModified() const;

public slots:
    virtual void save();

protected slots:
    void setModified(bool modified = true);

signals:
    void modifiedChanged(bool modified);
    void saved();

protected:
    QWidget *about(CuteimeAbstractPluginObject *plugin, const QString &spec, QWidget *parent = 0);

private:
    class Private;
    Private *d;
};

#endif // CUTEIME_NO_GUI

#endif//CUTEIMESETTINGSWIDGET_H
