# Tycho preferences file to use large fonts.
#
# $Id$

preference assign fonts textFont  9x15
preference assign fonts entryFont 9x15

preference assign layout textWidth 72
preference assign layout textHeight 40

preference assign htmlview fontSize medium
preference assign htmlview textWidth 72
preference assign htmlview textHeight 40

# Unfortunately, we can't do this yet with preferences!
option add *font [.tychoFonts getFont Helvetica 12 bold]
