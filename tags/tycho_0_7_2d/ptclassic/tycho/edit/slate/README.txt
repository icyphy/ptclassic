README.txt
@(#)README.txt	1.3 07/29/98

This is the README.txt file for the Tycho
package slate (also known as tycho.edit.slate).

DESCRIPTION

     The Tycho Slate is a Tcl/Tk package that implements a
     useful layer of abstraction over the regular Tk canvas. The
     Slate is part of the Tycho user interface system, but can be
     installed and used by itself as well. It runs with either: 

          Tcl 8.0; or
          [incr Tcl] 3.0

     In the [incr Tcl] version, a slate is an [incr Tk] megawidget
     that contains a Tk canvas. In the Tcl version, a slate is a
     "faked" mega-widget, that can be invoked with an
     object-oriented style of interface or a Tcl-like procedural
     style of interface. 

     The Slate contains a number of features that we believe make
     the Tk canvas more useful. We wrote it to help us implement
     graphical editors and visualization widgets. The key features
     of the Slate are: 

     * User-defined item types 
     * Shapes and features 
     * Events and interactors 

     The Slate also contains other useful methods, such as item
     highlighting, selection, and so on. To see an example of an
     editor and some widgets created with the Slate, see the
     Continuous time simulation demo screen-shot page
     (http://ptolemy.eecs.berkeley.edu/~johnr/research/ct-demo.html).

CURRENT RELEASE

     3.0 alpha

     Feedback on this package is appreciated. This is
     the first version of Slate that works with plain Tcl.

DISTRIBUTION

     This distribution can be obtained from

         http://ptolemy.eecs.berkeley.edu/~johnr/code/slate/

     The Tycho home page is always at

         http://ptolemy.eecs.berkeley.edu/tycho


John Reekie
Ptolemy project, UC Berkeley
July 23rd, 1998
