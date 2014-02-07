#
# Copyright (c) 1996-2001 by Guardsoft Pty Ltd.
# © 2011-2012 Cray Inc. All Rights Reserved
#
dnl All rights reserved. This program and the accompanying materials
dnl are made available under the terms of the Eclipse Public License v1.0
dnl which accompanies this distribution, and is available at
dnl http://www.eclipse.org/legal/epl-v10.html
dnl

AC_PREREQ([2.63])

AC_INIT([libmi], [1.0.0])

AC_CONFIG_SRCDIR([MIValue.c])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl Turn on Posix extensions
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([foreign -Wall])
LT_INIT

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_PID_T

dnl Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([dup2 memset select strchr strdup strerror strstr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
