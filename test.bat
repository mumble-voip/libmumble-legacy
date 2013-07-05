:: Copyright (c) 2013 The libmumble Developers
:: The use of this source code is goverened by a BSD-style
:: license that can be found in the LICENSE-file.

set GYP=./gyp
set GYP_MSVS_VERSION=2012

python %GYP% libmumble.gyp -f msvs --depth .. -Dlibrary=static_library -Dopenssl_asm=gnuas-x86 -Dtarget_arch=ia32 --generator-out=test
msbuild test\libmumble.sln /m /target:libmumble-test
test\Default\libmumble-test.exe