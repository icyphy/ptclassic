#!/usr/local/bin/wish
#
# Version: $Id$
# Written by Michael C. Williamson
#
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
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
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
# 
# This is a Tcl/Tk helper app for the VHDL domain's TkSchedTarget.

# Procedure to mark a node when it is selected.
proc NodeMark { w x y } {
    global dot

    set dot($w,active) 1

    set nodeID [$w find closest [$w canvasx $x] [$w canvasy $y]]
    set dot($w,nodeID) $nodeID
    set node $dot($nodeID,node)
    set dot($w,node) $node
    set dot($w,textID) $dot($node,textID)
    set dot($w,x) $x
    set dot($w,y) $y

    # Highlight the node
    $w itemconfig $dot($w,nodeID) -fill red

    # Set the label text
    set theText [$w itemcget $dot($w,textID) -text]
#    puts "theText:  $theText"
    set dot($w,nodeName) $theText
}

# Procedure to drop a node when the button is released.
proc NodeDrop { w x y } {
    global dot

    set cx [$w canvasx $x]
    set cx [expr int($cx)]

    set nodeID $dot($w,nodeID)
    set node $dot($nodeID,node)
    set box [$w coords $nodeID]
    set inX [lindex $box 0]
    set inY [lindex $box 1]
    set outX [lindex $box 2]
    set outY [lindex $box 3]

    set leftX $inX
    set rightX $outX
    set upY $inY
    set downY $outY

    # Calculate where I am and call setProc accordingly.
#    puts "X-position:  $cx"
    set procnum [expr ($cx - (($cx - 62) % 100)) / 100]
    # Check that the procnum is within bounds
    set nprocs $dot($w,nprocs)
    if { $procnum < 0 } {
	set procnum 0
    }
    if { $procnum > [expr $nprocs - 1] } {
	set procnum [expr $nprocs - 1]
    }

#    puts "procnum:  $procnum"
    set theText [$w itemcget $dot($w,textID) -text]
#    puts "textID:  $theText"

    setProc $theText $procnum

    set newx [expr ($procnum * 100) + 100 + ($x - $leftX)]

    # Figure out the bounds of this node based on its input
    # and output arcs.  Don't allow it to move outside its
    # precedence constraints.
    set topBound [expr $dot($w,canvasTop) + 50]
    set bottomBound [expr $dot($w,canvasBottom) - 50]
    foreach conn $dot($node,inConns) {
	set connID $dot($conn,connID)
	set coords [$w coords $connID]
	set fromY [lindex $coords 1]
	# Don't let this node get above its input arc's fromY
	# Remember the one with highest numerical value
	if { $fromY > $topBound } {
	    set topBound $fromY
	}
    }
    foreach conn $dot($node,outConns) {
	set connID $dot($conn,connID)
	set coords [$w coords $connID]
	set toY [lindex $coords 3]
	# Don't let this node get below its output arc's toY
	# Remember the one with lowest numerical value
	if { $toY < $bottomBound } {
	    set bottomBound $toY
	}
    }

    if { $upY < $topBound } {
	set y [expr $y + ($topBound - $upY)]
    }
    if { $downY > $bottomBound } {
	set y [expr $y - ($downY - $bottomBound)]
    }
    set newy $y

#    puts "y:            $y"
#    puts "newy:         $newy"
#    puts "topBound:     $topBound"
#    puts "bottomBound:  $bottomBound"

    # The x arg to NodeDrag should be relative to where the
    # cursor is, not where the block is.
    NodeDrag $w $newx $newy

    # Unhighlight the node
    $w itemconfig $dot($w,nodeID) -fill green

    set dot($w,active) 0
    set dot($w,node) {}
    set dot($w,nodeID) {}
    set dot($w,textID) {}
}

# Procedure to drag a node to a new location.
proc NodeDrag { w x y } {
    global dot

    if { $dot($w,active) == 0 } { return }

    set dx [expr $x - $dot($w,x)]
    set dy [expr $y - $dot($w,y)]

    $w move $dot($w,nodeID) $dx $dy
    $w move $dot($w,textID) $dx $dy
    MoveArcs $w $dot($w,node) $x $y

    set dot($w,x) $x
    set dot($w,y) $y
}

# Procedure to return the input and output arc attachment coordinates.
proc NodeInOutCoords { w nodeID } {
    global dot

    # Depending on the node type { node or token }
    # determine the in and out x and y coordinates.
    set node $dot($nodeID,node)
#    set nodeID $dot($node,nodeID)
    set nodeType $dot($node,type)

    if { $nodeType == "node" } {
	set box [$w coords $nodeID]
	set upLeftX [lindex $box 0]
	set upLeftY [lindex $box 1]
	set lowRightX [lindex $box 2]
	set lowRightY [lindex $box 3]
	set inX [expr int(($upLeftX + $lowRightX) / 2)]
	set inY $upLeftY
	set outX [expr int(($upLeftX + $lowRightX) / 2)]
	set outY $lowRightY
#	puts "node coords:  $inX $inY $outX $outY"
	return "$inX $inY $outX $outY"
    }
    if { $nodeType == "token" } {
	set box [$w coords $nodeID]
	set leftX [lindex $box 0]
	set leftY [lindex $box 1]
	set topX [lindex $box 2]
	set topY [lindex $box 3]
	set rightX [lindex $box 4]
	set rightY [lindex $box 5]
	set bottomX [lindex $box 6]
	set bottomY [lindex $box 7]
	set inX $topX
	set inY $topY
	set outX $bottomX
	set outY $bottomY
#	puts "token coords:  $inX $inY $outX $outY"
	return "$inX $inY $outX $outY"
    }
}

# Procedure to move the arcs attached to a given node.
proc MoveArcs { w node x y } {
    global dot

    set nodeID $dot($node,nodeID)
    set inOutCoords [NodeInOutCoords $w $nodeID]
    set inX [lindex $inOutCoords 0]
    set inY [lindex $inOutCoords 1]
    set outX [lindex $inOutCoords 2]
    set outY [lindex $inOutCoords 3]

    # Find all the arcs connected to node
    # Move their endpoints connected to node
    # Use the right coordinates of node depending
    # on whether it's an arc into or out of node
    foreach conn $dot($node,inConns) {
	# These are in-arcs, so they connect to
	# the top of the node
	set connID $dot($conn,connID)
	set coords [$w coords $connID]
	set fromX [lindex $coords 0]
	set fromY [lindex $coords 1]
	set toX [lindex $coords 2]
	set toY [lindex $coords 3]
	# Redraw the line with new destination coords inX and inY
	$w coords $connID $fromX $fromY $inX $inY
    }
    foreach conn $dot($node,outConns) {
	# These are out-arcs, so they connect to
	# the bottom of the node
	set connID $dot($conn,connID)
	set coords [$w coords $connID]
	set fromX [lindex $coords 0]
	set fromY [lindex $coords 1]
	set toX [lindex $coords 2]
	set toY [lindex $coords 3]
	# Redraw the line with new source coords outX and outY
	$w coords $connID $outX $outY $toX $toY
    }
}

# Procedure to read in the graph information from a file.
proc ReadFile { f } {
    global dot

    set nodeList {}
    set connList {}
    set topTokenList {}
    set bottomTokenList {}
    set iterConnList {}

    # Open the file and get its file ID
    set fileId [open $f RDONLY]
    # Process the file line by line
    foreach line [split [read $fileId] \n] {
	# Process one line
	# There are four kinds of lines:
	# 1. Lines which specify new nodes
	# 2. Lines which specify dependencies between nodes (directional)
	# 3. Lines which specify top tokens
	# 4. Lines which specify bottom tokens
	# 5. Lines which specify dependencies over iterations (directional)
	# In cdot format there are also lines which begin with
	# keywords "digraph" and "node"

	# Test if the line is specifies a node
	if { [isNode $line] == 1 } {
	    # Re-parse the line for the node name and add it to
	    # the node list
	    set node [lindex $line 1]
	    lappend nodeList $node
#	    puts "node: $node"
	}
	# Test if the line is specifies a connection
	if { [isConn $line] == 1 } {
	    # Re-parse the line for the connection and add it to
	    # the connection list
	    set conn {}
	    lappend conn [lindex $line 1]
	    # From the index-3 item, pick out the label that ends with a ";"
	    # match has the ";" on the end; dest does not
	    regexp {([^;]*);} [lindex $line 3] match dest
	    lappend conn $dest
	    lappend connList $conn
#	    puts "conn: $conn"
	}
	# Test if the line is specifies a top token
	if { [isTopToken $line] == 1 } {
	    # Re-parse the line for the top token name and add it to
	    # the top token list
	    set topToken [lindex $line 1]
	    lappend topTokenList $topToken
#	    puts "topToken:  $topToken"
	}
	# Test if the line is specifies a bottom token
	if { [isBottomToken $line] == 1 } {
	    # Re-parse the line for the bottom token name and add it to
	    # the bottom token list
	    set bottomToken [lindex $line 1]
	    lappend bottomTokenList $bottomToken
#	    puts "bottomToken:  $bottomToken"
	}
	# Test if the line is specifies an iter connection
	if { [isIterConn $line] == 1 } {
	    # Re-parse the line for the connection and add it to
	    # the connection list
	    set iterConn {}
	    lappend iterConn [lindex $line 1]
	    # From the index-3 item, pick out the label that ends with a ";"
	    # match has the ";" on the end; dest does not
	    regexp {([^;]*);} [lindex $line 3] match dest
	    lappend iterConn $dest
	    lappend iterConnList $iterConn
#	    puts "iterConn: $iterConn"
	}
    }
    set dot(nodeList) $nodeList
    set dot(connList) $connList
    set dot(topTokenList) $topTokenList
    set dot(bottomTokenList) $bottomTokenList
    set dot(iterConnList) $iterConnList
}

# Procedure to test if a text line specifies a connection.
proc isConn { line } {
    # Begins with the label "Conn"
    if { [lindex $line 0] == "Conn" } { return 1 }
    set result 0
#    set result [regexp {(->)+} $line]
    return $result
}

# Procedure to test if a text line specifies an iteration connection.
proc isIterConn { line } {
    # Begins with the label "IterConn"
    if { [lindex $line 0] == "IterConn" } { return 1 }
    set result 0
    return $result
}

# Procedure to test if a text line specifies a firing node.
proc isNode { line } {
    # Begins with the label "Node"
    if { [lindex $line 0] == "Node" } { return 1 }
    set result 0
#    # No curly braces
#    if { [regexp {\{|\}} $line] == 1 } { return 0 }
#    # No leading newline
#    if { [regexp {^\n} $line] == 1 } { return 0 }
    return $result
}

# Procedure to test if a text line specifies a top token.
proc isTopToken { line } {
    # Begins with the label "TopToken"
    if { [lindex $line 0] == "TopToken" } { return 1 }
    set result 0
    return $result
}

# Procedure to test if a text line specifies a bottom token.
proc isBottomToken { line } {
    # Begins with the label "BottomToken"
    if { [lindex $line 0] == "BottomToken" } { return 1 }
    set result 0
    return $result
}

# Procedure to set up and display the graph.
proc DisplayGraph {} {
    global dot

    set canvasLeft 0
    set canvasTop 0
    set canvasRight 5000
    set canvasBottom 1000

    set dot(.view.c,canvasLeft) $canvasLeft
    set dot(.view.c,canvasTop) $canvasTop
    set dot(.view.c,canvasRight) $canvasRight
    set dot(.view.c,canvasBottom) $canvasBottom

    frame .view
    canvas .view.c -width 500 -height 500 -scrollregion \
	    " $canvasLeft $canvasTop $canvasRight $canvasBottom " \
	    -xscrollcommand [list .view.xscroll set] \
	    -yscrollcommand [list .view.yscroll set]
    scrollbar .view.xscroll -orient horizontal \
	    -command [list .view.c xview]
    scrollbar .view.yscroll -orient vertical \
	    -command [list .view.c yview]

    set dot(.view.c,nodeName) "No node selected"
    label .view.nodeName -textvariable dot(.view.c,nodeName)

    pack .view.nodeName -side bottom -fill x
    pack .view.xscroll -side bottom -fill x
    pack .view.yscroll -side right  -fill y
    pack .view.c -side left -fill both -expand true
    pack .view -side top -fill both -expand true

    .view.c bind movable <Button-1> {NodeMark %W %x %y}
    .view.c bind movable <B1-Motion> {NodeDrag %W %x %y}
    .view.c bind movable <ButtonRelease-1> {NodeDrop %W %x %y}

    # Initialize each node
    foreach node $dot(nodeList) {
	set dot($node,inConns) {}
	set dot($node,outConns) {}
	set dot($node,level) -1
    }

    # Initialize each topToken
    foreach topToken $dot(topTokenList) {
	set dot($topToken,inConns) {}
	set dot($topToken,outConns) {}
	set dot($topToken,level) -1
    }

    # Initialize each bottomToken
    foreach bottomToken $dot(bottomTokenList) {
	set dot($bottomToken,inConns) {}
	set dot($bottomToken,outConns) {}
	set dot($bottomToken,level) -1
    }

    # Establish each connection and register it with its nodes
    foreach conn $dot(connList) {
	set sourceNode [lindex $conn 0]
	set destNode [lindex $conn 1]

	# Register the ends of the connection with their nodes
	# Sources output onto connections
	set outConns $dot($sourceNode,outConns)
	lappend outConns $conn
	set dot($sourceNode,outConns) $outConns
	# Destinations input from connections
	set inConns $dot($destNode,inConns)
	lappend inConns $conn
	set dot($destNode,inConns) $inConns
    }

    # Calculate the level of each node based on its predecessors
    foreach node $dot(nodeList) {
	Level $node
    }
    foreach node $dot(nodeList) {
#	puts "Leveldone $dot($node,level)"
    }

    set x1 0
    set x2 25
    set y1 50
    set y2 75

    set nprocs 8
    set dot(.view.c,nprocs) $nprocs
    set level 0

    # Loop through all nprocs and draw big lines separating
    # them in columns in the view.
    set yys [expr $dot(.view.c,canvasTop) - 100]
    set yyd [expr $dot(.view.c,canvasBottom) + 100]

    set xxs 50
	set connID [.view.c create line $xxs $yys $xxs $yyd \
		-width 2 ]

    incr xxs 13

    for { set icount 0 } { $icount <= [expr $nprocs] } { incr icount 1 } {
	set connID [.view.c create line $xxs $yys $xxs $yyd \
		-width 2 ]
	incr xxs 100
    }

    incr xxs -87
	set connID [.view.c create line $xxs $yys $xxs $yyd \
		-width 2 ]

    # Draw tokens at both top and bottom
    set xt 112
    foreach topToken $dot(topTokenList) {
	set yt 50
	# Calculate where to place it
	# Draw the topToken
	DrawToken $topToken $xt $yt "top"

	set yt 400
	# Calculate where to place it
	# Draw the topToken
	DrawToken $topToken $xt $yt "bottom"

	set xt [expr $xt + 100]
    }

    foreach bottomToken $dot(bottomTokenList) {
	set yt 50
	# Calculate where to place it
	# Draw the bottomToken
	DrawToken $bottomToken $xt $yt "top"

	set yt 400
	# Calculate where to place it
	# Draw the bottomToken
	DrawToken $bottomToken $xt $yt "bottom"

	set xt [expr $xt + 100]
    }

    foreach node $dot(nodeList) {
	# Calculate the next coordinate set
	if { $x2 >= [expr 100*$nprocs] || $dot($node,level) > $level } {
	    set x1 0
	    set x2 25
	    set y1 [expr $y1+50]
	}
	set level $dot($node,level)

	set y2 [expr $y1 + 25]

	set x1 [expr $x1+100]
	set x2 [expr $x2+100]

	DrawNode $node $x1 $y1 $x2 $y2

	# Set the initial proc placements
	set procnum [expr int(($x1-100)/100)]
	set textID $dot($node,textID)
	set theText [.view.c itemcget $textID -text]
	setProc $theText $procnum
    }

    foreach conn $dot(connList) {
	CreateConn $conn
    }
    foreach iterConn $dot(iterConnList) {
	CreateIterConn $iterConn
    }
}

# Procedure to recursively determine the level of a node in the graph.
proc Level { node } {
    global dot

    set maxPredLevel 0
    # Find the connections leading into it
    foreach conn $dot($node,inConns) {
	# Find the nodes which are predecessors
	set predNode [lindex $conn 0]
	# If it's a topToken, use predLevel = 0
	if { [lsearch $dot(topTokenList) $predNode] >= 0 } {
	    set predLevel 0
	} elseif { $dot($predNode,level) == -1 } {
	    set predLevel [Level $predNode]
	} else {
	    set predLevel $dot($predNode,level)
	}

	if { $predLevel > $maxPredLevel } {
	    set maxPredLevel $predLevel
	}
    }
    set level [expr $maxPredLevel + 1]

    set dot($node,level) $level
#    puts "LEVEL: $node $level"
    return $level
}

# Procedure to create and draw a firing node.
proc DrawNode { node x1 y1 x2 y2 } {
    global dot

    # Create a rect for the node
    set nodeID [.view.c create rect $x1 $y1 $x2 $y2 -fill green \
	    -tag "node $node movable"]
    set textID [.view.c create text [expr $x1+20] [expr $y1+20] \
	    -anchor nw -text $node -tag "node $node"]

    # Associate the rect ID and the node with each other
    set dot($nodeID,node) $node
    set dot($node,nodeID) $nodeID
    set dot($node,textID) $textID
    set dot($node,type) "node"

    # Set top and bottom to be the same thing
    set dot($node,topNodeID) $nodeID
    set dot($node,bottomNodeID) $nodeID
}

# Procedure to create and draw a token.
proc DrawToken { token x y topOrBottom } {
    global dot

    # Create a diamond for the node
    set nodeID [.view.c create poly \
	    [expr $x - 7] $y \
	    $x [expr $y - 10] \
	    [expr $x + 7] $y \
	    $x [expr $y + 10] \
	    -fill white \
	    -outline green \
	    -tag "token $token"]
    set textID [.view.c create text [expr $x+5] [expr $y+5] \
	    -anchor nw -text $token -tag "token $token"]

    # Associate the token ID and the node with each other
    set dot($nodeID,node) $token
    set dot($token,textID) $textID
    set dot($token,type) "token"
    set dot($token,nodeID) $nodeID
    if { $topOrBottom == "top" } {
	set dot($token,topNodeID) $nodeID
    }
    if { $topOrBottom == "bottom" } {
	set dot($token,bottomNodeID) $nodeID
    }
}

# Procedure to create and draw a connection.
proc CreateConn { conn } {
    global dot

    # xs,ys are the sourceNode coords
    set sourceNode [lindex $conn 0]
    set sourceNodeID $dot($sourceNode,topNodeID)
    set sourceInOutCoords [NodeInOutCoords .view.c $sourceNodeID]

    set xs [lindex $sourceInOutCoords 2]
    set ys [lindex $sourceInOutCoords 3]

    # xd,yd are the destNode coords
    set destNode [lindex $conn 1]
    set destNodeID $dot($destNode,bottomNodeID)
    set destInOutCoords [NodeInOutCoords .view.c $destNodeID]

    set xd [lindex $destInOutCoords 0]
    set yd [lindex $destInOutCoords 1]

    # Create a line for the connection
    set connID [.view.c create line $xs $ys $xd $yd \
	    -width 2 -arrow last -tag "conn $conn"]
    # Associate the line ID and the conn with each other
    set dot($connID,conn) $conn
    set dot($conn,connID) $connID
}

# Procedure to create and draw an iteration connection.
proc CreateIterConn { conn } {
    global dot

    # xs,ys are the sourceNode coords
    set sourceNode [lindex $conn 0]
    set sourceNodeID $dot($sourceNode,topNodeID)
#    set sourceNodeID $dot($sourceNode,bottomNodeID)
    set sourceInOutCoords [NodeInOutCoords .view.c $sourceNodeID]

    set xs [lindex $sourceInOutCoords 2]
    set ys [lindex $sourceInOutCoords 3]

    # xd,yd are the destNode coords
    set destNode [lindex $conn 1]
    set destNodeID $dot($destNode,bottomNodeID)
#    set destNodeID $dot($destNode,topNodeID)
    set destInOutCoords [NodeInOutCoords .view.c $destNodeID]

    set xd [lindex $destInOutCoords 0]
    set yd [lindex $destInOutCoords 1]

    # Create a line for the connection
    set connID [.view.c create line $xs $ys $xd $yd \
	    -fill red \
	    -width 2 -arrow last -tag "conn $conn"]
    # Associate the line ID and the conn with each other
    set dot($connID,conn) $conn
    set dot($conn,connID) $connID
}

# procedure to handle an exit request
proc requestExit {} {
    handleExit
}

# Top-level script to set up and display the graph.
button .quit -text Done -command "requestExit"
pack .quit
ReadFile $GRAPH_FILE
DisplayGraph

# END
