ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: J. T. Buck
Date of creation: 9/26/90

 A BlackHole accepts input Particles, but doesnt do anything with
 them -- typically used to connect up unwanted signals on other blocks

************************************************************************/
}
defstar {
	name {BlackHole}
	domain {SDF}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
