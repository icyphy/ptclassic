# Tycho preferences file to set color to "bisque."
#
# $Id$

preference assign textedit textBackground [ptkColor antiqueWhite white]
preference assign textedit selectColor [ptkColor yellow white]
preference assign dialogs entryBackground [ptkColor antiqueWhite white]

preference assign htmlview textBackground [ptkColor linen white]

preference configure tkpalette -inherits tkbisquepalettePreferences
updateTkPalette
