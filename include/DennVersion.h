#pragma once
#define VALUE_TO_STR(s) #s
#define DEFINE_TO_STR(s) VALUE_TO_STR(s)

#define DENN_VER_MAJOR   1
#define DENN_VER_MINOR   3
#define DENN_VER_PATCH   7

#define DENN_VER_STR \
		DEFINE_TO_STR(DENN_VER_MAJOR) "." \
		DEFINE_TO_STR(DENN_VER_MINOR) "." \
		DEFINE_TO_STR(DENN_VER_PATCH)

