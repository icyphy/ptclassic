# sets the default Options for widgets and windows used by Pigi
#
# Author: Alan Kamas, Kennard White, and Edward Lee
# Contributors: Brian Evans
# Version: $Id$
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY

############################################################################
# The following section defines options (X resources) that get used indirectly
# to set options associated with window classes or specific window
# names.  The idea is to have all particular uses of a color consolidated
# in simple resources that are available to all windows within Pigi.
# Thus, if you want to use the standard color that indicates a positive
# number for something, you would refer to it as:
#
#	[option get . positiveColor Pigi]
#	
# All of these options are in class Pigi.  The main window created by Pigi
# (called "."), and all derivative windows are of class Pigi as well.
#
# Below this is another section that associates these colors with specific
# windows and classes of windows.
############################################################################

# All font values should be set here.  This will allow users with
# limited fonts to adjust them here, in one place rather than throughout
# the code.

# For small fonts, we just accept the Tk defaults.

# Font for titles.  It is larger than the usual default Tk font.
option add Pigi.mediumfont *-times-medium-r-normal--*-180-* startupFile
option add CGC.mediumfont *-times-medium-r-normal--*-180* startupFile

# Font for major titles.  It is very large.
option add Pigi.bigfont  *-Helvetica-Bold-R-Normal-*-240-* startupFile
option add CGC.bigfont  *-Helvetica-Bold-R-Normal-*-240-* startupFile

#Define Special Colors
option add Pigi.positiveColor [ptkColor blue] startupFile
option add Pigi.negativeColor [ptkColor red] startupFile
option add Pigi.highlightColor [ptkColor firebrick] startupFile

#Colors used for plots with multiple traces
option add *plotColor1 [ptkColor red3] startupFile
option add *plotColor2 [ptkColor SkyBlue3] startupFile
option add *plotColor3 [ptkColor DarkOliveGreen3] startupFile
option add *plotColor4 [ptkColor ivory] startupFile
option add *plotColor5 [ptkColor gray40] startupFile
option add *plotColor6 [ptkColor gold] startupFile
option add *plotColor7 [ptkColor BlueViolet] startupFile
option add *plotColor8 [ptkColor CadetBlue] startupFile
option add *plotColor9 [ptkColor chocolate] startupFile
option add *plotColor10 [ptkColor DarkSlateGray] startupFile
option add *plotColor11 [ptkColor ForestGreen] startupFile
option add *plotColor12 [ptkColor LightYellow] startupFile

#Default parameters for certain widgets
option add Pigi.meterWidthInC 8.0 startupFile
option add Pigi.scaleWidthInC 7.0 startupFile

############################################################################
# The following section defines options (like X resources) associated with
# specific window names or window classes.  Normally, the user does not
# need to explicitly "get" these options.  Just by choosing a suitable window
# name or assigning a suitable classname to a Tk frame, the user will
# automatically get the the specified colors and fonts.
############################################################################

# Standard default colors for standard Tk widgets
option add Pigi*Text.Background [ptkColor antiqueWhite] startupFile
option add Pigi*Entry.Background [ptkColor wheat3] startupFile
option add Pigi*selectForeground [ptkColor midnightblue] startupFile
option add Pigi*Canvas.background [ptkColor antiqueWhite3] startupFile
option add Pigi*Button.foreground [ptkColor firebrick] startupFile
option add Pigi*Scale.Foreground [ptkColor bisque1] startupFile

# COMPATIBILITY
if { $tk_version >= 4.0 } {
# We follow the Motif guidelines, as does Tk4.0 (See default.h in the Tk dist)
option add Pigi*troughColor [ptkColor grey77] startupFile
option add Pigi*Scale.background [ptkColor grey85] startupFile
option add Pigi*Scale.activeBackground [ptkColor grey92] startupFile
} else {
option add Pigi*Scale.background [ptkColor tan4] startupFile
option add Pigi*Scale.sliderForeground [ptkColor bisque] startupFile
option add Pigi*Scale.activeForeground [ptkColor bisque2] startupFile
}

# Default font for all message windows used in Pigi
option add Pigi*Message*Font [option get . mediumfont Pigi] startupFile

# The following class is used to particularly highlight a widget,
# for example to call attention to a button that must be pushed
# in order to be able to continue with the program.
option add Pigi*Attention*Button.foreground [ptkColor firebrick] startupFile
option add Pigi*Attention*Button.activeForeground [ptkColor black] startupFile
if { $tk_version >= 4.0 } {
option add Pigi*Attention*Button.background [ptkColor grey37] startupFile
option add Pigi*Attention*Button.activeBackground [ptkColor grey44] startupFile
option add Pigi*Attention*Button.disabledForeground [ptkColor black] startupFile
} else {
option add Pigi*Attention*Button.background [ptkColor burlywood] startupFile
option add Pigi*Attention*Button.activeBackground [ptkColor burlywood3] startupFile
option add Pigi*Attention*Button.disabledForeground [ptkColor black] startupFile
}


# Set up defaults for the Windows generated by CGC programs
# #########################################################

option add *CGC*Message*Font [option get . mediumfont Pigi] startupFile

option add *CGC*Background [ptkColor antiqueWhite2] startupFile
option add *CGC*Text.Background [ptkColor antiqueWhite] startupFile
option add *CGC*Entry.Background [ptkColor ivory] startupFile
option add *CGC*selectForeground [ptkColor midnightblue] startupFile
option add *CGC*Canvas.background [ptkColor antiqueWhite3] startupFile
option add *CGC*Button.foreground [ptkColor blue4] startupFile
option add *CGC*Button.activeBackground [ptkColor antiqueWhite3] startupFile
option add *CGC*Button.activeForeground [ptkColor blue4] startupFile
option add *CGC*Scale.Foreground [ptkColor antiqueWhite2] startupFile
if { $tk_version >= 4.0 } {
option add *CGC*Scale.background [ptkColor cadetBlue] startupFile
option add *CGC*Scale.sliderForeground [ptkColor antiqueWhite2] startupFile
option add *CGC*Scale.activeForeground [ptkColor antiqueWhite3] startupFile
} else {
option add *CGC*Scale.background [ptkColor cadetBlue] startupFile
option add *CGC*Scale.sliderForeground [ptkColor antiqueWhite2] startupFile
option add *CGC*Scale.activeForeground [ptkColor antiqueWhite3] startupFile
}
