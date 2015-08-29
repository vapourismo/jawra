/* Jawra
 * Minimal-overhead V8 wrapper for C++
 *
 * Copyright (C) 2015, Ole Krüger <ole@vprsm.de>
 */

#ifndef JAWRA_VALUES_H_
#define JAWRA_VALUES_H_

#include "common.hpp"

#include <string>

JAWRA_NS_BEGIN

template <typename T>
struct ValueWrapper {
	static_assert(sizeof(T) == -1, "Invalid specialization of ValueWrapper");
};

template <>
struct ValueWrapper<uint32_t> {
	static
	constexpr const char* TypeName = "unsigned integer";

	static inline
	bool check(v8::Local<v8::Value> value) {
		return value->IsUint32();
	}

	static inline
	uint32_t unpack(v8::Local<v8::Value> value) {
		return value->Uint32Value();
	}

	static inline
	v8::Local<v8::Integer> pack(v8::Isolate* isolate, uint32_t value) {
		return v8::Integer::NewFromUnsigned(isolate, value);
	}
};

template <>
struct ValueWrapper<int32_t> {
	static
	constexpr const char* TypeName = "signed integer";

	static inline
	bool check(v8::Local<v8::Value> value) {
		return value->IsInt32();
	}

	static inline
	int32_t unpack(v8::Local<v8::Value> value) {
		return value->Int32Value();
	}

	static inline
	v8::Local<v8::Integer> pack(v8::Isolate* isolate, int32_t value) {
		return v8::Integer::New(isolate, value);
	}
};

template <>
struct ValueWrapper<double> {
	static
	constexpr const char* TypeName = "number";

	static inline
	bool check(v8::Local<v8::Value> value) {
		return value->IsNumber();
	}

	static inline
	double unpack(v8::Local<v8::Value> value) {
		return value->NumberValue();
	}

	static inline
	v8::Local<v8::Number> pack(v8::Isolate* isolate, double value) {
		return v8::Number::New(isolate, value);
	}
};

template <>
struct ValueWrapper<std::string> {
	static
	constexpr const char* TypeName = "string";

	static inline
	bool check(v8::Local<v8::Value> value) {
		return value->IsString();
	}

	static inline
	std::string unpack(v8::Local<v8::Value> value) {
		v8::String::Utf8Value value_string(value);
		return std::string(*value_string);
	}

	static inline
	v8::Local<v8::String> pack(v8::Isolate* isolate, const std::string& value) {
		return v8::String::NewFromUtf8(isolate, value.c_str());
	}
};

template <>
struct ValueWrapper<const char*> {
	static inline
	v8::Local<v8::String> pack(v8::Isolate* isolate, const char* value) {
		return v8::String::NewFromUtf8(isolate, value);
	}
};

template <>
struct ValueWrapper<v8::FunctionCallback> {
	static inline
	v8::Local<v8::Function> pack(v8::Isolate* isolate, v8::FunctionCallback callback) {
		return v8::Function::New(isolate, callback);
	}
};

template <typename T>
auto pack(v8::Isolate* isolate, T value) -> decltype(ValueWrapper<T>::pack(isolate, value)) {
	return ValueWrapper<T>::pack(isolate, value);
}

JAWRA_NS_END

#endif
