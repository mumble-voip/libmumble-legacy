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
			'include_dirs': [
				'include',
				'src',
				'3rdparty/libuv/include'
			],
			'sources': [
				'src/Connection.cpp',
				'src/Connection_p.cpp',
				'src/UVBio.cpp',
				'src/ByteArray.cpp',
				'src/X509Certificate.cpp',
				'src/X509Certificate_p.cpp',
				'src/X509Verifier_openssl.cpp',
				'src/X509HostnameVerifier.cpp',
			],
			'conditions': [
				['OS=="mac"', {
					'xcode_settings': {
						'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
						'CLANG_CXX_LIBRARY': 'libc++',
					},
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
				'3rdparty/libuv/uv.gyp:libuv'
			],
			'include_dirs': [
				'include',
				'src',
				'3rdparty/libuv/include',
				'3rdparty/gtest/include',
				'3rdparty/gtest',
			],
			'link_settings': {
				'libraries': [ '-lcrypto', '-lssl', '-lpthread', '-lrt' ],
			},
			'sources': [
				'3rdparty/gtest/src/gtest-all.cc',
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
				}],
			],
		}
	],
}
