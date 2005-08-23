# volume titles for Solaris
# written by Will C Lauer (wcl@cadre.com)
#
# With its default settings TkMan collects all volumes starting
# with the same character under a single section (for instance,
# 1, 1b, 1c, 1f, 1m and 1s are all collected under section 1).
# If multiple volumes share the same initial character
# in their man(manList) abbreviation, these
# sections are separated out in the Volumes menu.  The settings
# below separate out all volumes for Solaris.

# New users (those without ~/.tkman startup files) pick up these
# settings automatically.  The settings can also be interpolated
# into an existing startup file manually.

 
set man(manList) {1 1b 1c 1f 1g 1m 1s 2 3 3b 3c 3e 3f 3g 3i 3k 3l 3m 3n 3r 3s 
                  3t 3w 3x 4 4b 5 6 7 8 8c 8s 9 9e 9f 9s l o n p}
set man(manTitleList) {
        "User Commands" "BSD Compat Commands" Communications "FLMI Commands" 
        "Graphics/CAD Commands" "Maintenance Commands" "SunOS Specific Commands"
		"System Calls" Subroutines "BSD Compatibility" "Standard Library"
        "ELF Subroutines" "Fortran Subroutines" "Regexp Subroutines" 
        "Intl Subroutines" "Kvm Subroutines" "Lightweight Processes Subroutines"
        "Math Subroutines" "Networking Subroutines" 
        "Realtime Subroutines" "Standard I/O Subroutines" 
        "Threads Subroutines" "C Subroutines" "Misc Subroutines" 
        "File Formats" "BSD File Formats" "Headers, Tables, and Macros"
        "Games and Demos" "Devices/Special Files" "Maintenance Procedures"
        "Maintenance Procedures" "Maintenance Procedures"
        "DDI/DKI (Device Drv Interfaces)" 
        "DDI/DKI Entry Points" "DDI/DKI Kernel Functions"
        "DDI/DKI Data Structures"
        Local Old New Public
}
