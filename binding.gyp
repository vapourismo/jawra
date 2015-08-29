{
	"targets": [
		{
			"target_name": "jawra_examples",
			"sources": [
				"examples/values.cpp",
			],
			"cflags": [
				"-std=c++14",
				"-O2",
				"-g",
				"-DDEBUG",
				"-Wall",
				"-Wextra",
				"-pedantic",
				"-fmessage-length=0",
				"-Wno-unused-parameter",
				"-D_GLIBCXX_USE_C99",
				"-I../lib"
			],
			"ldflags": [
			]
		}
	]
}
