#!/usr/bin/env python
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

import sys
import os

srcroot = os.path.dirname(os.path.abspath(sys.argv[0]))
gyplib = os.path.join(srcroot, '3rdparty', 'gyp', 'pylib')
sys.path.insert(0, gyplib)

try:
	import gyp
except ImportError, e:
	print 'Unable to import the gyp module.'
	print 'Are you sure you have checked out all libmumble submodules?'
	print 'Please run:'
	print ' $ git submodule init'
	print ' $ git submodule update'
	sys.exit(1)

sys.exit(gyp.main(sys.argv[1:]))