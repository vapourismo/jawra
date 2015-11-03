/* Jawra
 * Minimal-overhead V8 wrapper for C++
 *
 * Copyright (C) 2015, Ole Krüger <ole@vprsm.de>
 */

#ifndef JAWRA_FUNCTIONS_H_
#define JAWRA_FUNCTIONS_H_

#include "common.hpp"
#include "values.hpp"

#include <type_traits>

JAWRA_NS_BEGIN

using CallbackInfo = const v8::FunctionCallbackInfo<v8::Value>;

template <typename T>
struct ParameterWraper {
	static_assert(sizeof(T) == -1, "Invalid specialization of ParameterWraper");
};

template <typename R, typename T>
struct ParameterWraper<R(T)> {
	static inline
	bool check(CallbackInfo& info, size_t idx) {
		if (ValueWrapper<T>::check(info[idx])) {
			return true;
		} else {
			v8::Isolate* isolate = info.GetIsolate();

			std::string error_message =
				"Type mismatch for argument #" + std::to_string(idx + 1) + ": Expected " +
				std::string(ValueWrapper<T>::TypeName);
			isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));

			return false;
		}
	}

	template <typename F, typename... A> static inline
	R direct(CallbackInfo& info, size_t idx, F&& hook, A&&... args) {
		return hook(
			std::forward<A>(args)...,
			ValueWrapper<T>::unpack(info[idx])
		);
	}
};

template <typename R, typename T1, typename... TR>
struct ParameterWraper<R(T1, TR...)> {
	static inline
	bool check(CallbackInfo& info, size_t idx) {
		return
			ParameterWraper<R(T1)>::check(info, idx) &&
			ParameterWraper<R(TR...)>::check(info, idx + 1);
	}

	template <typename F, typename... A> static inline
	R direct(CallbackInfo& info, size_t idx, F&& hook, A&&... args) {
		return ParameterWraper<R(TR...)>::direct(
			info,
			idx + 1,
			std::forward<F>(hook),
			std::forward<A>(args)...,
			ValueWrapper<T1>::unpack(info[idx])
		);
	}
};

template <typename... A> static inline
bool verify_parameters(v8::Isolate* isolate, CallbackInfo& args) {
	if (args.Length() < signed(sizeof...(A))) {
		std::string error_message =
			"Expected " + std::to_string(sizeof...(A)) + " parameters";
		isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));
		return false;
	}

	return ParameterWraper<void(A...)>::check(args, 0);
}

template <typename R, typename... A>
struct FunctionWrapper {
	template <R (* function_pointer)(A...), typename... X> static inline
	void wrapped(CallbackInfo& args, X&&... extra) {
		v8::Isolate* isolate = args.GetIsolate();

		if (!verify_parameters<A...>(isolate, args))
			return;

		auto return_value = ValueWrapper<R>::pack(
			isolate,
			ParameterWraper<R(A...)>::direct(
				args,
				0,
				function_pointer,
				std::forward<X>(extra)...
			)
		);
		args.GetReturnValue().Set(return_value);
	}
};

template <>
struct FunctionWrapper<void> {
	template <void (* function_pointer)(), typename... X> static inline
	void wrapped(CallbackInfo& args, X&&... extra) {
		function_pointer(std::forward<X>(extra)...);
	}
};

template <typename... A>
struct FunctionWrapper<void, A...> {
	template <void (* function_pointer)(A...), typename... X> static inline
	void wrapped(CallbackInfo& args, X&&... extra) {
		v8::Isolate* isolate = args.GetIsolate();

		if (!verify_parameters<A...>(isolate, args))
			return;

		ParameterWraper<void(A...)>::direct(
			args,
			0,
			function_pointer,
			std::forward<X>(extra)...
		);
	}
};

template <typename T>
struct FunctionSignatureWrapper {
	static_assert(sizeof(T) == -1, "Invalid specialization of FunctionSignatureWrapper");
};

template <typename R, typename... A>
struct FunctionSignatureWrapper<R(*)(A...)>: FunctionWrapper<R, A...> {};

#define JAWRA_WRAP_FUNCTION(fun) \
	(&jawra::FunctionSignatureWrapper<decltype(&fun)>::template wrapped<&fun>)

JAWRA_NS_END

#endif
