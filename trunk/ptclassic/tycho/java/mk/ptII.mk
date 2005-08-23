# Makefile for Ptolemy II standalone distribution
#
# Version identification:
# $Id$
#
# Copyright (c) 1996-1999 The Regents of the University of California.
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

# NOTE: Don't edit this file if it is called ptII.mk, instead
# edit ptII.mk.in, which is read by configure

# Every Ptolemy II makefile should include ptII.mk

# Variables with @ around them are subsituted in by the configure script

# Default top-level directory.  Usually this is the same as $PTII
prefix =	/users/ptolemy/tycho/java

# Usually the same as prefix.  exec_prefix is part of the autoconf standard.
exec_prefix =	${prefix}

# Source directory we are building from.
srcdir =	.

# Directory in which to install scripts
BIN_INSTALL_DIR =	$(exec_prefix)/bin

# The home of the Java Developer's Kit (JDK)
# Generating Java documentation uses this makefile variable
# The line below gets subsituted by the configure script
PTJAVA_DIR = 	/usr/java

# JDK Version from the java.version property
JVERSION =	1.1.6 

# Java CLASSPATH separator
# For Unix, this would be : 
# For Cygwin, this would be ;
CLASSPATHSEPARATOR = :

# The home of the Java Foundation Classes (JFC) aka Swing
JFCHOME = 	/opt/swing

# CLASSPATH necessary to find the swing.jar file for JFC
JFCCLASSPATH = $(JFCHOME)/swing.jar

# The variables below are for the SunTest JavaScope code coverage tool
# See http://www.suntest.com/JavaScope
# The 'jsinstr' command, which instruments Java code.
JSINSTR = 	jsinstr
JSINSTRFLAGS = 	-IFLUSHCLASS=true
# The 'jsrestore' command which uninstruments Java code.
JSRESTORE =	jsrestore
# The pathname to the JavaScope.zip file
JSCLASSPATH = 	/users/ptdesign/vendors/sun/JavaScope/JavaScope.zip

# The variables below are for JavaSound, which is used by files in
# ptolemy/media/javasound
# The directory that contains sound.jar
JAVASOUND_LIBDIR = /users/ptolemy/tycho/java/vendors/sun/javasound/lib

# JAVASOUND_NEEDED_DIRS is used in ptolemy/media/makefile
# to add the javasound directory to the list of directories
# to run submakes in if we found sound.jar
JAVASOUND_NEEDED_DIRS = 

########## You should not have to change anything below this line ######

# The 'javac' compiler.
JAVAC = 	$(PTJAVA_DIR)/bin/javac

# Flags to pass to javac.  Usually something like '-g -depend'
# This line gets substituted by configure
# Note that we don't include JDEPENDFLAG in the JFLAGS because it
# causes the build to take much longer
JDEPENDFLAG =	-depend   
JDEBUG =	-g
JOPTIMIZE =	#-O
JFLAGS = 	$(JDEBUG) $(JOPTIMIZE)

# The 'javadoc' program
JAVADOC = 	$(PTJAVA_DIR)/bin/javadoc

# Flags to pass to javadoc.
JDOCFLAGS = 	-author -version

# The jar command, used to produce jar files, which are similar to tar files
JAR =		$(PTJAVA_DIR)/bin/jar

# The 'java' interpreter.
JAVA =		$(PTJAVA_DIR)/bin/java

# JavaCC is the Java Compiler Compiler which is used by ptolemy.data.expr
# The default location is $(PTII)/vendors/sun/JavaCC
JAVACC_DIR =	/users/ptolemy/tycho/java/vendors/sun/JavaCC
# Under Unix:
# JJTREE =	$(JAVACC_DIR)/bin/jjtree
# JAVACC =	$(JAVACC_DIR)/bin/javacc
# Under Cygwin32 NT the following should be used and JavaCC.zip must be in 
# the CLASSPATH
# JJTREE = 	$(JAVA) COM.sun.labs.jjtree.Main
# JAVACC = 	$(JAVA) COM.sun.labs.javacc.Main

JJTREE =	jjtree
JAVACC =	javacc


# Jar file that contains Jacl
PTJACL_JAR =	/users/ptolemy/tycho/java/lib/ptjacl.jar
# Flags to pass java when we start Jacl.
PTJACL_JFLAG =  -native
# jtclsh script to run Jacl for the test suite.
# We could use bin/ptjacl here, but instead we start it from within
# make and avoid problems
JTCLSH =	CLASSPATH=$(CLASSPATH)$(AUXCLASSPATH)\$(CLASSPATHSEPARATOR)$(PTJACL_JAR) $(JAVA) $(PTJACL_JFLAG) tcl.lang.Shell

# Location of the diva.jar file.  Diva is (among other things) a graph
# visualization tool used by some of the demos.  For more information, see
# http://www-cad.eecs.berkeley.edu/diva/
DIVA_JAR = $(PTII)/lib/diva.jar

# Commands used to install scripts and data
# Use $(ROOT) instead of $(PTII) for install so that we don't
# need to have PTII set under Ptolemy classic when installing Ptplot
INSTALL =		$(ROOT)/config/install-sh -c
INSTALL_PROGRAM =	${INSTALL}
INSTALL_DATA =		${INSTALL} -m 644
