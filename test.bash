#!/bin/bash
# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

cmake -G Ninja && ninja && ./mumble-test
