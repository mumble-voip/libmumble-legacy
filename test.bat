:: Copyright (c) 2013 The libmumble Developers
:: The use of this source code is goverened by a BSD-style
:: license that can be found in the LICENSE-file.

set GYP=./3rdparty/gyp/gyp_main.py
set GYPFLAGS=-I common.gypi
set GYP_MSVS_VERSION=2013

python %GYP% libmumble.gyp %GYPFLAGS% -f msvs --depth .. -Dlibrary=static_library -Dopenssl_asm=gnuas-x86_64 -Dtarget_arch=x64 --generator-out=test
msbuild test\libmumble.sln /m /target:libmumble-test
test\Default\libmumble-test.exe