Summary: Prints sccs tokens found in files.
Name: what
Version: 1.0.1
Release: 1
Copyright: GPL
Group: Applications/File
Source: what-1.0.1.tar.gz

%description
This program finds and prints sccs tokens found in files.

%prep
%setup -q

%build
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin

install -s -m 755 what $RPM_BUILD_ROOT/usr/bin/what

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc README COPYING

/usr/bin/what

%changelog
* Mon Aug 21 2000 Scott Nelson <sbnelson@thermeon.com>
- Initial version
