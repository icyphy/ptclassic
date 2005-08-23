#!/usr/local/bin/wish
#
# Version: @(#)TkSched.tcl	1.13 02/20/98
# Written by Michael C. Williamson
#
# Copyright (c) 1990-1997 The Regents of the University of California.
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
# It is used to display a graphical representation of the schedule
# and hardware resource planning for an application.

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
    set dot($w,nodeName) $dot($node,name)

    # Highlight the node
    $w itemconfig $dot($w,nodeID) -fill red
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
#    set procnum [expr ($cx - (($cx - 62) % 100)) / 100]
    set procnum [XToProc $x]
    # Check that the procnum is within bounds
    set nprocs $dot($w,nprocs)
    if { $procnum < 0 } {
	set procnum 0
    }
    if { $procnum > [expr $nprocs - 1] } {
	set procnum [expr $nprocs - 1]
    }

    setProc $dot($node,name) $procnum

    # Update the node data.
    set dot($node,procnum) $procnum

    set newx [expr ($procnum * 100) + 100 + ($x - $leftX)]

    # Figure out the bounds of this node based on its input
    # and output arcs.  Don't allow it to move outside its
    # precedence constraints.
    set topBound [TimeToY $dot(startline)]
    set bottomBound [TimeToY $dot(deadline)]

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

    PrintIntervals
    UpdateStats
}

# Procedure to drag a node to a new location.
proc NodeDrag { w x y } {
    global dot

    if { $dot($w,active) == 0 } { return }

    # Determine the x and y deltas based on last canvas coordinates.
    set dx [expr int($x - $dot($w,x))]
    set dy [expr int($y - $dot($w,y))]

    # Update the canvas coordinates.
    set dot($w,x) $x
    set dot($w,y) $y

    # Move the node.
    $w move $dot($w,nodeID) $dx $dy
    $w move $dot($w,textID) $dx $dy
    MoveArcs $w $dot($w,node)

    # Determine the new start, end, and proc of the node.
    set node $dot($w,node)
    set oldStart $dot($node,startTime)
    set oldEnd $dot($node,endTime)
    set oldproc $dot($node,procnum)
    set newStart [expr $oldStart + $dy]
    set newEnd [expr $oldEnd + $dy]
    set newproc [XToProc $x]

#    puts "Oldproc: $oldproc Newproc: $newproc"
#    ChangeProcs $oldproc $newproc $node
#    ChangeTimes $oldStart $oldEnd $newStart $newEnd $node

    # Update the node data.
    set dot($node,startTime) $newStart
    set dot($node,endTime) $newEnd
    set dot($node,procnum) $newproc

    # Update the interval lists.
    MoveIntervalTemporal $oldproc $oldStart $oldEnd $newStart $newEnd
    MoveIntervalSpatial $oldproc $newproc $newStart $newEnd
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
proc MoveArcs { w node } {
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

    # Establish a default value for the startline, deadline
    set dot(startline) 0
    set dot(deadline) 500

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
	    set latency [lindex $line 3]
	    set procnum [lindex $line 5]
	    lappend nodeList $node
	    set dot($node,latency) $latency
	    set dot($node,procnum) $procnum
#	    puts "node: $node  latency: $dot($node,latency)"
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

    set nprocs [llength $dot(nodeList)]
    set dot(.view.c,nprocs) $nprocs

    set initWidth [expr $nprocs*100 + 113]
    set initHeight [expr $dot(deadline) - $dot(startline) + 150]

    set dot(.view.c,canvasLeft) $canvasLeft
    set dot(.view.c,canvasTop) $canvasTop
    set dot(.view.c,canvasRight) $canvasRight
    set dot(.view.c,canvasBottom) $canvasBottom

    frame .view
    frame .view.bottom
    canvas .view.new -width 300 -height $initHeight -scrollregion \
	    " $canvasLeft $canvasTop $canvasRight $canvasBottom " \
	    -background green \
	    -xscrollcommand [list .view.xscroll set] \
	    -yscrollcommand [list .view.yscroll set]
    canvas .view.c -width $initWidth -height $initHeight -scrollregion \
	    " $canvasLeft $canvasTop $canvasRight $canvasBottom " \
	    -xscrollcommand [list .view.xscroll set] \
	    -yscrollcommand [list .view.yscroll set]
    scrollbar .view.xscroll -orient horizontal \
	    -command [list .view.c xview]
    scrollbar .view.yscroll -orient vertical \
	    -command [list .view.c yview]
    set padx [expr [.view.xscroll cget -width] + 2* \
	    ([.view.xscroll cget -bd] + \
	    [.view.xscroll cget -highlightthickness])]
    set pady [expr [.view.yscroll cget -width] + 2* \
	    ([.view.yscroll cget -bd] + \
	    [.view.yscroll cget -highlightthickness])]
    frame .view.pad -width $padx -height $pady

    set dot(.view.c,nodeName) "No node selected"
    label .view.nodeName -textvariable dot(.view.c,nodeName)
    set dot(.view.c,procsUsed) ""
    label .view.procsUsed -textvariable dot(.view.c,procsUsed)
    set dot(.view.c,lastFinish) ""
    label .view.lastFinish -textvariable dot(.view.c,lastFinish)
    set dot(.view.c,throughput) ""
    label .view.throughput -textvariable dot(.view.c,throughput)

#    pack .view.new -side right -fill both -expand true
    pack .view.nodeName -side bottom -fill x
    pack .view.procsUsed -side bottom -fill x
    pack .view.lastFinish -side bottom -fill x
    pack .view.throughput -side bottom -fill x

    pack .view.bottom -side bottom -fill x
    pack .view.pad -in .view.bottom -side right
    pack .view.xscroll -in .view.bottom -side bottom -fill x
    pack .view.c -side left -fill both -expand true
    pack .view.yscroll -side right  -fill y
    pack .view -side top -fill both -expand true

    .view.c bind movable <Button-1> {NodeMark %W %x %y}
    .view.c bind movable <B1-Motion> {NodeDrag %W %x %y}
    .view.c bind movable <ButtonRelease-1> {NodeDrop %W %x %y}

    # Initialize each node
    foreach node $dot(nodeList) {
	set dot($node,name) $node
	set dot($node,inConns) {}
	set dot($node,outConns) {}
	set dot($node,level) -1
	set dot($node,startTime) 0
	set dot($node,endTime) 0
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

    set level 0

    # Initialize the interval lists for each proc.
    for { set icount -1 } { $icount < [expr $nprocs] } { incr icount 1 } {
	set dot($icount,intervals) {}
    }

    # Loop through all nprocs and draw big lines separating
    # them in columns in the view.
    set yys [expr $dot(.view.c,canvasTop) - 100]
    set yyd [expr $dot(.view.c,canvasBottom) + 100]

    set xxs 50
    set leftMost $xxs
	set connID [.view.c create line $xxs $yys $xxs $yyd \
		-width 2 ]

    incr xxs 13

    for { set icount 0 } { $icount <= [expr $nprocs] } { incr icount 1 } {
	set connID [.view.c create line $xxs $yys $xxs $yyd \
		-width 2 ]
	incr xxs 100
    }

    incr xxs -87
    set rightMost $xxs
    set connID [.view.c create line $xxs $yys $xxs $yyd \
	    -width 2 ]


    set xxs 50
    set xxd $rightMost
    set yys [expr [TimeToY $dot(startline)]]
    set connID [.view.c create line $xxs $yys $xxd $yys \
	    -width 2 -fill red]
    set yys [expr [TimeToY $dot(deadline)]]
    set connID [.view.c create line $xxs $yys $xxd $yys \
	    -width 2 -fill red]

    set xpos 113
    # Draw total times and utilizations under each proc's column.
    for { set procnum 0 } { $procnum < [expr $nprocs] } { incr procnum 1 } {
	# Draw a num-tasks text item.
	set dot($procnum,numTasks) 0
	set textID [.view.c create text [expr $xpos] \
		[expr [TimeToY $dot(deadline)] + 40] \
		-anchor n -text $dot($procnum,numTasks)]
	set dot($procnum,numTasksID) $textID
	# Draw a total-time text item.
	set dot($procnum,totTime) 0
	set textID [.view.c create text [expr $xpos] \
		[expr [TimeToY $dot(deadline)] + 60] \
		-anchor n -text $dot($procnum,totTime)]
	set dot($procnum,totTimeID) $textID
	# Draw a utilization text item.
	set dot($procnum,util) 0
	set textID [.view.c create text [expr $xpos] \
		[expr [TimeToY $dot(deadline)] + 80] \
		-anchor n -text "$dot($procnum,util)%"]
	set dot($procnum,utilID) $textID
	# Calculate the next drawing point.
	incr xpos 100
    }

    # Draw a time scale on the left and the right.
    set timeIncr 50
    set time 0
    for { set time $dot(startline) } { $time <= $dot(deadline) } \
	    { incr time $timeIncr } {
	set y [TimeToY $time]
	set x [expr $leftMost - 3]
	set textID [.view.c create text [expr $x] [expr $y - 5] \
		-anchor ne -text $time]
	set x [expr $rightMost + 3]
	set textID [.view.c create text [expr $x] [expr $y - 5] \
		-anchor nw -text $time]
    }

    # Draw tokens at both top and bottom.
    set xt 112
    foreach topToken $dot(topTokenList) {
	set yt [expr [TimeToY $dot(startline)] - 10]
	# Draw the topToken
	DrawToken $topToken $xt $yt "top"

	set yt [expr [TimeToY $dot(deadline)] + 10]
	# Draw the topToken
	DrawToken $topToken $xt $yt "bottom"

	set xt [expr $xt + 100]
    }

    foreach bottomToken $dot(bottomTokenList) {
	set yt [expr [TimeToY $dot(startline)] - 10]
	# Calculate where to place it
	# Draw the bottomToken
	DrawToken $bottomToken $xt $yt "top"

	set yt [expr [TimeToY $dot(deadline)] + 10]
	# Calculate where to place it
	# Draw the bottomToken
	DrawToken $bottomToken $xt $yt "bottom"

	set xt [expr $xt + 100]
    }

    set startTime 0
    foreach node $dot(nodeList) {
	# Calculate the next location.
	if { $dot($node,level) > $level } {
	    set startTime [expr $startTime + 50]
	}
	set level $dot($node,level)
	set endTime [expr $startTime + $dot($node,latency)]
	# dot($node,procnum) is set in the graph file.
	set procnum $dot($node,procnum)

	DrawNode $node $procnum $startTime $endTime

	# Register the initial proc placements
	setProc $dot($node,name) $procnum

	# Update the node data.
	set dot($node,procnum) $procnum
    }

    foreach conn $dot(connList) {
	CreateConn $conn
    }
    foreach iterConn $dot(iterConnList) {
	CreateIterConn $iterConn
    }

    UpdateStats
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

    puts "Level: NODE: $dot($node,name)   LEVEL: $node $level"

    return $level
}

# Procedure to recursively move a node and its predecessors to pack up.
# This keeps them assigned to the same resource.
proc UpPack { node } {
    global dot

    set w .view.c

    set maxPredLevel $dot(startline)
    # Find the connections leading into it
    foreach conn $dot($node,inConns) {
	# Find the nodes which are predecessors
	set predNode [lindex $conn 0]
	# If it's a topToken, use predLevel = $dot(startline)
	if { [lsearch $dot(topTokenList) $predNode] >= 0 } {
	    set predLevel $dot(startline)
	} else {
	    UpPack $predNode
	    set predLevel $dot($predNode,endTime)
	}
	if { $predLevel > $maxPredLevel } {
	    set maxPredLevel $predLevel
	}
    }
    set level [expr $maxPredLevel]
    set oldStart $dot($node,startTime)
    set oldEnd $dot($node,endTime)
    # Now make sure that you don't overrun the occupied intervals.
    set proc [ProcOfNode $node]
    set latest [LatestOcc $proc $oldStart]
    if { $latest > $level } {
	set level $latest
    }

#    puts "UpPack: NODE: $dot($node,name)   LEVEL: $level"

    set newStart $level
    set dy [expr $newStart - $dot($node,startTime)]
    set newEnd [expr $oldEnd + $dy]

    ChangeTimes $oldStart $oldEnd $newStart $newEnd $node
}

# Procedure to recursively move a node and its predecessors to pack down.
# This keeps them assigned to the same resource.
proc DownPack { node } {
    global dot

    set w .view.c

    set minSuccLevel $dot(deadline)
    # Find the connections leading out of it
    foreach conn $dot($node,outConns) {
	# Find the nodes which are successors
	set succNode [lindex $conn 1]
	# If it's a topToken, use succLevel = $dot(deadline)
	if { [lsearch $dot(topTokenList) $succNode] >= 0 } {
	    set succLevel $dot(deadline)
	} else {
	    DownPack $succNode
	    set succLevel $dot($succNode,startTime)
	}
	if { $succLevel < $minSuccLevel } {
	    set minSuccLevel $succLevel
	}
    }
    set level [expr $minSuccLevel]
    set oldStart $dot($node,startTime)
    set oldEnd $dot($node,endTime)
    # Now make sure that you don't overrun the occupied intervals.
    set proc [ProcOfNode $node]
    set soonest [SoonestOcc $proc $oldEnd]
    if { $soonest < $level } {
	set level $soonest
    }

#    puts "DownPack: NODE: $dot($node,name)   LEVEL: $level"

    set newEnd $level
    set dy [expr $newEnd - $dot($node,endTime)]
    set newStart [expr $oldStart + $dy]

    ChangeTimes $oldStart $oldEnd $newStart $newEnd $node
}

# Return the procnum based on the x-coordinate.
proc XToProc { x } {
    return [expr int((int($x)-63)/100)]
}

# Return the x-coordinate based on the procnum.
# Note that this returns the x-coordinate of the left side of a task.
proc ProcToX { proc } {
    return [expr int((int($proc)*100)+100)]
}

# Return the time based on the y-coordinate.
proc YToTime { y } {
    return [expr int(int($y)-50)]
}

# Return the y-coordinate based on the time.
proc TimeToY { time } {
    return [expr int(int($time)+50)]
}

# Print out all the intervals occupied for each resource.
proc PrintIntervals {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    puts {}
    for { set procnum -1 } { $procnum < $nprocs } { incr procnum 1 } {
	puts "Proc: $procnum    Intervals: $dot($procnum,intervals)"
    }
    puts {}
}

# Update the values of the performance statistics and display them.
proc UpdateStats {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    # Recalculate the totTime and util for each proc.
    for { set procnum 0 } { $procnum < $nprocs } { incr procnum 1 } {
	set dot($procnum,numTasks) [NumTasks $procnum]
	if { $dot($procnum,numTasks) == 1 } {
	    .view.c itemconfig $dot($procnum,numTasksID) \
		    -text "$dot($procnum,numTasks) task"
	} else {
	    .view.c itemconfig $dot($procnum,numTasksID) \
		    -text "$dot($procnum,numTasks) tasks"
	}
	set dot($procnum,totTime) [TotTime $procnum]
	.view.c itemconfig $dot($procnum,totTimeID) \
		-text "$dot($procnum,totTime) nsec"
	set dot($procnum,util) [Util $procnum]
	.view.c itemconfig $dot($procnum,utilID) \
		-text "$dot($procnum,util)% util"
    }

    set dot($w,procsUsed) "Procs Used: [ProcsUsed]"
    set lastFinish [LastFinish]
    # Don't allow zero, since throughput would be infinite.
    # (divide by zero would occur)
    if { $lastFinish == 0 } {
	set lastFinish 1
    }
    set dot($w,lastFinish) "Last Finish: $lastFinish nsec"
    # If LastFinish is in nanoseconds, then throughput
    # will be in MHz.  Do a calculation:
    set throughput [expr 1000.0/$lastFinish]
    # Round it to two decimal places.
    set throughput [expr int(100.0*$throughput)/100.0]
    set dot($w,throughput) "Max Throughput: $throughput MHz"
}

# Return the finish time of the last interval to finish.
proc LastFinish {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    set lastFinish 0
    # Recalculate the lastFinish for each proc.
    for { set procnum 0 } { $procnum < $nprocs } { incr procnum 1 } {
	foreach interval $dot($procnum,intervals) {
	    set finish [lindex $interval 1]
	    if { $finish > $lastFinish } {
		set lastFinish $finish
	    }
	}
    }

    return $lastFinish
}

# Return the number of procs utilized.
proc ProcsUsed {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    set numUsed 0
    # Recalculate the number of procs used.
    for { set procnum 0 } { $procnum < $nprocs } { incr procnum 1 } {
	set used 0
	foreach interval $dot($procnum,intervals) {
	    set used 1
	}
	incr numUsed $used
    }

    return $numUsed
}

# Return the total number of tasks for the given proc.
proc NumTasks { procnum } {
    global dot

    set total 0
    foreach interval $dot($procnum,intervals) {
	# One interval per task.
	incr total 1
    }
    return $total
}

# Return the total time occupied for the given proc.
proc TotTime { procnum } {
    global dot

    set total 0
    foreach interval $dot($procnum,intervals) {
	# Calculate the interval length
	set iStart [lindex $interval 0]
	set iEnd [lindex $interval 1]
	set length [expr $iEnd - $iStart + 1]
	incr total $length
    }
    return $total
}

# Return the utilization for the given proc.
proc Util { procnum } {
    global dot

    # Util is a percentage figure of utilization.
    set runLength [expr $dot(deadline) - $dot(startline)]
    set util [expr int((100*$dot($procnum,totTime))/$runLength)]
    return $util
}

# Test if a resource is occupied anywhere within the given interval.
proc Unoccupied { procnum start end } {
    set unoccupied 1
    if { [Occupied $procnum $start $end] } {
	set unoccupied 0
    }
    return $unoccupied
}

# Test if a resource is occupied anywhere within the given interval.
proc Occupied { procnum start end } {
    global dot

    set occupied 0
    foreach interval $dot($procnum,intervals) {
	set iStart [lindex $interval 0]
	set iEnd [lindex $interval 1]
	# Screen out cases where one falls inside the other,
	# but not those which just overlap at one end.
	# If the input start falls in the interval, it's occupied.
	if { $iStart < $start && $start < $iEnd } {
	    set occupied 1
	}
	# If the input end falls in the interval, it's occupied.
	if { $iStart < $end && $end < $iEnd } {
	    set occupied 1
	}
	# If the interval start falls in the input interval, it's occupied.
	if { $start < $iStart && $iStart < $end } {
	    set occupied 1
	}
	# If the interval end falls in the input interval, it's occupied.
	if { $start < $iEnd && $iEnd < $end } {
	    set occupied 1
	}
	# If the two intervals coincide, it's occupied.
	if { $start == $iStart && $end == $iEnd } {
	    set occupied 1
	}
    }
    return $occupied
}

# Return the soonest time after the given time
# at which the given resource is occupied.
proc SoonestOcc { procnum time } {
    global dot

    set soonest $dot(deadline)
    foreach interval $dot($procnum,intervals) {
	set iStart [lindex $interval 0]
	set iEnd [lindex $interval 1]
	# Ignore zero-length intervals
	if { $iStart != $iEnd } {
	    # Consider intervals that start at or after the input time.
	    if { $time <= $iStart } {
		if { $iStart < $soonest } {
		    set soonest $iStart
		}
	    }
	}
    }
    return $soonest
}

# Return the latest time before the given time
# at which the given resource is occupied.
proc LatestOcc { procnum time } {
    global dot

    set latest $dot(startline)
    foreach interval $dot($procnum,intervals) {
	set iStart [lindex $interval 0]
	set iEnd [lindex $interval 1]
	# Ignore zero-length intervals
	if { $iStart != $iEnd } {
	    # Consider intervals that end before or at the input time.
	    if { $iEnd <= $time } {
		if { $latest < $iEnd } {
		    set latest $iEnd
		}
	    }
	}
    }
    return $latest
}

# Return the proc of the given node.
# Currently, this is calculated from the x-coordinate.
proc ProcOfNode { node } {
    global dot

    set w .view.c

    set nodeID $dot($node,nodeID)
    set box [$w coords $nodeID]
    set oldx [lindex $box 0]
    set oldproc [XToProc $oldx]
}

# Move a node to pack left.
# This moves it to the lowest-index contemporaneously-unoccupied resource.
proc LeftPack { node } {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)
    set startTime $dot($node,startTime)
    set endTime $dot($node,endTime)
    set oldproc [ProcOfNode $node]

    # Don't bother if it's already at procnum 0.
    if { $oldproc == 0 } {
	return
    }

    # Find the leftmost unoccupied proc.
    set newproc $oldproc
    while { $newproc > 0 && \
 	    [Unoccupied [expr $newproc-1] $startTime $endTime] } {
	incr newproc -1
    }

    # Don't bother if can't find a suitable newproc.
    if { ($newproc >= $nprocs) || ($newproc >= $oldproc) } {
	return
    }

    ChangeProcs $oldproc $newproc $node
}

# Move a node to pack right.
# This moves it to the highest-index contemporaneously-unoccupied resource.
proc RightPack { node } {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)
    set startTime $dot($node,startTime)
    set endTime $dot($node,endTime)
    set oldproc [ProcOfNode $node]

    # Don't bother if it's already at procnum (nprocs-1).
    if { $oldproc == [expr $nprocs-1] } {
	return
    }

    # Find the rightmost unoccupied proc.
    set newproc $oldproc
    while { $newproc < [expr $nprocs-1] && \
 	    [Unoccupied [expr $newproc+1] $startTime $endTime] } {
	incr newproc 1
    }

    # Don't bother if can't find a suitable newproc.
    if { ($newproc < 0) || ($newproc <= $oldproc) } {
	return
    }

    ChangeProcs $oldproc $newproc $node
}

# Move the given node from the oldproc to the newproc.
proc ChangeProcs { oldproc newproc node } {
    global dot

    set w .view.c

    set nodeID $dot($node,nodeID)
    set textID $dot($node,textID)
    set startTime $dot($node,startTime)
    set endTime $dot($node,endTime)
    set oldx [ProcToX $oldproc]
    set newx [ProcToX $newproc]
    set dx [expr $newx - $oldx]

    # Move it over to the new proc
    $w move $nodeID $dx 0
    $w move $textID $dx 0
    MoveArcs $w $node

    # Callback the C++ target to set the proc placement.
    setProc $dot($node,name) $newproc

    # Update the node data.
    set dot($node,procnum) $newproc

    # Update the interval lists.
    MoveIntervalSpatial $oldproc $newproc $startTime $endTime
}

# Move the given node from the old times to the new times.
proc ChangeTimes { oldStart oldEnd newStart newEnd node } {
    global dot

    set w .view.c

    set nodeID $dot($node,nodeID)
    set textID $dot($node,textID)
    set proc [ProcOfNode $node]
    set dy [expr $newStart - $oldStart]

    # Move it to the new time.
    $w move $nodeID 0 $dy
    $w move $textID 0 $dy
    MoveArcs $w $node

    # Update the node data.
    set dot($node,startTime) $newStart
    set dot($node,endTime) $newEnd

    # Update the interval lists.
    MoveIntervalTemporal $proc $oldStart $oldEnd $newStart $newEnd
}

# Add the interval to the new proc and delete it from the old proc.
proc MoveIntervalSpatial { oldproc newproc startTime endTime } {
    # Add the interval to the new proc's intervals.
    AddInterval $newproc $startTime $endTime
    # Delete the interval from the old proc's intervals.
    DelInterval $oldproc $startTime $endTime
}

# Add the new interval to the proc and delete the old one from the proc.
proc MoveIntervalTemporal { proc oldStartTime oldEndTime newStartTime \
	newEndTime } {
    # Add the new interval to the proc's intervals.
    AddInterval $proc $newStartTime $newEndTime
    # Delete the old interval from the proc's intervals.
    DelInterval $proc $oldStartTime $oldEndTime
}

# Add the interval to the new proc's intervals.
proc AddInterval { proc startTime endTime } {
    global dot

    set intervals $dot($proc,intervals)
    set newIntervals [ladd $intervals "$startTime $endTime"]
    set dot($proc,intervals) $newIntervals
}

# Delete the interval from the old proc's intervals.
proc DelInterval { proc startTime endTime } {
    global dot

    set intervals $dot($proc,intervals)
    set newIntervals [ldelete $intervals "$startTime $endTime"]
    set dot($proc,intervals) $newIntervals
}

# Procedure to create and draw a firing node.
proc DrawNode { node procnum startTime endTime } {
    global dot

    set x1 [ProcToX $procnum]
    set x2 [expr $x1+25]
    set y1 [TimeToY $startTime]
    set y2 [TimeToY $endTime]

    # Create a rect for the node
    set nodeID [.view.c create rect $x1 $y1 $x2 $y2 -fill green \
	    -tag "node $node movable"]
    set textID [.view.c create text [expr $x2+5] [expr $y1+5] \
	    -anchor nw -text $node -tag "node $node"]

    # Associate the rect ID and the node with each other.
    set dot($nodeID,node) $node
    set dot($node,nodeID) $nodeID
    set dot($node,textID) $textID
    set dot($node,type) "node"

    # Set top and bottom to be the same thing.
    set dot($node,topNodeID) $nodeID
    set dot($node,bottomNodeID) $nodeID

    # Set the initial startTime, endTime, and procnum. 
    set dot($node,startTime) $startTime
    set dot($node,endTime) $endTime
    set dot($node,procnum) $procnum

    # Add the interval to the proc's intervals.
    AddInterval $procnum $startTime $endTime
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
	    -fill blue \
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
    setAllTimes
}

# Call setTimes for each node.
proc setAllTimes {} {
    global dot

    foreach node $dot(nodeList) {
	set name $node
	set startTime $dot($node,startTime)
	set endTime $dot($node,endTime)
	set latency $dot($node,latency)
	setTimes $name $startTime $endTime $latency
    }
}

# Return a list which is the argument list appended with
# the given value, if that value is not already in the list.
# Else return the list unchanged.
#proc ladduniq { inputList value } {
##    puts "Value: $value"
#    set ix [lsearch $inputList $value]
#    if { $ix >= 0 } {
#	return $inputList
#    } else {
#	return [lappend inputList $value]
#    }
#}

# Return a list which is the argument list appended with
# the given value, even if that value is already in the list.
proc ladd { inputList value } {
#    puts "Value: $value"
    return [lappend inputList $value]
}

# Return a list which is the argument list minus the first instance
# of the given value, if that value appears in the list.
# Else return the list unchanged.
proc ldelete { inputList value } {
#    puts "Value: $value"
    set ix [lsearch $inputList $value]
    if { $ix >= 0 } {
	return [lreplace $inputList $ix $ix]
    } else {
	return $inputList
    }
}

# Spread out all nodes among the procs.
proc SpreadAll {} {
    global dot
    set w .view.c

    set procnum -1
    foreach node $dot(nodeList) {
	# Increment procnum for the next node.
	incr procnum 1

	set nodeID $dot($node,nodeID)
	set textID $dot($node,textID)
	set startTime $dot($node,startTime)
	set endTime $dot($node,endTime)

	set box [$w coords $nodeID]
	set oldx [lindex $box 0]
	set oldproc [XToProc $oldx]
	set newproc $procnum

	if { $newproc == $oldproc } {
	    continue
	}

	ChangeProcs $oldproc $newproc $node
    }
    UpdateStats
}

# procedure to move UpPack all nodes.
proc UpPackAll {} {
    global dot
    foreach node $dot(nodeList) {
	UpPack $node
    }
    UpdateStats
}

# procedure to move DownPack all nodes.
proc DownPackAll {} {
    global dot
    foreach node $dot(nodeList) {
	DownPack $node
    }
    UpdateStats
}

# procedure to move LeftPack all nodes.
proc LeftPackAll {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    # Go through procs from left to right and
    # go through the nodes on each proc.
    for { set procnum 0 } { $procnum < $nprocs } { incr procnum 1 } {
	foreach node $dot(nodeList) {
	    if { $procnum == [ProcOfNode $node] } {
		LeftPack $node
	    }
	}
    }
    UpdateStats
}

# procedure to move RightPack all nodes.
proc RightPackAll {} {
    global dot

    set w .view.c
    set nprocs $dot($w,nprocs)

    # Go through procs from right to left and
    # go through the nodes on each proc.
    for { set procnum [expr $nprocs-1] } { $procnum > -1 } { incr procnum -1 } {
	foreach node $dot(nodeList) {
	    if { $procnum == [ProcOfNode $node] } {
		RightPack $node
	    }
	}
    }
    UpdateStats
}

# Top-level script to set up and display the graph.
frame .menubar

button .spread -text Spread -command "SpreadAll"
button .upPack -text UpPack -command "UpPackAll"
button .downPack -text DownPack -command "DownPackAll"
button .leftPack -text LeftPack -command "LeftPackAll"
button .rightPack -text RightPack -command "RightPackAll"
button .setAll -text Times -command "setAllTimes"
button .done -text Done -command "requestExit"

pack .done -in .menubar -side right
pack .spread -in .menubar -side left
pack .upPack -in .menubar -side left
pack .downPack -in .menubar -side left
pack .leftPack -in .menubar -side left
pack .rightPack -in .menubar -side left
pack .setAll -in .menubar -side left

pack .menubar -side bottom
ReadFile $GRAPH_FILE
DisplayGraph

# END
