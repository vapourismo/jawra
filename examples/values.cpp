#include <jawra.hpp>
#include <node.h>

#include <iostream>
#include <string>
#include <utility>

using namespace std;
using namespace jawra;

int32_t foo(int32_t a, int32_t b) {
	return a + b;
}

static
void jawra_examples_init(v8::Local<v8::Object> module) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	ObjectWrapper module_wrapper(isolate, module);

	module_wrapper.set("foo", JAWRA_WRAP_FUNCTION(foo));
}

NODE_MODULE(jawra_examples, jawra_examples_init)
