// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>

#include "X509HostnameVerifier.h"

#include <cstdlib>
#include <cstring>

TEST(X509HostnameVerifierTest, WildcardMatch) {
	std::string host("www.google.com");
	std::string wexpr("*.google.com");
	EXPECT_TRUE(mumble::X509HostnameVerifier::HostnameExprMatch(wexpr, host));
}

TEST(X509HostnameVerifierTest, WildcardMismatch) {
	std::string host("a.b.example.com");
	std::string wexpr("*.example.com");
	EXPECT_FALSE(mumble::X509HostnameVerifier::HostnameExprMatch(wexpr, host));
}

TEST(X509HostnameVerifierTest, GlobWildcardDisallowed) {
	std::string host("www.google.com");
	std::string wexpr("w*.google.com");
	EXPECT_FALSE(mumble::X509HostnameVerifier::HostnameExprMatch(wexpr, host));
}

TEST(X509HostnameVerifierTest, MultiWildcardMatch) {
	std::string host("a.b.example.com");
	std::string wexpr("*.*.example.com");
	EXPECT_TRUE(mumble::X509HostnameVerifier::HostnameExprMatch(wexpr, host));
}

TEST(X509HostnameVerifierTest, FQDNMismatch) {
	std::string host("www.google.com");
	std::string wexpr("*.google.com.");
	EXPECT_FALSE(mumble::X509HostnameVerifier::HostnameExprMatch(wexpr, host));
}

TEST(X509HostnameVerifierTest, HostVectorSize) {
	auto v1 = mumble::X509HostnameVerifier::LowerCaseHostnameVector(std::string("*.google.com"));
	auto v2 = mumble::X509HostnameVerifier::LowerCaseHostnameVector(std::string("a.b.example.com"));
	EXPECT_EQ(3, v1.size());
	EXPECT_EQ(4, v2.size());
}

TEST(X509HostnameVerifierTest, HostVectorLowerCase) {
	std::vector<std::string> expected;
	expected.push_back(std::string("*"));
	expected.push_back(std::string("google"));
	expected.push_back(std::string("com"));
	auto actual = mumble::X509HostnameVerifier::LowerCaseHostnameVector(std::string("*.GooGlE.CoM"));
	EXPECT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}

TEST(X509HostnameVerifierTest, HostVectorLowerCaseComprehensive) {
	std::vector<std::string> expected;
	expected.push_back(std::string("abcdefghijklmopqrstuvwxyz"));
	auto actual = mumble::X509HostnameVerifier::LowerCaseHostnameVector(std::string("ABCDEFGHIJKLMOPQRSTUVWXYZ"));
	EXPECT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}