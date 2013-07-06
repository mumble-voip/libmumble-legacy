# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

{
	'target_defaults': {
		'conditions': [
			['OS == "win"', {
				'target_conditions': [
					['target_arch=="x64"', {
						'msvs_configuration_platform': 'x64',
					}],
				],
			}],
		],
	},
}