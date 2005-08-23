README.txt
@(#)README.txt	1.2 03/16/98

This is the README.txt file for the Tycho
package tycho.util.tytest.

tycho.util.tytest contains the script definitions
for the test scripts in every package, and a shell
script for running individual test scripts.

Every test script in a packages test directory should
starts with

  package require tycho.util.tytest
  package require <mypackage>

This will ensure that the script can be run by the
automated tools. To run a single test script,
change to the test directory and type

  tytest <scriptname>


This package contains the following directories:

.     : (this directory) Tcl and Itcl source files
bin   : start-up scripts
demos : demonstration scripts
doc   : HTML documentation
test  : test files

For the latest version of this package, see the Tycho
home page: http://ptolemy.eecs.berkeley.edu/tycho.


