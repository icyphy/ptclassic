# test.tcl
#
# Simple test file for Tycho packages
#
# $Id$
#

# Load the package
::tycho::loadIfNotPresent ::tycho::testtask tytest

# Initialize a Tk counter
set counter 0

# Start, execute, and wrapup the test task
::tycho::testtask setup counter
::tycho::testtask execute
set count1 $counter
::tycho::testtask execute
::tycho::testtask wrapup

# Print result
puts "Test task counted $count1 and $counter times"
