# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

cmake_minimum_required(VERSION 2.8)

project(libmumble)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -fcolor-diagnostics -g -ggdb -O0")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if (DEFINED SYSTEM_GTEST)
	set(GTEST_INCLUDE_DIR /usr/include)
	set(GTEST_ROOT_DIR /usr/src/gtest)
else()
	set(GTEST_CHECKOUT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/gtest)
	set(GTEST_ROOT_DIR ${GTEST_CHECKOUT})
	set(GTEST_INCLUDE_DIR ${GTEST_CHECKOUT}/include)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv/include
	${GTEST_INCLUDE_DIR}
	${GTEST_ROOT_DIR}
)

link_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libuv
)

add_library(mumble
	src/Connection.cpp
	src/Connection_p.cpp
	src/UVBio.cpp
	src/ByteArray.cpp
	src/X509Certificate.cpp
	src/X509Certificate_p.cpp
	src/X509Verifier_openssl.cpp
	src/X509HostnameVerifier.cpp
)

add_executable(mumble-demo
	src/demo.cpp
)
target_link_libraries(mumble-demo mumble uv pthread rt ssl crypto)

file(GLOB TEST_SOURCES "src/*_test.cpp")
add_executable(mumble-test
	${GTEST_ROOT_DIR}/src/gtest-all.cc
	${TEST_SOURCES})
target_link_libraries(mumble-test mumble uv pthread rt ssl crypto)
