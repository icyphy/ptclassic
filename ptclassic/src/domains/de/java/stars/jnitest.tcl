#
# Tcl script used by the DEJNITest star
# $Id$
# Author: Jens Voigt
proc createjava_$starID {starID} {
    cjvm
    puts " I have a JavaVM now"
}

proc loadlib_$starID {starID} {
    puts "loading libtyjni[info sharedlibextension]"
    load /users/voigtje/java/test/libtyjni.so
    puts "library has been loaded"
}
    
proc new_$starID {starID} {
    
    set inputvals [grabInputsNumber_$starID]
    setOutputFactorial_$starID [jnifac [lindex $inputvals 0]] 
}
    
proc destroyjava_$starID {starID} {
    djvm
    puts " I have the JavaVM destroyed"
}
