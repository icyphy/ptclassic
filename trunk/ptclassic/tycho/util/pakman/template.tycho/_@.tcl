# Package load file for the %Package% package
#
# @Author: %author%
#
# @Version: $Id$
#
%copyright%

# Based on pp 344-346 of Harrison and McClellan's "Effective Tcl/Tk
# Programming" book

package provide %Package% %version%
global env auto_path
set env(%PACKAGE%_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(%PACKAGE%_LIBRARY)] == -1 } {
    lappend auto_path $env(%PACKAGE%_LIBRARY)
}
