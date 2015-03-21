AC_INIT([arek-wm], 0.0, [hjdskes@gmail.com], [arek-wm],
	[https://github.com/Unia/arek-wm/])
AM_INIT_AUTOMAKE([1.11 -Wno-portability no-dist-gzip dist-xz foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CC_STDC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT(/usr/local)

LT_PREREQ(2.2)
LT_INIT([disable-static])

GLIB_GSETTINGS

# Package requirements
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([MUTTER], [libmutter >= 3.14.0])

AC_CONFIG_FILES(Makefile
		data/Makefile
		src/Makefile)

AC_OUTPUT
