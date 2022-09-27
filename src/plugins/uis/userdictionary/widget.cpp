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

#include "widget.h"
#include "ui_widget.h"

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimeengine.h>
#include <cuteimeenginedictionary.h>
#include <cuteimepluginmanager.h>

#include <QMessageBox>
#include <QSettings>
#include <QSortFilterProxyModel>

namespace UserDictionary {

class Widget::Private : private QObject
{
    Q_OBJECT
public:
    Private(Widget *parent);
    ~Private();

    void accept();
    void show();
    void hide();

private slots:
    void exec();

    void add();
    void del();
    void edit(const QModelIndex &index);

private:
    Widget *q;
    Ui::Widget ui;
    CuteimeInputMethodManager inputMethodManager;
    CuteimeEngineDictionary *currentDictionary;
    QSortFilterProxyModel *model;
};

}

using namespace UserDictionary;

Widget::Private::Private(Widget *parent)
    : QObject(parent)
    , q(parent)
    , currentDictionary(0)
{
    cuteimeDebugIn() << parent;
    ui.setupUi(q);

    connect(ui.add, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui.del, SIGNAL(clicked()), this, SLOT(del()));
    connect(ui.treeView, SIGNAL(activated(QModelIndex)), this, SLOT(edit(QModelIndex)));
    inputMethodManager.init();
    connect(&inputMethodManager, SIGNAL(engineChanged(QString)), this, SLOT(exec()));
    model = new QSortFilterProxyModel(this);
    connect(ui.filter, SIGNAL(textChanged(QString)), model, SLOT(setFilterFixedString(QString)));
    model->setFilterKeyColumn(0);
    ui.treeView->setModel(model);
    exec();
    cuteimeDebugOut();
}

Widget::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Widget::Private::accept()
{
    currentDictionary->save();
}

void Widget::Private::exec()
{
    cuteimeDebugIn();
    QString currentEngine = inputMethodManager.engine();
    foreach(CuteimeEngine *engine, CuteimePluginManager::objects<CuteimeEngine>(this)) {
        if (engine->identifier() == currentEngine) {
            currentDictionary = engine->dictionary();
            if (currentDictionary) {
                ui.filter->clear();
                model->setSourceModel(currentDictionary->model());
            } else {
                QMessageBox::warning(q, engine->name(), tr("%1 does not support user dictoinary.").arg(engine->name()));
                return;
            }
            break;
        }
    }
    cuteimeDebugOut();
}

void Widget::Private::show()
{
    QString currentEngine = inputMethodManager.engine();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    q->resize(settings.value("Size", q->size()).toSize());
    settings.beginGroup(currentEngine);
    ui.treeView->header()->restoreState(settings.value("Header").toByteArray());
    settings.endGroup();
}

void Widget::Private::hide()
{
    QString currentEngine = inputMethodManager.engine();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Size", q->size());
    settings.beginGroup(currentEngine);
    settings.setValue("Header", ui.treeView->header()->saveState());
    settings.endGroup();
}

void Widget::Private::add()
{
    currentDictionary->add(q);
}

void Widget::Private::del()
{
    currentDictionary->del(model->mapToSource(ui.treeView->currentIndex()), q);
}

void Widget::Private::edit(const QModelIndex &index)
{
    currentDictionary->edit(model->mapToSource(index), q);
}

Widget::Widget(QWidget *parent)
    : QDialog(parent)
{
    d = new Private(this);
}

Widget::~Widget()
{
    delete d;
}

bool Widget::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        emit languageChanged();
    }
    return QDialog::event(e);
}

void Widget::showEvent(QShowEvent *e)
{
    d->show();
    QDialog::showEvent(e);
}

void Widget::hideEvent(QHideEvent *e)
{
    d->hide();
    QDialog::hideEvent(e);
}

void Widget::accept()
{
    d->accept();
    QDialog::accept();
}

#include "widget.moc"
