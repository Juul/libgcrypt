# Copyright (C) 2001, 2002, 2003, 2005 Free Software Foundation, Inc.
#
# This file is part of Libgcrypt.
#
# Libgcrypt is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of
# the License, or (at your option) any later version.
#
# Libgcrypt is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA

## Process this file with automake to produce Makefile.in

TESTS = version t-mpi-bit t-mpi-point prime basic \
        mpitests tsexp keygen pubkey hmac keygrip fips186-dsa aeswrap \
	curves t-kdf pkcs1v2 random dsa-rfc6979


# The last test to run.
TESTS += benchmark


# Need to include ../src in addition to top_srcdir because gcrypt.h is
# a built header.
AM_CPPFLAGS = -I../src -I$(top_srcdir)/src
AM_CFLAGS = $(GPG_ERROR_CFLAGS)

LDADD = ../src/libgcrypt.la $(DL_LIBS) ../compat/libcompat.la $(GPG_ERROR_LIBS)

EXTRA_PROGRAMS = testapi pkbench
noinst_PROGRAMS = $(TESTS) fipsdrv rsacvt

EXTRA_DIST = README rsa-16k.key cavs_tests.sh cavs_driver.pl \
	     pkcs1v2-oaep.h pkcs1v2-pss.h pkcs1v2-v15c.h pkcs1v2-v15s.h
