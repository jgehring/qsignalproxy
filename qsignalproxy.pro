#
# QSignalProxy - Proxy funtions for Qt Signal/Slot connections
# Copyright (C) 2011 Jonas Gehring
#

TEMPLATE = app
TARGET = qsignalproxy

QT += testlib
CONFIG -= app_bundle

SOURCES += \
	qsignalproxy.cpp \
	proxytests.cpp

HEADERS += \
	qsignalproxy.h \
	qsignalproxyfunctions.h
