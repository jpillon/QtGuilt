#-------------------------------------------------
#
# Project created by QtCreator 2013-10-11T21:11:41
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = QtGuiltTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../config.qmake)


DEFINES += SRCDIR=\\\"$$PWD/\\\"


include(../QtGuiltModel/QtGuiltModel.prf)
