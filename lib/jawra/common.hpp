/* Jawra
 * Minimal-overhead V8 wrapper for C++
 *
 * Copyright (C) 2015, Ole Krüger <ole@vprsm.de>
 */

#ifndef JAWRA_COMMON_H_
#define JAWRA_COMMON_H_

// Check C++ version
#if !defined(__cplusplus) || __cplusplus < 201402L
	#error You need a C++14 compliant compiler
#endif

#include <v8.h>

// Namespaces
#define JAWRA_NS_BEGIN namespace jawra {
#define JAWRA_NS_END }

// Version MAJOR.MINOR.PATH
#define JAWRA_VERSION_MAJOR 0
#define JAWRA_VERSION_MINOR 1

#endif
