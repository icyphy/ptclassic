# Tcl script used by the DEJNITest star
# $Id$
# Authors: Jens Voigt, Christopher Hylands
# Copyright (c) 1997 Dresden University of Technology,
# Mobile Communications Systems
#
# Copyright (c) 1997 The Regents of the University of California.
# All rights reserved.
# See the file $PTOLEMY/copyright for copyright notice,
# limitation of liability, and disclaimer of warranty provisions.

#######################################################################
#### createjava_$starID
# Create a Java virtual machine
#
proc createjava_$starID {starID} {
    global env

    # Look for a CLASSPATH and add to it
    if [info exists env(CLASSPATH)] {
	# FIXME: this won't work under windows.
	set classpath $env(CLASSPATH):$env(TYCHO)/src/tyjni
    } else {
	if [info exists env(JAVAHOME)] {
	    set classpath $env(JAVAHOME)/lib/classes.zip:$env(TYCHO)/src/tyjni
	} else
	    set classpath $env(TYCHO)/src/tyjni
	}
    }

    # jnicreatejvm is defined in $PTOLEMY/tycho/src/tyjni/tyjni.c
    jnicreatejvm $classpath
    puts " I have a JavaVM now"
}

#######################################################################
#### loadlib_$starID
# Load the libtyjni shared library
#
proc loadlib_$starID {starID} {
    puts "loading libtyjni[info sharedlibextension]"
    ::tycho::loadIfNotPresent jnifac tyjni
    puts "library has been loaded"
}
    
#######################################################################
#### new_$starID
#
#
proc new_$starID {starID} {
    set inputvals [grabInputsNumber_$starID]
    setOutputFactorial_$starID [jnifac [lindex $inputvals 0]] 
}
    
#######################################################################
#### createjava_$starID
# Destroy the Java virtual machine
# Note that calling these seems to crash Ptolemy sometimes!
# The Java JNI docs say:
#
#    You cannot unload the Java Virtual Machine without terminating
#    the process. The DestroyJavaVM call simply returns an error code. 
#
proc destroyjava_$starID {starID} {
    # jnidestroyjvm is defined in $PTOLEMY/tycho/src/tyjni/tyjni.c
    jnidestroyjvm
    puts " I have destroyed the JavaVM"
}
