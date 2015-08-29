#include <jawra/values.hpp>
#include <jawra/functions.hpp>

#include <node.h>

#include <iostream>
#include <string>
#include <utility>

using namespace std;
using namespace jawra;

int32_t add_test(int32_t a, int32_t b) {
	return a + b;
}

static
void jawra_examples_init(v8::Handle<v8::Object> module) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::FunctionCallback callback = JAWRA_WRAP_FUNCTION(add_test);

	v8::Local<v8::String> key = pack(isolate, "addTest");
	v8::Local<v8::Function> value = pack(isolate, callback);
	module->Set(key, value);
}

NODE_MODULE(jawra_examples, jawra_examples_init)
