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


#ifndef QSIGNALPROXY_FUNCTIONS_H
#define QSIGNALPROXY_FUNCTIONS_H


#include <QMetaType>

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>


#define RAW_TYPE(T) typename boost::remove_const<typename boost::remove_reference<T>::type>::type

namespace QSignalProxyFunctions
{

// Helper functions
template <typename T> inline const char *rawTypeName() { return QMetaType::typeName(qMetaTypeId<RAW_TYPE(T)>()); }
template <> inline const char *rawTypeName<void>() { return "void"; }

// Returns a type name string using qMetaTypeId<T>()
template <typename T> QByteArray typeName() {
	if (boost::is_const<T>::value || boost::is_const<typename boost::remove_reference<T>::type>::value) {
		if (boost::is_reference<T>::value) {
			return QByteArray("const ") + rawTypeName<T>() + "&";
		} else {
			return QByteArray("const ") + rawTypeName<T>();
		}
	} else if (boost::is_reference<T>::value) {
		return QByteArray(rawTypeName<T>()) + "&";
	} else {
		return rawTypeName<T>();
	}
}

// Base class for stored functions
struct Function
{
	virtual ~Function() { }
	virtual QByteArray signature_in() const = 0; // Like a slot with parameters
	virtual QByteArray signature_out() const = 0; // Like a signal with return type
	virtual void call(void **result, void **args) = 0;
};

// Helper classes for function pointers
template <int N, typename T, typename F> struct FPSignature {
	static QByteArray sigin(); // without closing brace
};
template <int N, typename T, typename F> struct FPCall {
	template<typename FP> static void call(FP f, void **r, void **a);
};

template <typename T, typename F> struct FPSignature<0, T, F> {
	static inline QByteArray sigin() { return "_proxy_in("; }
};
template <typename T, typename F> struct FPCall<0, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **) {
		typename F::result_type v = (*f)();
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<0, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **) { (*f)(); }
};

template <typename T, typename F> struct FPSignature<1, T, F> {
	static inline QByteArray sigin() { return FPSignature<0, T, F>::sigin() + typeName<typename F::arg1_type>(); }
};
template <typename T, typename F> struct FPCall<1, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **a) {
		typename F::result_type v = (*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]));
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<1, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **a) {
		(*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]));
	}
};

template <typename T, typename F> struct FPSignature<2, T, F> {
	static inline QByteArray sigin() { return FPSignature<1, T, F>::sigin() + "," + typeName<typename F::arg2_type>(); }
};
template <typename T, typename F> struct FPCall<2, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **a) {
		typename F::result_type v = (*f)(
			*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			*reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2])
		);
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<2, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **a) {
		(*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2])
		);
	}
};

template <typename T, typename F> struct FPSignature<3, T, F> {
	static inline QByteArray sigin() { return FPSignature<2, T, F>::sigin() + "," + typeName<typename F::arg3_type>(); }
};
template <typename T, typename F> struct FPCall<3, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **a) {
		typename F::result_type v = (*f)(
			*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			*reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			*reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3])
		);
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<3, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **a) {
		(*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3])
		);
	}
};

template <typename T, typename F> struct FPSignature<4, T, F> {
	static inline QByteArray sigin() { return FPSignature<3, T, F>::sigin() + "," + typeName<typename F::arg4_type>(); }
};
template <typename T, typename F> struct FPCall<4, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **a) {
		typename F::result_type v = (*f)(
			*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			*reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			*reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3]),
			*reinterpret_cast<RAW_TYPE(typename F::arg4_type) *>(a[4])
		);
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<4, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **a) {
		(*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg4_type) *>(a[4])
		);
	}
};

template <typename T, typename F> struct FPSignature<5, T, F> {
	static inline QByteArray sigin() { return FPSignature<4, T, F>::sigin() + "," + typeName<typename F::arg5_type>(); }
};
template <typename T, typename F> struct FPCall<5, T, F> {
	template <typename FP> static inline void call(FP f, void **r, void **a) {
		typename F::result_type v = (*f)(
			*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			*reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			*reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3]),
			*reinterpret_cast<RAW_TYPE(typename F::arg4_type) *>(a[4]),
			*reinterpret_cast<RAW_TYPE(typename F::arg5_type) *>(a[5])
		);
		*r = QMetaType::construct(qMetaTypeId<RAW_TYPE(typename F::result_type)>(), &v);
	}
};
template <typename F> struct FPCall<5, void, F> {
	template <typename FP> static inline void call(FP f, void **, void **a) {
		(*f)(*reinterpret_cast<RAW_TYPE(typename F::arg1_type) *>(a[1]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg2_type) *>(a[2]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg3_type) *>(a[3]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg4_type) *>(a[4]),
			 *reinterpret_cast<RAW_TYPE(typename F::arg5_type) *>(a[5])
		);
	}
};


// Function implementation for plain old function pointers
template <typename F>
struct FunctionPointer : public Function
{
	typedef typename boost::function_traits<typename boost::remove_pointer<F>::type> type;
	typedef typename type::result_type result_type;

	FunctionPointer(F f) : f(f) { }
	QByteArray signature_in() const {
		return QMetaObject::normalizedSignature(FPSignature<type::arity, result_type, type>::sigin() + ")");
	}
	QByteArray signature_out() const {
		return QMetaObject::normalizedSignature(QByteArray("_proxy_out(") + typeName<result_type>() + ")");
	}
	void call(void **result, void **args) {
		FPCall<type::arity, result_type, type>::call(f, result, args);
	}

private:
	F f;
};


} // namespace QSignalProxyFunctions


#endif // QSIGNALPROXY_FUNCTIONS_H
