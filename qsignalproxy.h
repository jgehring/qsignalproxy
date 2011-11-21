/*
 * QSignalProxy - Proxy funtions for Qt Signal/Slot connections
 * Copyright (C) 2011 Jonas Gehring
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef QSIGNLPROXY_H_
#define QSIGNLPROXY_H_


#include <QObject>

#include "qsignalproxyfunctions.h"


class QSignalProxy : public QObject
{
	struct Receiver
	{
		Receiver(QSignalProxyFunctions::Function *f, const QObject *o, int methodId)
			: f(f), o(o), methodId(methodId) { }
		~Receiver() { delete f; }

		QSignalProxyFunctions::Function *f;
		const QObject *o;
		int methodId;
	};

public:
	QSignalProxy(QObject *parent = 0);
	~QSignalProxy();

	// Function pointer proxies (multi-in, single-out)
	template <typename F>
	inline bool connect(const QObject *s, const char *si, F f, const QObject *r, const char *me)
		{ return proxyConnect(s, si, new QSignalProxyFunctions::FunctionPointer<F>(f), r, me); }

	// Reimplemented from QOBJECT
	const QMetaObject *metaObject() const;
	void *qt_metacast(const char *className);
	int qt_metacall(QMetaObject::Call call, int id, void **args);
	static const QMetaObject staticMetaObject;

private:
	bool proxyConnect(const QObject *sender, const char *signal, QSignalProxyFunctions::Function *proxy, const QObject *receiver, const char *method);

	// Prevent copies
	QSignalProxy(const QSignalProxy &);
	QSignalProxy &operator=(const QSignalProxy &);

private:
	QList<Receiver *> m_connections;
};

#endif // QSIGNLPROXY_H_
