 Makefile for tycho standalone distribution
#
# Version identification:
# $Id$
#
# Copyright (c) 1996-%Q% The Regents of the University of California.
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
# Date of creation: 7/31/96
# Author: Christopher Hylands

# This makefile should be included at the top of every Tycho makefile.
# This makefile uses the ROOT makefile variable.


# The home of the Java Developer's Kit (JDK)
# Generating Java documentation uses this makefile variable
# JAVAHOME = 	/opt/SUNWjws/JDK
JAVAHOME = 	/opt/jdk1.2beta2

# Location of the Java html documentation.
# The file java.lang.Object.html should
# be at $JAVAHTMLDIR/java.lang.Object.html
# JAVAHTMLDIR = 	/opt/SUNWjws/JWS/lib/html/java/api
JAVAHTMLDIR = 	$(JAVAHOME)/docs/api

# The variables below are for the SunTest JavaScope code coverage tool
# See http://www.suntest.com/JavaScope
# The 'jsinstr' command, which instruments Java code.
JSINSTR = 	jsinstr
JSINTRFLAGS = 	-IFLUSHCLASS=true
# The 'jsrestore' command which uninstruments Java code.
JSRESTORE =	jsrestore
# The pathname to the JavaScope.zip file
JSCLASSPATH = 	/users/ptdesign/vendors/sun/JavaScope/JavaScope.zip

########## You should not have to change anything below this line ######


# The 'javac' compiler.
JAVAC = 	$(JAVAHOME)/bin/javac

# Flags to pass to javac.
JFLAGS = 	-g

# The 'javadoc' program
JAVADOC = 	$(JAVAHOME)/bin/javadoc

# Flags to pass to javadoc.
JDOCFLAGS = 	-author -version # -doctype MIF

# The jar command, used to produce jar files, which are similar to tar files
JAR =		jar

# Script to run tclBlend
JTCLSH =	jtclsh

# The destination directory for any libraries created.
# Usually this is an architecture dependent library.
LIBDIR =	$(ROOT)/lib.$(PTARCH)

# Provide an initial value for LIB_DEBUG so we don't get messages about
# multiply defined rules for $(LIBDIR)/$(LIB_DEBUG) if LIB_DEBUG is empty.
LIBR_DEBUG =	libdummy_g

# Itcl2.1 shell: [incr Tcl] sh  (tclsh + namespaces)
#ITCLSH =	$(PTOLEMY)/tcltk/itcl.$(PTARCH)/bin/itclsh
ITCLSH =	itclsh
