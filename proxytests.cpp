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


#include <QtTest>
#include <QtConcurrentRun>

#include <cmath>
#include <cstdlib>

#include "qsignalproxy.h"


class TestObject : public QObject
{
Q_OBJECT

public:
	TestObject(int id = 0) : id(id) { }

	void emitId() { emit myId(id); }
	void emitIds() { emit myIds(id, id); }

signals:
	void myId(int id);
	void myIds(int id, int id2);
	void myIds(int id, const QTime &id2, const QUrl &id3);
	void myIds(int id, char id2, int id3, int id4);
	void myIds(int id, const QString &id2, float id3, double id4, float id5);

public:
	int id;
};

static QString warnigns;
void myMessageHandler(QtMsgType type, const char *msg)
{
	switch (type) { // Ingore warnings
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg);
			abort();
		default: break;
	}
}

static int mul2(int i) { return i * 2; }
static int mul(int i, int j) { return i * j; }
static int a3(int, const QTime &, const QUrl &) { return 0; }
static int a4(int, char, int, int) { return 0; }
static QString a5(int, const QString &, float, double, float) { return QString(); } 

struct MyType {
	QString str;
	TestObject *obj;

	static MyType mod(const MyType &m) {
		MyType m2;
		m2.str = m.str.split(" ").last();
		m2.obj = m.obj;
		return m2;
	}
};
Q_DECLARE_METATYPE(MyType);


class ProxyTests : public QObject
{
Q_OBJECT

public:
	ProxyTests() : m_testobj(42) { }

private slots: // Tests
	void connectionTypeCheck()
	{
		qInstallMsgHandler(myMessageHandler); // Ignore warnings

		// Floating-point remainder function: float fmodf(float, float)
		QSignalProxy p;
		QVERIFY(false == p.connect(&m_testobj, SIGNAL(myId(int)), fmodf, this, SLOT(getInt(int))));

		// Terminate calling process: void _exit(int)
		QVERIFY(false == p.connect(&m_testobj, SIGNAL(myId(int)), _exit, this, SLOT(getInt(int))));

		// Random bytes: int rand(void)
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myId(int)), rand, this, SLOT(getInt(int))));

		// Absolute value: int abs(int)
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myId(int)), abs, this, SLOT(getInt(int))));

		qInstallMsgHandler(NULL);
	}

	void connectionArgsCheck()
	{
		qInstallMsgHandler(myMessageHandler); // Ignore warnings

		QSignalProxy p;
		QVERIFY(false == p.connect(&m_testobj, SIGNAL(noSuchSignal(int)), abs, this, SLOT(getInt(int))));
		QVERIFY(false == p.connect(&m_testobj, SIGNAL(myId(int)), abs, this, SLOT(noSuchSlot(int))));

		qInstallMsgHandler(NULL);
	}

	void functionPointers()
	{
		QSignalProxy p;
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myId(int)), mul2, this, SLOT(getInt(int))));
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myIds(int, int)), mul, this, SLOT(getInt(int))));

		m_testobj.emitId();
		QVERIFY(m_int == mul2(m_testobj.id));
		m_testobj.emitIds();
		QVERIFY(m_int == mul(m_testobj.id, m_testobj.id));
	}

	void customTypes()
	{
		m_type.str = "might be changed";
		m_type.obj = &m_testobj;

		QSignalProxy p;
		QVERIFY(true == p.connect(this, SIGNAL(myType(const MyType &)), MyType::mod, this, SLOT(getMyType(const MyType &))));

		emit myType(m_type);
		QVERIFY(m_type.str == MyType::mod(m_type).str);
	}

	void longSignatures()
	{
		QSignalProxy p;
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myId(int)), mul2, this, SLOT(getInt(int))));
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myIds(int, int)), mul, this, SLOT(getInt(int))));
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myIds(int, const QTime &, const QUrl &)), a3, this, SLOT(getInt(int))));
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myIds(int, char, int, int)), a4, this, SLOT(getInt(int))));
		QVERIFY(true == p.connect(&m_testobj, SIGNAL(myIds(int, const QString &, float, double, float)), a5, this, SLOT(getString(const QString &))));
	}

protected slots:
	void getInt(int i) { m_int = i; }
	void getString(const QString &s) { m_str = s; }
	void getMyType(const MyType &m) { m_type = m; }


signals:
	void myType(const MyType &m);

private:
	TestObject m_testobj;
	MyType m_type;
	int m_int;
	QString m_str;
};


QTEST_MAIN(ProxyTests)
#include "proxytests.moc"
