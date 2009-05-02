#!/bin/sh

aclocal -I autoconf && \
	autoheader && \
	autoconf -I autoconf

