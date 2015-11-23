{
	"targets": [
		{
			"target_name": "playground",
			"sources": ["playground.cpp"],
			"cflags": [
				"-O0",
				"-g",
				"-Wall",
				"-Wextra",
				"-pedantic",
				"-fmessage-length=0",
				"-Wno-unused-parameter",
				"-Wno-unused-result",
				"-Wno-missing-field-initializers",
				"-D_GLIBCXX_USE_C99",
				"-I../../lib"
			]
		}
	]
}
