# Tycho preferences file to set color to "bisque."
#
# $Id$

::tycho::preference assign edittext textBackground [ptkColor antiqueWhite white]
::tycho::preference assign edittext selectColor [ptkColor yellow white]
::tycho::preference assign dialogs entryBackground [ptkColor antiqueWhite white]

::tycho::preference assign viewhtml textBackground [ptkColor linen white]
::tycho::preference assign graphics canvasColor [ptkColor antiqueWhite3 white]

::tycho::preference configure tkpalette -inherits tkbisquepalettePreferences
updateTkPalette
