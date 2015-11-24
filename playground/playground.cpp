#include <jawra.hpp>

template <typename T>
struct ValueWrapper {
	static
	bool check(v8::Isolate* isolate, v8::Local<v8::Value>);

	static
	T unpack(v8::Isolate* isolate, v8::Local<v8::Value>);

	static
	v8::Local<v8::Value> pack(v8::Isolate* isolate, T);

	static_assert(sizeof(T) == -1, "Invalid specialization of ValueWrapper");
};

static
void playground_init(v8::Local<v8::Object> module) {

}

NODE_MODULE(playground, playground_init)
