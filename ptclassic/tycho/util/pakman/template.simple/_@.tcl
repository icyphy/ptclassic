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
global env
set env(%PKG%_LIBRARY) [file dirname [info script]]
lappend auto_path $env(%PKG%_LIBRARY)
