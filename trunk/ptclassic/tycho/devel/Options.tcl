# sets the default Options for widgets and windows used by Tycho
#
# Authors: Edward Lee
# Version: $Id$
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
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
# The following section defines options (X resources) that get used
# indirectly to set options associated with window classes or specific
# window names. The idea is to have all particular uses of a color
# consolidated in simple resources that are available to all windows
# within Tycho. Thus, if you want to use the standard color that
# indicates a positive number for something, you would refer to it as:
#
#	[option get . positiveColor $topLevelClass]
#	
# All of these options are in class $topLevelClass. The main window
# created by sg (called "."), and all derivative windows are of class
# $topLevelClass as well.  The global variable topLevelClass is set in
# Font.tcl, which must be sourced before this file is sourced.
#
# Below this is another section that associates these colors with
# specific windows and classes of windows.
############################################################################

# font values are controlled in the file Font.tcl, not here.

# FIXME:  All of the following mechanisms for handling options are obsolete.
# The name of the application class.
set topLevelClass [winfo class .]

# Define Special Colors
option add $topLevelClass.positiveColor [ptkColor blue black] startupFile
option add $topLevelClass.negativeColor [ptkColor red black] startupFile
option add $topLevelClass.highlightColor [ptkColor firebrick black] startupFile

#Colors used for plots with multiple traces
option add *plotColor1 [ptkColor red3 black] startupFile
option add *plotColor2 [ptkColor SkyBlue3 black] startupFile
option add *plotColor3 [ptkColor DarkOliveGreen3 black] startupFile
option add *plotColor4 [ptkColor ivory black] startupFile
option add *plotColor5 [ptkColor gray40 black] startupFile
option add *plotColor6 [ptkColor gold black] startupFile
option add *plotColor7 [ptkColor BlueViolet black] startupFile
option add *plotColor8 [ptkColor CadetBlue black] startupFile
option add *plotColor9 [ptkColor chocolate black] startupFile
option add *plotColor10 [ptkColor DarkSlateGray black] startupFile
option add *plotColor11 [ptkColor ForestGreen black] startupFile
option add *plotColor12 [ptkColor LightYellow black] startupFile

#Default parameters for certain widgets
option add $topLevelClass.meterWidthInC 8.0 startupFile
option add $topLevelClass.scaleWidthInC 7.0 startupFile

############################################################################
# The following section defines options (like X resources) associated with
# specific window names or window classes.  Normally, the user does not
# need to explicitly "get" these options.  Just by choosing a suitable window
# name or assigning a suitable classname to a Tk frame, the user will
# automatically get the specified colors.
############################################################################

# Standard default colors for standard Tk widgets
# FIXME: These appear to have no affect.
option add *Listbox.HighlightColor \
	[ptkColor firebrick black] startupFile
option add *selectForeground \
	[ptkColor midnightblue black] startupFile
option add $topLevelClass*Canvas.background \
	[ptkColor antiqueWhite3 white] startupFile
option add $topLevelClass*Button.foreground \
	[ptkColor firebrick black] startupFile
option add $topLevelClass*Scale.Foreground \
	[ptkColor bisque1 white] startupFile
option add $topLevelClass*Scale.background \
	[ptkColor tan4 black] startupFile
option add $topLevelClass*Scale.sliderForeground \
	[ptkColor bisque white] startupFile
option add $topLevelClass*Scale.activeForeground \
	[ptkColor bisque2 black] startupFile

# The following class is used to particularly highlight a widget,
# for example to call attention to a button that must be pushed
# in order to be able to continue with the program.
option add $topLevelClass*Attention*Button.foreground \
	[ptkColor firebrick black] startupFile
option add $topLevelClass*Attention*Button.activeForeground \
	[ptkColor black black] startupFile
option add $topLevelClass*Attention*Button.background \
	[ptkColor burlywood1 white] startupFile
option add $topLevelClass*Attention*Button.activeBackground \
	[ptkColor burlywood3 white] startupFile
option add $topLevelClass*Attention*Button.disabledForeground \
	[ptkColor black black] startupFile

# Set up defaults for the Windows generated by CGC programs
# #########################################################

option add *CGC*Background [ptkColor antiqueWhite2 white] startupFile
option add *CGC*Text.Background [ptkColor antiqueWhite white] startupFile
option add *CGC*Entry.Background [ptkColor ivory white] startupFile
option add *CGC*selectForeground [ptkColor midnightblue black] startupFile
option add *CGC*Canvas.background [ptkColor antiqueWhite3 white] startupFile
option add *CGC*Button.foreground [ptkColor blue4 black] startupFile
option add *CGC*Button.activeBackground [ptkColor antiqueWhite3 white] startupFile
option add *CGC*Button.activeForeground [ptkColor blue4 black] startupFile
option add *CGC*Scale.Foreground [ptkColor antiqueWhite2 white] startupFile
option add *CGC*Scale.background [ptkColor cadetBlue black] startupFile
option add *CGC*Scale.sliderForeground [ptkColor antiqueWhite2 white] startupFile
option add *CGC*Scale.activeForeground [ptkColor antiqueWhite3 white] startupFile
