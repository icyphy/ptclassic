! $Id$
! VEM options for Pigi for Color Printing:

vem.bindings:	$PTOLEMY/lib/vem.bindings.8-1
vem*deftechname:	ptolemy
vem*grid*major*units:	100
vem*grid*minor*units:	10
vem*lambda:	1
vem*logfile:	/dev/null
vem*snap:	25
vem.technology:       $PTOLEMY/lib/tech4colorPrint

! The following are optional.  They can be overriden by the user.
vem*font: 9x15
vem*wrap: word
vem*grid*zero*size: 0
vem*label*boundbox: 10
vem*button*highlightThickness: 5
vem*edit*borderWidth: 3
#ifdef COLOR
vem.cursor.color: red
vem.background: white
vem.background.color: white
vem.grid.zero.color: white
vem.menu.item.background: LightGray
vem.button.foreground: IndianRed
vem.title.foreground: black
#else
*Background:    white
*Foreground:    black
#endif

! Ptolemy resources for xterm windows
ptolemy_code.Background:	white
ptolemy_code.Foreground: 	midnightBlue
! the ptolemy resources below are not currently implemented
ptolemy_sim.Background:		black
ptolemy_sim.Foreground: 	white
ptolemy_man.Background:		black
ptolemy_man.Foreground: 	orange

xgraph.Background: white
xgraph.ZeroColor: navy
xgraph.Markers: off
xgraph.Foreground: black
binxgraph.Background: white
binxgraph.ZeroColor: navy
binxgraph.Markers: off
binxgraph.Foreground: black
