Summary: Object Manager for Oracle databases.
Name: objectmanager
Version: 2.0
Release: 1
Copyright: GPL
Group: Applications/Databases
Source: ftp://oracle.mattshouse.com/pub/objectmanager-2.0-1.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
AutoReqProv: no

%description
Object Manager is a GTK application that allows a DBA
or end user to access and manage objects on an Oracle
database.

%prep 
%setup -q

%build
./configure
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
install -s -m 755 objectman /$RPM_BUILD_ROOT/usr/local/bin/objectman

%clean
rm -rf $RPM_BUILD_ROOT

%files

%defattr(-,root,root)
%doc README TODO COPYING ChangeLog

/usr/local/bin/objectman

%changelog

%pre

%post
