# $Id$
# Author: J. Buck
# Help system for ptcl.

proc add_to_help {cmd argl desc} {
	global argl_array
	global desc_array
	set argl_array($cmd) $argl
	set desc_array($cmd) $desc
	return
}

if [info exists argl_array] then {unset argl_array}
if [info exists desc_array] then {unset desc_array}

add_to_help alias {<galportname> <starname> <starportname>} {
Add a porthole to the current galaxy, and alias it to a porthole
belonging to a contained star or galaxy.
}

add_to_help animation {?on/off?} {
With no aruments, indicate whether animation is turned on or off.  An
argument of "on" enables animation; an argument of "off" disables it.  If
animation is enabled, then the names of stars are printed on the standard
output as they are fired.
}

add_to_help busconnect {<srcstar> <srcport> <dststar> <dstport> <width> ?<delay>?} {
Form a bus connection between two multiportholes belonging to blocks
within the current galaxy.  <width> is an expression specifying the width
of the bus (how many portholes in the multiportholes); <delay> is an
optional expression giving the delay on each connection.
}

add_to_help connect {<srcstar> <srcport> <dststar> <dstport> ?<delay>?} {
Form a connection between two portholes belonging to blocks within the
current galaxy.  <delay> is optional; if given, it is an integer
expression that is evaluated to determine the delay on the arc.
}

add_to_help cont {?<n>?} {
Continue and execute <n> more times, or <n> more time units.
<n> is optional; default is last value of a run or cont argument
(1 if never given).
}

add_to_help defgalaxy {<newclass> {<galaxy-building-commands>}} {
Define a new class of galaxy.  The first argument is the galaxy class
name; the second argument is a list containing a sequence of ptcl
commands. The main ones you'll want to use are domain, star, connect, alias,
newstate, setstate, and descriptor.  The galaxy definition is
added to the known list and you can create instances of the galaxy within
other galaxies.
}

add_to_help delnode {<node>} {
Delete the named node from the current galaxy.
}

add_to_help delstar {<star>} {
Delete the named star from the current galaxy.
}

add_to_help descriptor {?<desc>?} {
If an argument is given, set the descriptor of the current galaxy to the
indicated text.  Otherwise return the descriptor.
}

add_to_help disconnect {<starname> <portname>} {
Disconnect the given porthole from whatever it is connected to.
}

add_to_help domain {?<newdomain>?} {
With an argument, change the current domain to the one specified.
Otherwise return the current domain.
}

add_to_help domains {} {
Return a list of known domains.
}

add_to_help help {?<command>?} {
If no arguments are given, you will see this message.  If a command name
is given, a short description of the given command will be printed.
Help is only available for commands added by ptcl to Standard Tcl.

For a list of commands that have documentation, type "help ?"
}

add_to_help knownlist {?<domain>?} {
Return the list of known blocks for a domain.  If the argument is omitted,
the current domain is used.
}

add_to_help link {<objfile>} {
Incrementally link <objfile> into the interpreter.  The
object file must contain the definition for a single star or
target.

Code in the object file must not call any functions that are not
either in the object file itself or statically linked in to the
interpreter.  To get around this restriction, see "multilink" or
"permlink".
}

add_to_help multilink {<arg> ?<arg> ...?} {
Incrementally link arbitrary code into the compiler.  The arguments
are passed on to the linker, and may include .o files or linker switches
such as -l to specify libraries and -L to specify library search
directories.  Code linked in by this command can be replaced and cannot
be referred to by subsequent incremental links; see "permlink" to get
around this restriction.
}

add_to_help newstate {<statename> <class> <defvalue>} {
Add a state of the given class to the current galaxy.  This creates
a new state variable.  The class must be a type of state that is already
linked into the Ptolemy executable.
}

add_to_help node {<name>} {
Create a node named <name> for use in netlist-style connections.
}

add_to_help nodeconnect {<starname> <portname> <node>} {
Connect a porthole to a node.  If you connect one input and multiple
outputs to the same node, a Fork star will automatically be created to
connect the input to all the outputs.
}

add_to_help numports {<starname> <portname> <n>} {
portname must be a MultiPortHole.  Causes it to contain n PortHoles.  If
portname was named "input", the created portholes will be named "input#1",
"input#2", etc.  Since # is the comment character, you must quote
"input#1" to refer to it.
}

add_to_help permlink {<arg> ?<arg> ...?} {
Incrementally link arbitrary code into the compiler.  The arguments
are passed on to the linker, and may include .o files or linker switches
such as -l to specify libraries and -L to specify library search
directories.  Code linked in by this command can is permanent, and
subsequent incremental links can use it; no global symbol defined in
the linked-in code can be redefined.  See "multilink" for an alternative.
}

add_to_help print {?<block-or-classname>?} {
Print out a description of the named block, if an argument is given,
or of the current galaxy, if no argument is given.  The special argument
"target" will print the target description.
}

add_to_help reset {?<name>?} {
Replaces the named universe (or "main" if no name is given) by an empty
universe.  Any defgalaxy definitions you have made, or universes with
names other than "main", are still remembered.  Equivalent to
"newuniverse main".
}

add_to_help run {?<n>?} {
Generate schedule and run <n> times, or for <n> time units (may be
floating point for DE).  <n> is optional; default 1.
}

add_to_help schedule {} {
Generate and print the schedule.  Meaningful only for some domains.
}

add_to_help seed {?<n>?} {
Change the seed of the random number generation over the system to
<n>.  The default seed is 1.
}

add_to_help setstate {<block> <state> <value>} {
Change a state of a block within the current galaxy to the new value
<value>.  "setstate this <state> <value>" changes a state of the current
galaxy.
}

add_to_help source {<file>} {
Builtin Tcl command.  Read ptcl commands from <file>.
}

add_to_help star {<name> <class>} {
Add a star, or galaxy, of the given class to the current galaxy and name
it <name>.
}

add_to_help statevalue {<block> <state> ?current/initial?} {
Return the current or initial value of <state> within <block>.
Default is current value.
}

add_to_help target {?<newtarget>?} {
Display or change the current target.  If inside a defgalaxy and an
argument is given, the inside will use a separate target from the outside
and a wormhole will be constructed.
}

add_to_help targetparam {<name> ?<value>?} {
If <value> is given, set the value of target parameter <name> to 
<value>.  If <value> is not given, return the value of target 
parameter <name>.
}

add_to_help targets {?<domain>?} {
List targets that are usable for the domain (the current domain if the
argument is omitted.
}

add_to_help topblocks {?<block-or-classname>?} {
Return a list of top-level blocks in the current galaxy, if no argument
is given, or in the named block, if an argument is given.  Empty value
is returned for atomic blocks.
}

add_to_help wrapup {} {
Invoke the wrapup command on the universe.
}

add_to_help curuniverse {?<name>?} {
With no arguments, print the name of the current universe.  At startup the
current universe is an empty universe named "main".  With an argument,
switch the current universe to <name>, which must exist (or else an error
occurs).  The current domain will switch to that of <name>; each universe
on the list has its own notion of the current domain.
}

add_to_help newuniverse {?<name>? ?<dom>?} {
Create a new, empty universe named <name> with domain <dom>.  Both
arguments may be omitted, <dom> is the current domain by default and
<name> is "main" by default.  If <name> is the same as the name of
an existing universe, that universe is deleted and recreated empty.
}

add_to_help deluniverse {<name>} {
Deletes the universe named <name>.  If the argument is omitted, the
current universe is deleted and the current universe reverts to "main".
}

add_to_help univlist {} {
Returns a list of the names of all defined universes.
}

add_to_help renameuniv {?<oldname>? <newname>} {
With one argument, rename the current universe to <newname>.  With two
arguments, rename the universe named <oldname> to <newname>.  Any existing
universe named <newname> is deleted.
}

add_to_help schedtime {[actual]} {
Returns the current time from the schedule of the current top-level
universe.  If the target has a state named "schedulePeriod", the value
returned by calling "now" on the scheduler is divided by the
schedulePeriod value, unless the "actual" argument is given, in which case
the division is omitted.  This is done to cause the time to match the
number of iterations specified for the SDF, DDF, and BDF domains.
}

add_to_help listobjs {[states|ports|multiports] ?<block-or-classname>} {
Return a list of either the states, ports, or multiports in the named
block or class.  If the last argument is omitted, the current galaxy is
used.  The special name "target" may be used to list the states in the
target.
}

add_to_help cancelAction {<action_handle>} {
Cancel a pre- or post-action previously registered using the command
registerAction.  The argument must be the action_handle previously returned
by the registerAction command.
}

add_to_help registerAction {[pre or post] <tcl_command>} {
Register a tcl command to be executed before or after the firing of any star.
The function takes two arguments, the first of which must be "pre" or "post".
This argument indicates whether the action should occur before or after
the firing of a star.  The second argument is a string giving a tcl command.
Before this command is invoked, however, the name of the star that triggered
the action will be appended as an argument.  For example:
	registerAction pre puts
will result in the name of a star being printed on the standard output
before it is fired.  The value returned by registerAction is an
"action_handle", which must be used to cancel the action using cancelAction.
}

proc help {args} {
	case [llength $args] in {
		0 {set cmd help}
		1 {set cmd [lindex $args 0]}
		default {error "Too many arguments to 'help'"}
	}
	global argl_array
	global desc_array
	if { ($cmd == "?") } then {return [lsort [array names desc_array]]}
	if [info exists argl_array($cmd)] then {
		return "syntax: $cmd $argl_array($cmd) $desc_array($cmd)"
	} else {
		error "no help for $cmd"
	}
}


