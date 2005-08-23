# Makefile for Ptolemy lib dir (used primarily for 'make sources')

# Version Identification:
# @(#)makefile	1.16 02/01/99
# Copyright (c) 1990-1999 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY

ROOT = ..
VPATH = $(ROOT)/lib

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

SCRIPT_SRCS = pigiDebugRpc pigiRpcConsole \
	pigiRpcDebug pigiRpcDebug.dbx pigiRpcDebug.ddd pigiRpcDebug.xdb \
	pigiRpcShell

EXTRA_SRCS = $(SCRIPT_SRCS) \
	CompileMake.template README \
	oct2ps.pro \
	pigiXRes7 pigiXRes8 pigiXRes8.bw pigiXRes8.cp \
	pigiXRes9 pigiXRes9.bw pigiXRes9.cp startPigiRpc \
	vem.bindings.7-3 vem.bindings.8-0 vem.bindings.8-1

# Don't include xpole-1.4.6 here, as we don't ship it.

MISC_FILES = 	bw_patterns \
		cg56 cgc colors defaultContent \
		mathematica matlab rename tcl tech technology vem
LIBR = dummy

# Do make sources so that we ensure that the subdirectories are checked out
all: sources $(EXTRA_SRCS) $(MISC_FILES)
	-chmod a+x $(SCRIPT_SRCS)

install: all

tech: colors
	ln -s $^ $@

technology: colors
	ln -s $^ $@

# We have an all rule, so include tycommon.mk.
include $(ROOT)/mk/tycommon.mk
