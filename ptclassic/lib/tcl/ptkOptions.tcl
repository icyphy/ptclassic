# sets the default Options for widgets and windows used by Pigi
#
# Author: Alan Kamas, Kennard White, and Edward Lee
# Version: $Id$
#
# Copyright (c) 1990-1993 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
#                                                         COPYRIGHTENDKEY

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
option add Pigi.mediumfont *-times-medium-r-normal--*-180-*
option add CGC.mediumfont *-times-medium-r-normal--*-180*

# Font for major titles.  It is very large.
option add Pigi.bigfont  *-Helvetica-Bold-R-Normal-*-240-*
option add CGC.bigfont  *-Helvetica-Bold-R-Normal-*-240-*

#Define Special Colors
option add Pigi.positiveColor [ptkColor blue]
option add Pigi.negativeColor [ptkColor red]

#Colors used for plots with multiple traces
option add Pigi.plotColor1 [ptkColor red3]
option add Pigi.plotColor2 [ptkColor SkyBlue3]
option add Pigi.plotColor3 [ptkColor DarkOliveGreen3]
option add Pigi.plotColor4 [ptkColor ivory]
option add Pigi.plotColor5 [ptkColor gray40]
option add Pigi.plotColor6 [ptkColor gold]
option add Pigi.plotColor7 [ptkColor BlueViolet]
option add Pigi.plotColor8 [ptkColor CadetBlue]
option add Pigi.plotColor9 [ptkColor chocolate]
option add Pigi.plotColor10 [ptkColor DarkSlateGray]
option add Pigi.plotColor11 [ptkColor ForestGreen]
option add Pigi.plotColor12 [ptkColor LightYellow]

#Default parameters for certain widgets
option add Pigi.meterWidthInC 8.0
option add Pigi.scaleWidthInC 7.0

############################################################################
# The following section defines options (like X resources) associated with
# specific window names or window classes.  Normally, the user does not
# need to explicitly "get" these options.  Just by choosing a suitable window
# name or assigning a suitable classname to a Tk frame, the user will
# automatically get the the specified colors and fonts.
############################################################################

# Standard default colors for standard Tk widgets
option add Pigi*Text.Background [ptkColor antiqueWhite]
option add Pigi*Entry.Background [ptkColor wheat3]
option add Pigi*Canvas.background [ptkColor antiqueWhite3]
option add Pigi*Button.foreground [ptkColor firebrick]
option add Pigi*Scale.Foreground [ptkColor bisque1]
option add Pigi*Scale.background [ptkColor tan4]
option add Pigi*Scale.sliderForeground [ptkColor bisque]
option add Pigi*Scale.activeForeground [ptkColor bisque2]

# Default font for all message windows used in Pigi
option add Pigi*Message*Font [option get . mediumfont Pigi]

# The following class is used to particularly highlight a widget,
# for example to call attention to a button that must be pushed
# in order to be able to continue with the program.
option add Pigi*Attention*Button.foreground [ptkColor firebrick]
option add Pigi*Attention*Button.activeForeground [ptkColor black]
option add Pigi*Attention*Button.background [ptkColor burlywood1]
option add Pigi*Attention*Button.activeBackground [ptkColor burlywood3]
option add Pigi*Attention*Button.disabledForeground [ptkColor black]

# Set up defaults for the Windows generated by CGC programs
# #########################################################

option add *CGC*Message*Font [option get . mediumfont Pigi]

# set the specific interations entry window color
option add CGC.numberIters.entry.background [ptkColor wheat3]

# set general options for all CGC windows
option add *CGC*Button*Foreground [ptkColor blue3]
option add *CGC*Scale*Foreground [ptkColor bisque1]
option add *CGC*Scale*SliderForeground [ptkColor bisque1]
option add *CGC*Scale*Background [ptkColor tan4]
option add *CGC*Entry*Background [ptkColor burlywood1]

