# Tycho preferences file to set color to "bisque."
#
# $Id$

preference assign edittext textBackground [ptkColor antiqueWhite white]
preference assign edittext selectColor [ptkColor yellow white]
preference assign dialogs entryBackground [ptkColor antiqueWhite white]

preference assign viewhtml textBackground [ptkColor linen white]
preference assign graphics canvasColor [ptkColor antiqueWhite3 white]

preference configure tkpalette -inherits tkbisquepalettePreferences
updateTkPalette
