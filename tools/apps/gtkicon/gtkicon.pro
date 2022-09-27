TARGET = gtkicon
CONFIG += gtk
CONFIG -= qt
QT -= core gui
include(../../../cuteimeapp.pri)

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0 gdk-pixbuf-2.0

HEADERS =
SOURCES = main.c
