Summary: A program that computes cryptographic hashes.
Name: Hashish
Version: 1.1a
Release: 1
Copyright: GPL
Group: Development/Tools
Source: Hashish-1.1a.tar.gz
URL: http://hashish.sf.net
Vendor: A. S. Logic Systems Co.
Packager: Jesse Lovelace <jllovela@eos.ncsu.edu>


%description
Hashish allows the user to compute cryptographic hashes on
both files and strings.  With these a user can verify if 
their file has been tampered with or she can hash a phrase
to use as a pass word.  

Many hashing functions are supported including: SHA-1, MD5, 
SHA-2 (256, 384, 512), Panama, RipeMD-160, Tiger, and 
HAVAL (1, 3, 4, 5).  Also, CRC32 is supported for kicks.

%prep
%setup

%build
./configure --enable-static

%install
make install-strip

%files
%doc README COPYING AUTHORS ChangLog
/usr/local/bin/hashish

#%changelog
#* Release 1.1a
#- Fixed hash choice box error
#
#* Release 1.1
#- Fixed FreeBSD ports makefile
#- Added support for CryptoPP 5.1
#
#* Release 1.0 
#- Added support for system tray (win32)
#- Configure script for unix machines.
#- Edit functions for clipboard
#- Better endian support
#- More binary builds (OSX!)
#- More keyboard shortcuts
#
#* Beta 0.4
#- Added drag and drop.
#- Released unix builds
#- Added HAVAL 3-5, CRC32, and Sapphire Hash
#
#* Beta 0.1
#- Initial Release

