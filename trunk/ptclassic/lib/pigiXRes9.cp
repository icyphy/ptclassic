! $Id$
! VEM options for Pigi:
vem.bindings:	$PTOLEMY/lib/vem.bindings.8-1
vem.deftechname:	ptolemy
vem.grid.major.units:	100
vem.grid.minor.units:	10
vem.lambda:	1
vem*logfile:	/dev/null
vem*snap:	25
vem*technology:	$PTOLEMY/lib/colors

! The following are optional.  They can be overriden by the user.
vem*font: 9x15
vem*wrap: word
vem*grid*zero*size: 0
vem*label*boundbox: 10
vem*button*highlightThickness: 5
vem*edit*borderWidth: 3
#ifdef COLOR
vem*cursor*color: red
vem*background: white
vem*foreground: black
vem.background.color: #cdcdc1
vem.grid.zero.color: #cdcdc1
vem*menu*item*background: LightGray
vem*button*foreground: IndianRed
vem*title*foreground: black
#else
*Background:    white
*Foreground:    black
#endif

! Ptolemy resources for xterm windows
ptolemy_code*Background:	LightGray
ptolemy_code*Foreground: 	midnightBlue
! the ptolemy resources below are not currently implemented
ptolemy_sim*Background:		black
ptolemy_sim*Foreground: 	green
ptolemy_man*Background:		black
ptolemy_man*Foreground: 	orange

pxgraph.Background: white
pxgraph.ZeroColor: navy
pxgraph.Markers: off
pxgraph.Foreground: black

! Ptolemy resources for Xedit
Xedit*input:			TRUE
Xedit*filename*preferredPaneSize:	525
Xedit*editWindow*preferredPaneSize:	500
Xedit*labelWindow*justify:		center
Xedit*labelWindow*label:		no file yet
Xedit*quit.label:			Quit
Xedit*save.label:			Save
Xedit*load.label:			Load
Xedit*buttons*orientation:		horizontal
Xedit*buttons*showGrip:			False
Xedit*messageWindow*preferredPaneSize:	50
Xedit*Paned*Text*allowResize:		True
Xedit*messageWindow.autoFill:		True
Xedit*messageWindow.scrollVertical:	WhenNeeded
Xedit*editWindow.autoFill:		True
Xedit*editWindow.scrollVertical:	WhenNeeded
Xedit*bc_label*label:			Use Control-S and Control-R to Search.
!
! Keep CR in filename window from confusing the user.
!
Xedit*filename.translations: #override \
				<Key>Return: end-of-line()
Xedit*filename.baseTranslations: #override \
				<Key>Return: end-of-line()
