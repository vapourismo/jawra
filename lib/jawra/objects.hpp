/* Jawra
 * Minimal-overhead V8 wrapper for C++
 *
 * Copyright (C) 2015, Ole Krüger <ole@vprsm.de>
 */

#ifndef JAWRA_OBJECTS_H_
#define JAWRA_OBJECTS_H_

#include "common.hpp"
#include "values.hpp"

JAWRA_NS_BEGIN

struct ObjectWrapper: v8::Handle<v8::Object> {
	v8::Isolate* isolate;
	v8::Object* handle;

	inline
	ObjectWrapper(v8::Isolate* isolate):
		v8::Handle<v8::Object>(v8::Object::New(isolate)),
		isolate(isolate),
		handle(this->operator *())
	{}

	inline
	ObjectWrapper(v8::Isolate* isolate, const v8::Handle<v8::Object>& rhs):
		v8::Handle<v8::Object>(rhs),
		isolate(isolate),
		handle(this->operator *())
	{}

	inline
	ObjectWrapper(const ObjectWrapper& rhs):
		v8::Handle<v8::Object>(rhs),
		isolate(rhs.isolate),
		handle(this->operator *())
	{}

	inline
	ObjectWrapper(ObjectWrapper&& rhs):
		v8::Handle<v8::Object>(rhs),
		isolate(rhs.isolate),
		handle(this->operator *())
	{}

	template <typename K, typename V> inline
	void set(K key, V value) {
		handle->Set(ValueWrapper<K>::pack(isolate, key), ValueWrapper<V>::pack(isolate, value));
	}

	template <typename K>
	bool has(K key) {
		return handle->Has(ValueWrapper<K>::pack(isolate, key));
	}

	template <typename V, typename K>
	bool check(K key) {
		auto value = handle->Get(ValueWrapper<K>::pack(isolate, key));
		return ValueWrapper<V>::check(value);
	}

	template <typename V, typename K>
	bool expect(K key) {
		auto key_handle = ValueWrapper<K>::pack(isolate, key);
		auto value = handle->Get(key_handle);

		if (!handle->Has(key_handle)) {
			v8::String::Utf8Value key_string(key_handle);
			std::string error_message = "Field '" + std::string(*key_string) + "' does not exist";
			isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));

			return false;
		} else if (ValueWrapper<V>::check(value)) {
			return true;
		} else {
			v8::String::Utf8Value key_string(key_handle);
			std::string error_message =
				"Type mismatch for field '" + std::string(*key_string)  + "': Expected " +
				std::string(ValueWrapper<V>::TypeName);
			isolate->ThrowException(v8::Exception::TypeError(pack(isolate, error_message)));

			return false;
		}
	}

	template <typename V, typename K>
	V get(K key) {
		auto value = handle->Get(ValueWrapper<K>::pack(isolate, key));
		return ValueWrapper<V>::unpack(value);
	}
};

template <>
struct ValueWrapper<ObjectWrapper> {
	static
	constexpr const char* TypeName = "object";

	static inline
	bool check(v8::Handle<v8::Value> value) {
		return value->IsObject();
	}

	static inline
	ObjectWrapper unpack(v8::Handle<v8::Value> value) {
		return {v8::Isolate::GetCurrent(), value->ToObject()};
	}

	static inline
	v8::Local<v8::Object> pack(v8::Isolate* isolate, ObjectWrapper value) {
		return value;
	}
};

JAWRA_NS_END

#endif
