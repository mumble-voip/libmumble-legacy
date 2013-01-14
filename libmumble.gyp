# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

{
	'targets': [
		{
			'target_name':  'libmumble',
			'product_name': 'mumble',
			'type':         'static_library',
			'cflags_cc':    ['-std=c++11'],
			'dependencies': [
				'3rdparty/libuv/uv.gyp:libuv',
				'3rdparty/opensslbuild/OpenSSL.gyp:libcrypto',
				'3rdparty/opensslbuild/OpenSSL.gyp:libssl',
			],
			'include_dirs': [
				'include',
				'src',
				'3rdparty/libuv/include',
				'3rdparty/openssl/include',
			],
			'sources': [
				'src/Connection.cpp',
				'src/Connection_p.cpp',
				'src/UVBio.cpp',
				'src/ByteArray.cpp',
				'src/X509Certificate.cpp',
				'src/X509Certificate_p.cpp',
				'src/X509PEMVerifier.cpp',
				'src/X509Verifier_openssl.cpp',
				'src/X509HostnameVerifier.cpp',
			],
			'conditions': [
				['OS=="linux"', {
					'defines': ['LIBMUMBLE_OS_LINUX'],
					'link_settings': {
						'libraries': [ '-lpthread', '-lrt' ],
					},
				}],
				['OS=="mac"', {
					'defines': ['LIBMUMBLE_OS_MAC'],
					'xcode_settings': {
						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
						'CLANG_CXX_LIBRARY': 'libc++',
					},
					'link_settings': {
						'libraries': [
							'$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
							'$(SDKROOT)/System/Library/Frameworks/Security.framework',
							'$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
						],
					},
					'sources!': [
						'src/X509Verifier_openssl.cpp',
					],
					'sources': [
						'src/X509Verifier_mac.mm',
					],
				}],
				['OS=="android"', {
					'defines': [ 'LIBMUMBLE_OS_ANDROID', '__STDC_LIMIT_MACROS' ],
					'sources': [
						'src/Compat_android.cpp',
					],
				}],
			],
		},
		{
			'target_name':   'libmumble-test',
			'product_name':  'libmumble-test',
			'type':          'executable',
			'cflags_cc':     ['-std=c++11'],
			'dependencies':  [
				'libmumble',
			],
			'include_dirs': [
				'include',
				'src',
				'3rdparty/libuv/include',
				'3rdparty/openssl/include',
				'3rdparty/gtest/include',
				'3rdparty/gtest',
			],
			'sources': [
				'src/ByteArray_test.cpp',
				'src/mumble_test.cpp',
				'src/X509Certificate_test.cpp',
				'src/X509HostnameVerifier_test.cpp',
				'src/X509Verifier_test.cpp',
			],
			'conditions': [
				['OS=="mac"', {
					'xcode_settings': {
						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
						'CLANG_CXX_LIBRARY': 'libc++',
					},
					'defines': [
						'GTEST_HAS_TR1_TUPLE=0',
						'GTEST_USE_OWN_TR1_TUPLE=1',
					],
				}],
				['OS=="android"', {
					'defines': ['__STDC_LIMIT_MACROS' ],
					'sources': [
						'src/Compat_android_test.cpp',
					],
				}],
			],
		},
	],
}
