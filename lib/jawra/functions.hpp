/* Jawra
 * Minimal-overhead V8 wrapper for C++
 *
 * Copyright (C) 2015, Ole Krüger <ole@vprsm.de>
 */

#ifndef JAWRA_FUNCTIONS_H_
#define JAWRA_FUNCTIONS_H_

#include "common.hpp"

JAWRA_NS_BEGIN

template <typename T>
struct FunctionParameterWrapper {
	static_assert(sizeof(T) == -1, "Invalid specialization of FunctionParameterWrapper");
};

template <typename R, typename T>
struct FunctionParameterWrapper<R(T)> {
	static inline
	bool check(const v8::FunctionCallbackInfo<v8::Value>& info, size_t idx) {
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
	R direct(const v8::FunctionCallbackInfo<v8::Value>& info, size_t idx, F hook, A&&... args) {
		return hook(
			std::forward<A>(args)...,
			ValueWrapper<T>::unpack(info[idx])
		);
	}
};

template <typename R, typename T1, typename... TR>
struct FunctionParameterWrapper<R(T1, TR...)> {
	static inline
	bool check(const v8::FunctionCallbackInfo<v8::Value>& info, size_t idx) {
		return
			ValueWrapper<T1>::check(info[idx]) &&
			FunctionParameterWrapper<R(TR...)>::check(info, idx + 1);
	}

	template <typename F, typename... A> static inline
	R direct(const v8::FunctionCallbackInfo<v8::Value>& info, size_t idx, F hook, A&&... args) {
		return FunctionParameterWrapper<R(TR...)>::direct(
			info,
			idx + 1,
			hook,
			std::forward<A>(args)...,
			ValueWrapper<T1>::unpack(info[idx])
		);
	}
};

template <typename R, typename... A>
struct FunctionWrapper {
	template <R (* function_pointer)(A...)> static inline
	void wrapped(const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();

		if (args.Length() < signed(sizeof...(A))) {
			std::string error_message =
				"Expected " + std::to_string(sizeof...(A)) + " parameters";
			isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));
			return;
		} else if (!FunctionParameterWrapper<R(A...)>::check(args, 0)) {
			return;
		}

		auto return_value = ValueWrapper<R>::pack(
			isolate,
			FunctionParameterWrapper<R(A...)>::direct(
				args,
				0,
				function_pointer
			)
		);
		args.GetReturnValue().Set(return_value);
	}
};

template <>
struct FunctionWrapper<void()> {
	template <void (* function_pointer)()> static inline
	void wrapped(const v8::FunctionCallbackInfo<v8::Value>& args) {
		function_pointer();
	}
};

template <typename... A>
struct FunctionWrapper<void, A...> {
	template <void (* function_pointer)(A...)> static inline
	void wrapped(const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();

		if (args.Length() < signed(sizeof...(A))) {
			std::string error_message =
				"Expected " + std::to_string(sizeof...(A)) + " parameters";
			isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));
			return;
		} else if (!FunctionParameterWrapper<void(A...)>::check(args, 0)) {
			return;
		}

		FunctionParameterWrapper<void(A...)>::direct(
			args,
			0,
			function_pointer
		);
	}
};

template <typename T>
struct SignatureWrapper {
	static_assert(sizeof(T) == -1, "Invalid specialization of SignatureWrapper");
};

template <typename R, typename... A>
struct SignatureWrapper<R(A...)>: FunctionWrapper<R, A...> {};

template <typename R, typename... A>
struct SignatureWrapper<R(*)(A...)>: FunctionWrapper<R, A...> {};

#define JAWRA_WRAP_FUNCTION(fun) \
	(&jawra::SignatureWrapper<decltype(fun)>::template wrapped<fun>)

JAWRA_NS_END

#endif
