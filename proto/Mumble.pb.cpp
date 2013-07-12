// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

// This file is a redirector that allows us to include
// Mumble.pb.cc into our sources on Android. Gyp insists
// that all of our C++ sources share the same extension.
#include "Mumble.pb.cc"