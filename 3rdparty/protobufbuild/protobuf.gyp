# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.
#
# This file is based on protobuf.gyp from the Chromium
# project, which is licensed under the following terms:
#
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.chromium file.

{
  'conditions': [
    ['use_system_protobuf==0', {
      'conditions': [
        ['OS!="win"', {
          'variables': {
            'config_h_dir':
              '.',  # crafted for gcc/linux.
          },
        }, {  # else, OS=="win"
          'variables': {
            'config_h_dir':
              '../protobuf/vsprojects',  # crafted for msvc.
          },
          'target_defaults': {
            'msvs_disabled_warnings': [
              4018,  # signed/unsigned mismatch in comparison
              4244,  # implicit conversion, possible loss of data
              4355,  # 'this' used in base member initializer list
              4267,  # size_t to int truncation
            ],
            'defines!': [
              'WIN32_LEAN_AND_MEAN',  # Protobuf defines this itself.
            ],
          },
        }],
      ],
      'targets': [
        # The "lite" lib is about 1/7th the size of the heavy lib,
        # but it doesn't support some of the more exotic features of
        # protobufs, like reflection.  To generate C++ code that can link
        # against the lite version of the library, add the option line:
        #
        #   option optimize_for = LITE_RUNTIME;
        #
        # to your .proto file.
        {
          'target_name': 'protobuf_lite',
          'type': '<(library)', # was: component
          'sources': [
            '../protobuf/src/google/protobuf/stubs/atomicops.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_arm_gcc.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_atomicword_compat.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_macosx.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_mips_gcc.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_x86_gcc.h',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc',
            '../protobuf/src/google/protobuf/stubs/atomicops_internals_x86_msvc.h',
            '../protobuf/src/google/protobuf/stubs/common.h',
            '../protobuf/src/google/protobuf/stubs/once.h',
            '../protobuf/src/google/protobuf/stubs/platform_macros.h',
            '../protobuf/src/google/protobuf/extension_set.h',
            '../protobuf/src/google/protobuf/generated_message_util.h',
            '../protobuf/src/google/protobuf/message_lite.h',
            '../protobuf/src/google/protobuf/repeated_field.h',
            '../protobuf/src/google/protobuf/unknown_field_set.cc',
            '../protobuf/src/google/protobuf/unknown_field_set.h',
            '../protobuf/src/google/protobuf/wire_format_lite.h',
            '../protobuf/src/google/protobuf/wire_format_lite_inl.h',
            '../protobuf/src/google/protobuf/io/coded_stream.h',
            '../protobuf/src/google/protobuf/io/zero_copy_stream.h',
            '../protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.h',

            '../protobuf/src/google/protobuf/stubs/common.cc',
            '../protobuf/src/google/protobuf/stubs/once.cc',
            '../protobuf/src/google/protobuf/stubs/hash.h',
            '../protobuf/src/google/protobuf/stubs/map-util.h',
            '../protobuf/src/google/protobuf/stubs/stl_util-inl.h',
            '../protobuf/src/google/protobuf/extension_set.cc',
            '../protobuf/src/google/protobuf/generated_message_util.cc',
            '../protobuf/src/google/protobuf/message_lite.cc',
            '../protobuf/src/google/protobuf/repeated_field.cc',
            '../protobuf/src/google/protobuf/wire_format_lite.cc',
            '../protobuf/src/google/protobuf/io/coded_stream.cc',
            '../protobuf/src/google/protobuf/io/coded_stream_inl.h',
            '../protobuf/src/google/protobuf/io/zero_copy_stream.cc',
            '../protobuf/src/google/protobuf/io/zero_copy_stream_impl_lite.cc',
            '<(config_h_dir)/config.h',
          ],
          'include_dirs': [
            '<(config_h_dir)',
            '../protobuf/src',
          ],
          'defines': [
            # This macro must be defined to suppress the use of dynamic_cast<>,
            # which requires RTTI.
            'GOOGLE_PROTOBUF_NO_RTTI',
            'GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER',

            'LIBPROTOBUF_EXPORTS',
            'PROTOBUF_USE_DLLS',
          ],
          'direct_dependent_settings': {
            'include_dirs': [
              '<(config_h_dir)',
              '../protobuf/src',
            ],
            'defines': [
              'PROTOBUF_USE_DLLS'
              'GOOGLE_PROTOBUF_NO_RTTI',
              'GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER',
            ],
            # TODO(jschuh): http://crbug.com/167187 size_t -> int.
            'msvs_disabled_warnings': [ 4267 ],
          },
          # Required for component builds. See http://crbug.com/172800.
          'defines': [
            'LIBPROTOBUF_EXPORTS',
            'PROTOBUF_USE_DLLS',
          ],
        },
      ],
    }, { # use_system_protobuf==1
      'targets': [
        {
          'target_name': 'protobuf_lite',
          'type': 'none',
          'direct_dependent_settings': {
            'cflags': [
              # Use full protobuf, because vanilla protobuf doesn't have
              # our custom patch to retain unknown fields in lite mode.
              '<!@(pkg-config --cflags protobuf)',
            ],
            'defines': [
              'USE_SYSTEM_PROTOBUF',

              # This macro must be defined to suppress the use
              # of dynamic_cast<>, which requires RTTI.
              'GOOGLE_PROTOBUF_NO_RTTI',
              'GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER',
            ],
          },
          'link_settings': {
            # Use full protobuf, because vanilla protobuf doesn't have
            # our custom patch to retain unknown fields in lite mode.
            'ldflags': [
              '<!@(pkg-config --libs-only-L --libs-only-other protobuf)',
            ],
            'libraries': [
              '<!@(pkg-config --libs-only-l protobuf)',
            ],
          },
        },
      ],
    }],
  ],
}