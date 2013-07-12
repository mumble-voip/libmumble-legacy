# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

{
	'variables': {
		'use_system_protobuf': 0,
	},
	'target_defaults': {
		'conditions': [
			['OS == "win"', {
				'target_conditions': [
					['target_arch=="x64"', {
						'msvs_configuration_platform': 'x64',
					}],
				],
			}],
			['OS == "mac"', {
				'target_conditions': [
					['target_arch=="x64"', {
						'xcode_settings': {
							'ARCHS': 'x86_64',
						},
					}],
				],
			}],
			['OS == "ios"', {
				'xcode_settings': {
					'SDKROOT': 'iphoneos',
					'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
				},
			}],
		],
	},
}