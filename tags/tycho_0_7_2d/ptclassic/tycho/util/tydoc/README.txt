README.txt
@(#)README.txt	1.2 06/11/98

This is the README.txt file for the Tycho
package tycho.util.tydoc.

tydoc is a standalone utility that will convert Itcl files into
html files.  Tydoc is similar to javadoc in that it uses doctags
that begin with @.

To run tydoc, do the following:
1) Set the TYDOC_LIBRARY environment variable to the directory where
this file resides
   setenv TYDOC_LIBRARY `pwd`
2) Run:
   bin/tydoc TyDocMain.itcl
3) The output will appear in doc/codeDoc

Usage: tydoc [-v] [-t "title"] itcl_file [itcl_file . . .] 
-v: verbose html output
-t "title": use "title" as the title for the index page


For more information, see doc/index.html

This package contains the following directories:

.     : (this directory)
      TyDocMain.itcl  - The Itcl source that does the real work
      tydoc.tcl -       Tcl script used by bin/tydoc
bin   : start-up scripts
      tydoc -           Shell script that starts Tydoc
doc   : HTML documentation
test  : test files

For the latest version of this package, see the Tycho
home page: http://ptolemy.eecs.berkeley.edu/tycho.


