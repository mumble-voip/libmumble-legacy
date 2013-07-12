# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.
#
# This file is originally based on protoc.gypi from the
# Chromium project, which is licensed under the following terms:
#
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.chromium file.

# This file is meant to be included into a target to provide a rule
# which will invoke protoc on any *.proto files found in the target's
# sources. This is needed when we're running with a system-installed
# protobuf, and therefore cannot use our own pre-generated .pb.cc and
# .pb.h files because we cannot be sure that they're compatible with
# the sytem's libprotobuf.
#
# To use this generator to generate protoc products that are compatible
# with the system's protobuf library, add this gypi file to the target's
# includes, and add the .proto files you whish to process to the target's
# sources like below:
#
# {
#   'target_name': 'foo',
#   'type': 'executable',
#   'sources': [
#     'foo.proto',
#     'bar.proto',
#   ],
#   'includes': ['3rdparty/protobufbuild/protoc.gypi'],
# }
#
# The generated products will automatically be added to the target's sources,
# and the protoc compiler is guaranteed to run before compiling any other
# sources of the target.

{
  'variables': {
    'cc_dir': '<(SHARED_INTERMEDIATE_DIR)/protoc_out/',
    'cc_generator_options%': '',
    'cc_include%': '',
    'protoc': '<!(which protoc)',
  },
  'rules': [
    {
      'rule_name': 'genproto',
      'extension': 'proto',
      'inputs': [
        '<(protoc)',
      ],
      'outputs': [
        '<(cc_dir)/<(RULE_INPUT_ROOT).pb.cc',
        '<(cc_dir)/<(RULE_INPUT_ROOT).pb.h',
      ],
      'action': [
        '<(protoc)',
        '<(RULE_INPUT_PATH)',
        '--proto_path', 'proto', # hack! this should be guesstimated from RULE_INPUT_PATH.
        '--cpp_out', '<(cc_generator_options)<(cc_dir)',
      ],
      'msvs_cygwin_shell': 0,
      'message': 'Generating C++ code from <(RULE_INPUT_PATH)',
      'process_outputs_as_sources': 1,
    },
  ],
  'include_dirs': [
    '<(SHARED_INTERMEDIATE_DIR)/protoc_out',
  ],
  'direct_dependent_settings': {
    'include_dirs': [
      '<(SHARED_INTERMEDIATE_DIR)/protoc_out',
    ],
  },
  'export_dependent_settings': [
    # The generated headers reference headers within protobuf_lite,
    # so dependencies must be able to find those headers too.
    '<(DEPTH)/3rdparty/protobufbuild/protobuf.gyp:protobuf_lite',
  ],
  # This target exports a hard dependency because it generates header
  # files.
  'hard_dependency': 1,
}
