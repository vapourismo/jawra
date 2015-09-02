#include <jawra/values.hpp>
#include <jawra/functions.hpp>
#include <jawra/objects.hpp>

#include <node.h>

#include <iostream>
#include <string>
#include <utility>

using namespace std;
using namespace jawra;

void print_field(ObjectWrapper wrapper) {
	if (wrapper.expect<int32_t>("key"))
		std::cout << wrapper.get<int32_t>("key") << std::endl;
}

static
void jawra_examples_init(v8::Handle<v8::Object> module) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	ObjectWrapper module_wrapper(isolate, module);

	module_wrapper.set("test", JAWRA_WRAP_FUNCTION(print_field));
}

NODE_MODULE(jawra_examples, jawra_examples_init)
