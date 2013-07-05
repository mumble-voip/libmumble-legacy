# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.
#
# This file is based on yasm_compile.gypi from Chromium:
#
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.chromium file.

# This is an gyp include to use YASM for compiling GNU AS assembly files on Windows.
# Files to be compiled with YASM should have an extension of .S.
#
# Sample usage:
# 'sources': [
#   'ultra_optimized_awesome.S',
# ],
# 'includes': [
#   '3rdparty/yasm/yasm_gas.gypi'
# ],

{
  'variables': {
    'yasm_flags': [],
    'yasm_includes': [],
    'yasm_output_path': '<(SHARED_INTERMEDIATE_DIR)/project',

    # An ugly fix for some gyp brain damage. If we do
    # bla\bla\3rdparty here, gyp, or something, I don't
    # know what, will have the value escaped as an
    # ASCII decimal 3 once it hits the Visual Studio
    # project file. So we apply some escaping grease.
    # Yuck!
    'yasm_path': '<(DEPTH)\\\x33rdparty\yasm\yasm.exe',
    'asm_obj_extension': 'obj',

    'conditions': [
      [ 'OS=="win" and target_arch=="ia32"', {
        'yasm_flags': [
          '--prefix=_',
          '-fwin32',
          '-mx86',
          '-pgas',
        ],
      } ],
      [ 'OS=="win" and target_arch=="x64"', {
        'yasm_flags': [
          '-fwin64',
          '-mamd64',
          '-pgas'
        ],
      }],
    ],
  },  # variables

  'rules': [
    {
      'rule_name': 'assemble',
      'extension': 'S',
      'inputs': [ '<(yasm_path)', '<@(yasm_includes)'],
      'outputs': [
        '<(yasm_output_path)/<(RULE_INPUT_ROOT).<(asm_obj_extension)',
      ],
      'action': [
        '<(yasm_path)',
        '<@(yasm_flags)',
        '-o', '<(yasm_output_path)/<(RULE_INPUT_ROOT).<(asm_obj_extension)',
        '<(RULE_INPUT_PATH)',
      ],
      'process_outputs_as_sources': 1,
      'message': 'Compile assembly <(RULE_INPUT_PATH).',
      'msvs_cygwin_shell': 0,
    },
  ],  # rules
}