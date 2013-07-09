# Copyright Joyent, Inc. and other Node contributors. All rights reserved.
# Licensed under the terms of the MIT license (see ../libuv/LICENSE for more information).

{
  'variables': {
    'uv_use_dtrace%': 'false',
    # uv_parent_path is the relative path to libuv in the parent project
    # this is only relevant when dtrace is enabled and libuv is a child project
    # as it's necessary to correctly locate the object files for post
    # processing.
    # XXX gyp is quite sensitive about paths with double / they don't normalize
    'uv_parent_path': '/',
  },

  'target_defaults': {
    'conditions': [
      ['OS != "win"', {
        'defines': [
          '_LARGEFILE_SOURCE',
          '_FILE_OFFSET_BITS=64',
          '_GNU_SOURCE',
        ],
        'conditions': [
          ['OS=="solaris"', {
            'cflags': [ '-pthreads' ],
          }],
          ['OS not in "solaris android"', {
            'cflags': [ '-pthread' ],
          }],
        ],
      }],
    ],
  },

  'targets': [
    {
      'target_name': 'libuv',
      'type': '<(library)',
      'include_dirs': [
        '../libuv/include',
        '../libuv/src/',
      ],
      'direct_dependent_settings': {
        'include_dirs': [ '../libuv/include' ],
        'conditions': [
          ['OS != "win"', {
            'defines': [
              '_LARGEFILE_SOURCE',
              '_FILE_OFFSET_BITS=64',
            ],
          }],
          ['OS == "mac"', {
            'defines': [ '_DARWIN_USE_64_BIT_INODE=1' ],
          }],
          ['OS == "linux"', {
            'defines': [ '_POSIX_C_SOURCE=200112' ],
          }],
        ],
      },
      'defines': [
        'HAVE_CONFIG_H'
      ],
      'sources': [
        '../libuv/common.gypi',
        '../libuv/include/uv.h',
        '../libuv/include/tree.h',
        '../libuv/src/fs-poll.c',
        '../libuv/src/inet.c',
        '../libuv/src/queue.h',
        '../libuv/src/uv-common.c',
        '../libuv/src/uv-common.h',
        '../libuv/src/version.c'
      ],
      'conditions': [
        [ 'OS=="win"', {
          'defines': [
            '_WIN32_WINNT=0x0600',
            '_GNU_SOURCE',
          ],
          'sources': [
            '../libuv/include/uv-win.h',
            '../libuv/src/win/async.c',
            '../libuv/src/win/atomicops-inl.h',
            '../libuv/src/win/core.c',
            '../libuv/src/win/dl.c',
            '../libuv/src/win/error.c',
            '../libuv/src/win/fs.c',
            '../libuv/src/win/fs-event.c',
            '../libuv/src/win/getaddrinfo.c',
            '../libuv/src/win/handle.c',
            '../libuv/src/win/handle-inl.h',
            '../libuv/src/win/internal.h',
            '../libuv/src/win/loop-watcher.c',
            '../libuv/src/win/pipe.c',
            '../libuv/src/win/thread.c',
            '../libuv/src/win/poll.c',
            '../libuv/src/win/process.c',
            '../libuv/src/win/process-stdio.c',
            '../libuv/src/win/req.c',
            '../libuv/src/win/req-inl.h',
            '../libuv/src/win/signal.c',
            '../libuv/src/win/stream.c',
            '../libuv/src/win/stream-inl.h',
            '../libuv/src/win/tcp.c',
            '../libuv/src/win/tty.c',
            '../libuv/src/win/threadpool.c',
            '../libuv/src/win/timer.c',
            '../libuv/src/win/udp.c',
            '../libuv/src/win/util.c',
            '../libuv/src/win/winapi.c',
            '../libuv/src/win/winapi.h',
            '../libuv/src/win/winsock.c',
            '../libuv/src/win/winsock.h',
          ],
          'link_settings': {
            'libraries': [
              '-ladvapi32.lib',
              '-liphlpapi.lib',
              '-lpsapi.lib',
              '-lshell32.lib',
              '-lws2_32.lib'
            ],
          },
        }, { # Not Windows i.e. POSIX
          'cflags': [
            '-g',
            '--std=gnu89',
            '-pedantic',
            '-Wall',
            '-Wextra',
            '-Wno-unused-parameter',
          ],
          'sources': [
            '../libuv/include/uv-unix.h',
            '../libuv/include/uv-linux.h',
            '../libuv/include/uv-sunos.h',
            '../libuv/include/uv-darwin.h',
            '../libuv/include/uv-bsd.h',
            '../libuv/src/unix/async.c',
            '../libuv/src/unix/core.c',
            '../libuv/src/unix/dl.c',
            '../libuv/src/unix/error.c',
            '../libuv/src/unix/fs.c',
            '../libuv/src/unix/getaddrinfo.c',
            '../libuv/src/unix/internal.h',
            '../libuv/src/unix/loop.c',
            '../libuv/src/unix/loop-watcher.c',
            '../libuv/src/unix/pipe.c',
            '../libuv/src/unix/poll.c',
            '../libuv/src/unix/process.c',
            '../libuv/src/unix/signal.c',
            '../libuv/src/unix/stream.c',
            '../libuv/src/unix/tcp.c',
            '../libuv/src/unix/thread.c',
            '../libuv/src/unix/threadpool.c',
            '../libuv/src/unix/timer.c',
            '../libuv/src/unix/tty.c',
            '../libuv/src/unix/udp.c',
          ],
          'link_settings': {
            'libraries': [ '-lm' ],
            'conditions': [
              ['OS=="solaris"', {
                'ldflags': [ '-pthreads' ],
              }],
              ['OS != "solaris" and OS != "android"', {
                'ldflags': [ '-pthread' ],
              }],
            ],
          },
          'conditions': [
            ['library=="shared_library"', {
              'cflags': [ '-fPIC' ],
            }],
            ['library=="shared_library" and OS!="mac"', {
              'link_settings': {
                # Must correspond with UV_VERSION_MAJOR and UV_VERSION_MINOR
                # in src/version.c
                'libraries': [ '-Wl,-soname,libuv.so.0.11' ],
              },
            }],
          ],
        }],
        [ 'OS in "linux mac android"', {
          'sources': [ '../libuv/src/unix/proctitle.c' ],
        }],
        [ 'OS=="mac"', {
          'sources': [
            '../libuv/src/unix/darwin.c',
            '../libuv/src/unix/fsevents.c',
            '../libuv/src/unix/darwin-proctitle.c',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreServices.framework',
              '$(SDKROOT)/System/Library/Frameworks/ApplicationServices.framework',
            ],
          },
          'defines': [
            '_DARWIN_USE_64_BIT_INODE=1',
          ]
        }],
        [ 'OS!="mac"', {
          # Enable on all platforms except OS X. The antique gcc/clang that
          # ships with Xcode emits waaaay too many false positives.
          'cflags': [ '-Wstrict-aliasing' ],
        }],
        [ 'OS=="linux"', {
          'sources': [
            '../libuv/src/unix/linux-core.c',
            '../libuv/src/unix/linux-inotify.c',
            '../libuv/src/unix/linux-syscalls.c',
            '../libuv/src/unix/linux-syscalls.h',
          ],
          'link_settings': {
            'libraries': [ '-ldl', '-lrt' ],
          },
        }],
        [ 'OS=="android"', {
          'sources': [
            '../libuv/src/unix/linux-core.c',
            '../libuv/src/unix/linux-inotify.c',
            '../libuv/src/unix/linux-syscalls.c',
            '../libuv/src/unix/linux-syscalls.h',
            '../libuv/src/unix/pthread-fixes.c',
          ],
          'link_settings': {
            'libraries': [ '-ldl' ],
          },
        }],
        [ 'OS=="solaris"', {
          'sources': [ '../libuv/src/unix/sunos.c' ],
          'defines': [
            '__EXTENSIONS__',
            '_XOPEN_SOURCE=500',
          ],
          'link_settings': {
            'libraries': [
              '-lkstat',
              '-lnsl',
              '-lsendfile',
              '-lsocket',
            ],
          },
        }],
        [ 'OS=="aix"', {
          'include_dirs': [ '../libuv/src/ares/config_aix' ],
          'sources': [ '../libuv/src/unix/aix.c' ],
          'defines': [
            '_ALL_SOURCE',
            '_XOPEN_SOURCE=500',
          ],
          'link_settings': {
            'libraries': [
              '-lperfstat',
            ],
          },
        }],
        [ 'OS=="freebsd" or OS=="dragonflybsd"', {
          'sources': [ '../libuv/src/unix/freebsd.c' ],
        }],
        [ 'OS=="openbsd"', {
          'sources': [ '../libuv/src/unix/openbsd.c' ],
        }],
        [ 'OS=="netbsd"', {
          'sources': [ '../libuv/src/unix/netbsd.c' ],
        }],
        [ 'OS in "freebsd dragonflybsd openbsd netbsd".split()', {
          'link_settings': {
            'libraries': [ '-lkvm' ],
          },
        }],
        [ 'OS in "mac freebsd dragonflybsd openbsd netbsd".split()', {
          'sources': [ '../libuv/src/unix/kqueue.c' ],
        }],
        ['library=="shared_library"', {
          'defines': [ 'BUILDING_UV_SHARED=1' ]
        }],
        ['uv_use_dtrace=="true"', {
          'defines': [ 'HAVE_DTRACE=1' ],
          'dependencies': [ 'uv_dtrace_header' ],
          'include_dirs': [ '<(SHARED_INTERMEDIATE_DIR)' ],
          'conditions': [
            ['OS != "mac"', {
              'sources': ['../libuv/src/unix/dtrace.c' ],
            }],
          ],
        }],
      ]
    },

    {
      'target_name': 'run-tests',
      'type': 'executable',
      'dependencies': [ 'libuv' ],
      'sources': [
        '../libuv/test/blackhole-server.c',
        '../libuv/test/echo-server.c',
        '../libuv/test/run-tests.c',
        '../libuv/test/runner.c',
        '../libuv/test/runner.h',
        '../libuv/test/test-get-loadavg.c',
        '../libuv/test/task.h',
        '../libuv/test/test-util.c',
        '../libuv/test/test-active.c',
        '../libuv/test/test-async.c',
        '../libuv/test/test-callback-stack.c',
        '../libuv/test/test-callback-order.c',
        '../libuv/test/test-connection-fail.c',
        '../libuv/test/test-cwd-and-chdir.c',
        '../libuv/test/test-delayed-accept.c',
        '../libuv/test/test-error.c',
        '../libuv/test/test-embed.c',
        '../libuv/test/test-fail-always.c',
        '../libuv/test/test-fs.c',
        '../libuv/test/test-fs-event.c',
        '../libuv/test/test-get-currentexe.c',
        '../libuv/test/test-get-memory.c',
        '../libuv/test/test-getaddrinfo.c',
        '../libuv/test/test-getsockname.c',
        '../libuv/test/test-hrtime.c',
        '../libuv/test/test-idle.c',
        '../libuv/test/test-ipc.c',
        '../libuv/test/test-ipc-send-recv.c',
        '../libuv/test/test-list.h',
        '../libuv/test/test-loop-handles.c',
        '../libuv/test/test-loop-stop.c',
        '../libuv/test/test-walk-handles.c',
        '../libuv/test/test-multiple-listen.c',
        '../libuv/test/test-osx-select.c',
        '../libuv/test/test-pass-always.c',
        '../libuv/test/test-ping-pong.c',
        '../libuv/test/test-pipe-bind-error.c',
        '../libuv/test/test-pipe-connect-error.c',
        '../libuv/test/test-platform-output.c',
        '../libuv/test/test-poll.c',
        '../libuv/test/test-poll-close.c',
        '../libuv/test/test-process-title.c',
        '../libuv/test/test-ref.c',
        '../libuv/test/test-run-nowait.c',
        '../libuv/test/test-run-once.c',
        '../libuv/test/test-semaphore.c',
        '../libuv/test/test-shutdown-close.c',
        '../libuv/test/test-shutdown-eof.c',
        '../libuv/test/test-signal.c',
        '../libuv/test/test-signal-multiple-loops.c',
        '../libuv/test/test-spawn.c',
        '../libuv/test/test-fs-poll.c',
        '../libuv/test/test-stdio-over-pipes.c',
        '../libuv/test/test-tcp-bind-error.c',
        '../libuv/test/test-tcp-bind6-error.c',
        '../libuv/test/test-tcp-close.c',
        '../libuv/test/test-tcp-close-while-connecting.c',
        '../libuv/test/test-tcp-connect-error-after-write.c',
        '../libuv/test/test-tcp-shutdown-after-write.c',
        '../libuv/test/test-tcp-flags.c',
        '../libuv/test/test-tcp-connect-error.c',
        '../libuv/test/test-tcp-connect-timeout.c',
        '../libuv/test/test-tcp-connect6-error.c',
        '../libuv/test/test-tcp-open.c',
        '../libuv/test/test-tcp-write-to-half-open-connection.c',
        '../libuv/test/test-tcp-writealot.c',
        '../libuv/test/test-tcp-unexpected-read.c',
        '../libuv/test/test-tcp-read-stop.c',
        '../libuv/test/test-threadpool.c',
        '../libuv/test/test-threadpool-cancel.c',
        '../libuv/test/test-mutexes.c',
        '../libuv/test/test-thread.c',
        '../libuv/test/test-barrier.c',
        '../libuv/test/test-condvar.c',
        '../libuv/test/test-timer-again.c',
        '../libuv/test/test-timer-from-check.c',
        '../libuv/test/test-timer.c',
        '../libuv/test/test-tty.c',
        '../libuv/test/test-udp-dgram-too-big.c',
        '../libuv/test/test-udp-ipv6.c',
        '../libuv/test/test-udp-open.c',
        '../libuv/test/test-udp-options.c',
        '../libuv/test/test-udp-send-and-recv.c',
        '../libuv/test/test-udp-multicast-join.c',
        '../libuv/test/test-dlerror.c',
        '../libuv/test/test-udp-multicast-ttl.c',
        '../libuv/test/test-ip6-addr.c',
      ],
      'conditions': [
        [ 'OS=="win"', {
          'sources': [
            '../libuv/test/runner-win.c',
            '../libuv/test/runner-win.h'
          ],
          'libraries': [ 'ws2_32.lib' ]
        }, { # POSIX
          'defines': [ '_GNU_SOURCE' ],
          'sources': [
            '../libuv/test/runner-unix.c',
            '../libuv/test/runner-unix.h',
          ],
        }],
        [ 'OS=="solaris"', { # make test-fs.c compile, needs _POSIX_C_SOURCE
          'defines': [
            '__EXTENSIONS__',
            '_XOPEN_SOURCE=500',
          ],
        }],
        [ 'OS=="aix"', {     # make test-fs.c compile, needs _POSIX_C_SOURCE
          'defines': [
            '_ALL_SOURCE',
            '_XOPEN_SOURCE=500',
          ],
        }],
      ],
      'msvs-settings': {
        'VCLinkerTool': {
          'SubSystem': 1, # /subsystem:console
        },
      },
    },

    {
      'target_name': 'run-benchmarks',
      'type': 'executable',
      'dependencies': [ 'libuv' ],
      'sources': [
        '../libuv/test/benchmark-async.c',
        '../libuv/test/benchmark-async-pummel.c',
        '../libuv/test/benchmark-fs-stat.c',
        '../libuv/test/benchmark-getaddrinfo.c',
        '../libuv/test/benchmark-list.h',
        '../libuv/test/benchmark-loop-count.c',
        '../libuv/test/benchmark-million-async.c',
        '../libuv/test/benchmark-million-timers.c',
        '../libuv/test/benchmark-multi-accept.c',
        '../libuv/test/benchmark-ping-pongs.c',
        '../libuv/test/benchmark-pound.c',
        '../libuv/test/benchmark-pump.c',
        '../libuv/test/benchmark-sizes.c',
        '../libuv/test/benchmark-spawn.c',
        '../libuv/test/benchmark-thread.c',
        '../libuv/test/benchmark-tcp-write-batch.c',
        '../libuv/test/benchmark-udp-pummel.c',
        '../libuv/test/dns-server.c',
        '../libuv/test/echo-server.c',
        '../libuv/test/blackhole-server.c',
        '../libuv/test/run-benchmarks.c',
        '../libuv/test/runner.c',
        '../libuv/test/runner.h',
        '../libuv/test/task.h',
      ],
      'conditions': [
        [ 'OS=="win"', {
          'sources': [
            '../libuv/test/runner-win.c',
            '../libuv/test/runner-win.h',
          ],
          'libraries': [ 'ws2_32.lib' ]
        }, { # POSIX
          'defines': [ '_GNU_SOURCE' ],
          'sources': [
            '../libuv/test/runner-unix.c',
            '../libuv/test/runner-unix.h',
          ]
        }]
      ],
      'msvs-settings': {
        'VCLinkerTool': {
          'SubSystem': 1, # /subsystem:console
        },
      },
    },

    {
      'target_name': 'uv_dtrace_header',
      'type': 'none',
      'conditions': [
        [ 'uv_use_dtrace=="true"', {
          'actions': [
            {
              'action_name': 'uv_dtrace_header',
              'inputs': [ '../libuv/src/unix/uv-dtrace.d' ],
              'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/uv-dtrace.h' ],
              'action': [ 'dtrace', '-h', '-xnolibs', '-s', '<@(_inputs)',
                '-o', '<@(_outputs)' ],
            },
          ],
        }],
      ],
    },

    {
      'target_name': 'uv_dtrace_provider',
      'type': 'none',
      'conditions': [
        [ 'uv_use_dtrace=="true" and OS!="mac"', {
          'actions': [
            {
              'action_name': 'uv_dtrace_o',
              'inputs': [
                '../libuv/src/unix/uv-dtrace.d',
                '<(PRODUCT_DIR)/obj.target/libuv<(uv_parent_path)src/unix/core.o',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/obj.target/libuv<(uv_parent_path)src/unix/dtrace.o',
              ],
              'action': [ 'dtrace', '-G', '-xnolibs', '-s', '<@(_inputs)',
                '-o', '<@(_outputs)' ]
            }
          ]
        } ]
      ]
    },

  ]
}
