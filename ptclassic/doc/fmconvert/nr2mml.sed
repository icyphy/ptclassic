# Version id: $Id$
# Programmer: E. A. Lee,
# inspired by previous programs by David A. Wheeler and HorngDar Lin
#
# nr2mml.sed - convert a ptolemy troff documentation file into a mml file.
#
# The PTOLEMY environment variable must be set to point to the home
# directory of the Ptolemy installation.
#
# Usage:
#         Cat sect.* files to xxxx.mm.
#         Run nr2mml xxxx.mm.
#         Create a new maker file (e.g. using ~FRAME/TEMPLATES/printpaper)
#         Import xxxx.mml.
#
# Copyright (c) 1993 The Regents of the University of California.
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
#                                                        COPYRIGHTENDKEY
s/\\/\\\\/g
s/</\\</g
s/>/\\>/g
s/\\\\fI/<Italic>/g
s/\\\\fP/<Plain>/g
s/\\\\fR/<Plain>/g
s/\\\\fB/<Bold>/g
s/\\\\f6/<Bold>/g
s/\\\\f5/<Plain>/g
s/\\\\f\\\\\*F/<Class>/g
#s/	/ /g
s/	/\\t/g
s/\\\\\&//g
s//, /g
s/^\.sh/.H /
# Remove quotation marks
/^\.H/s/"//g
/^\.UH /s/"//g
/^\.TI /s/"//g
/^\.IE /s/"//g
/^\.Ie /s/"//g
/^\.Ir /s/"//g
/^\.Id /s/"//g
/^\.Se /s/"//g
/^\.Sr /s/"//g
/^\.Sd /s/"//g
/^\.ip /s/"//g
# Ptolemy information
/\\\\\*(PT/s//Ptolemy/g
/\\\\\*(PV/s//0.5/g
# Simple paragraph conversions
/^\.ls/s/ //g
s/^\.P/.pp/
s/^\.PP/.pp/
s/^\.AU/<Author> /
s/^\.AE/<Body> /
s/^\.BU/<Bullet> /
s/^\.LE/<Bullet> /
s/^\.IP/<Definition>/
s/^\.lp/<LBody>/
s/^\.pp/<Body>/
s/^\.LP/<LBody>/
s/^\.(f/<Footnote>/
s/^\.)f/<LBody>/
# Character conversions
s/``/<Character \\xd2 >/g
s/''/<Character \\xd3 >/g
s/\\\\(dg/<Character \\xa0 >/g
s/\\\\(dd/<Character \\xa4 >/g
s/\\\\(mu/x/g
