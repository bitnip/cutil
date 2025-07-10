SOURCE= \
	src/integer/integer.c \
	src/string.c \
	src/list/list.c \
	src/list/node.c \
	src/vector/vector.c \
	src/map/map.c \
	src/generic/generic.c \
	src/resource/file.c \
	src/resource/uri.c \
	src/resource/resource.c \
	src/sort/quick_sort.c \
	src/sort/merge_sort.c \
	src/signal.c
TEST_SOURCE= \
	src/test.c \
	src/integer/integer_test.c \
	src/string_test.c \
	src/list/list_test.c \
	src/vector/vector_test.c\
	src/map/map_test.c \
	src/generic/generic_test.c \
	src/resource/resource_test.c \
	src/resource/uri_test.c \
	src/sort/sort_test.c

COVERAGE_CC=gcc
ifeq ($(shell uname -s),Darwin)
	CC=gcc
endif
ifeq ($(shell uname -s),Linux)
	CC=gcc
endif
ifeq ($(OS),Windows_NT)
	CC=x86_64-w64-mingw32-gcc
endif