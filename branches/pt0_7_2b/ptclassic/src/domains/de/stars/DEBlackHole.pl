ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer: S. Ha
Date of creation: 8/28/90

 A BlackHole accepts input Particles, but doesnt do anything with
 them -- typically used to connect up unwanted signals on other blocks

************************************************************************/
}
defstar {
	name {BlackHole}
	domain {DE}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
}
