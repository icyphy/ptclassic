(technology schematic

  (comment "The following section is identical to that in Ptolemy 0.3.1 and")
  (comment "earlier versions.")

  (figureGroupDefault WIRING
      (userData priority 10)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault PLACE
      (userData priority 0)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault SYMBOL
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "Color layers, priority 5, from original ptolemy technology")

  (figureGroupDefault yellowSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault delay
      (userData priority 10)
      (width -2)
      (fillPattern 3 3 "010111010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault label
      (userData priority 9)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault redSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid
      (userData priority 5)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid
      (userData priority 5)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid
      (userData priority 5)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault cursor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault intColor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault floatColor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault anytypeColor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault complexColor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )
  (figureGroupDefault packetColor
      (userData priority 5)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "The following section is newer, and adds layers with various")
  (comment "priorities lower and higher than those in the original spec.")

  (comment "Special layers for icon making")

  (figureGroupDefault iconShadow
      (userData priority 0)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (figureGroupDefault iconBackground
      (userData priority 1)
      (width -2)
      (fillPattern 1 1 "0")
  )

  (comment "Color layers, priority 7")

  (figureGroupDefault yellowSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid7
      (userData priority 7)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "Color layers, priority 6")

  (figureGroupDefault yellowSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid6
      (userData priority 6)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "Color layers, priority 4")

  (figureGroupDefault yellowSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid4
      (userData priority 4)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "Color layers, priority 3")

  (figureGroupDefault yellowSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid3
      (userData priority 3)
      (width -2)
      (fillPattern 1 1 "1")
  )

  (comment "Color layers, priority 2")

  (figureGroupDefault yellowSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "000010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault yellowOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "101010101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blueOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "010101010")
      (borderPattern 1 "1")
  )
  (figureGroupDefault redOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "101000101")
      (borderPattern 1 "1")
  )
  (figureGroupDefault violetOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault greenSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "100010001")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault orangeSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 3 3 "001010100")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault brownSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 4 4 "1010111110101111")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault tanSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 4 4 "0101000001010000")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault whiteSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackOutline2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "0")
      (borderPattern 1 "1")
  )
  (figureGroupDefault blackSolid2
      (userData priority 2)
      (width -2)
      (fillPattern 1 1 "1")
  )

)
