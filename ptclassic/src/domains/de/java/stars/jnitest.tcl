#
# Tcl script used by the DEJNITest star
# $Id$
# Authors: Jens Voigt, Christopher Hylands

#######################################################################
#### createjava_$starID
# Create a Java virtual machine
#
proc createjava_$starID {starID} {
    cjvm
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
    djvm
    puts " I have the JavaVM destroyed"
}
