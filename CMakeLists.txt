project(DSTools)

add_compile_options(
	-nostdinc
)

add_darling_executable(dseditgroup ugtools/dseditgroup_stub.c)
install(TARGETS dseditgroup DESTINATION libexec/darling/usr/sbin)

add_darling_executable(dscl dscl/dscl_stub.c)
install(TARGETS dscl DESTINATION libexec/darling/usr/bin)

install(FILES
	dirt/dirt.1
	dscacheutil/dscacheutil.1
	dscacheutil/lookupd.1
	dscl/dscl.1
	dsconfigldap/dsconfigldap.1
	dsmbrutil/dsmemberutil.1
	dsmbrutil/memberd.1

	DESTINATION libexec/darling/usr/share/man/man1
)

install(FILES
	dsenableroot/dsenableroot.8
	dserr/dserr.8
	pwpolicy/pwpolicy.8
	ugtools/dseditgroup.8

	DESTINATION libexec/darling/usr/share/man/man8
)

install(PROGRAMS
	java_home/java_home
	DESTINATION libexec/darling/usr/libexec
)
