# Package load file for the %Package% package
#
# @Author: %author%
#
# @Version: %percentW%percent %percentG%percent
#
%copyright%

# Based on pp 344-346 of Harrison and McClellan's "Effective Tcl/Tk
# Programming" book

package provide %Package% %version%
global env auto_path
set env(%PKG%_LIBRARY) [file dirname [info script]]
if { [lsearch -exact $auto_path $env(%PKG%_LIBRARY)] == -1 } {
    lappend auto_path $env(%PKG%_LIBRARY)
}
