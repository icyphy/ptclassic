# Tycho preferences file to use small fonts.
#
# $Id$

preference assign fonts textFont  8x13
preference assign fonts entryFont [.tychoFonts getFont Helvetica 12]

preference assign layout textWidth 75
preference assign layout textHeight 50

preference assign htmlview fontSize small
preference assign htmlview textWidth 75
preference assign htmlview textHeight 50

# Unfortunately, we can't do this yet with preferences!
option add *font [.tychoFonts getFont Helvetica 12]
