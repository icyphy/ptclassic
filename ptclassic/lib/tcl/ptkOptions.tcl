# sets the default Options for widgets and windows used by Pigi
#
# Author: Alan Kamas and Kennard White
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

# All font values should be set here.  This will allow users with
# limited fonts to adjust them here, in one place rather than throughout
# the code.

# Set up defaults for the PigiWindows:
# ###################################

# Define font types used in PigiWindows:
option add Pigi.mainfont -Adobe-times-medium-r-normal--*-180*
option add Pigi.bigfont  -Adobe-Helvetica-Bold-R-Normal-*-240-*

#Define Special Colors
option add Pigi.positiveColor [ptkColor blue]
option add Pigi.negativeColor [ptkColor red]

#Define Special Button Color for Buttons that must be pressed
option add Pigi.pressMeBg [ptkColor orange1]
option add Pigi.pressMeActiveBg [ptkColor tan3]

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

# set font for all message windows used in Pigi
option add *Pigi*Message*Font [option get . mainfont Mainfont]

# Define Control Window specific options
#	Messages
option add *PigiControl*Message*Font [option get . mainfont Mainfont]
#	Entry
option add *PigiControl*Entry*Background [ptkColor burlywood1]
#	Canvas
option add *PigiControl*Canvas*Background [ptkColor AntiqueWhite3]
#	Buttons
option add *PigiControl*Button*foreground [ptkColor tan4]
#	Sliders
option add *PigiControl*Scale*Foreground [ptkColor bisque1]
option add *PigiControl*Scale*SliderForeground [ptkColor bisque1]
option add *PigiControl*Scale*background [ptkColor tan4]
#	Interations Window
option add *PigiControl.iter.entry.background [ptkColor wheat3]


# Set up defaults for the Windows generated by CGC programs
# #########################################################
option add CGC.mainfont -Adobe-times-medium-r-normal--*-180*
option add CGC.bigfont  -Adobe-Helvetica-Bold-R-Normal-*-240-*
option add *CGC*Message*Font [option get . mainfont Mainfont]
# set the specific interations entry window color
option add CGC.numberIters.entry.background [ptkColor wheat3]
# set general options for all CGC windows
option add *CGC*Button*Foreground [ptkColor blue3]
option add *CGC*Scale*Foreground [ptkColor bisque1]
option add *CGC*Scale*SliderForeground [ptkColor bisque1]
option add *CGC*Scale*Background [ptkColor tan4]
option add *CGC*Entry*Background [ptkColor burlywood1]

