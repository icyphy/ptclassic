static const char file_id[] = "DEDynForkBase.pl";
// .cc file generated from DEDynForkBase.pl by ptlang
/*
copyright (c) 1997-1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynForkBase.h"

const char *star_nm_DEDynForkBase = "DEDynForkBase";

const char* DEDynForkBase :: className() const {return star_nm_DEDynForkBase;}

ISA_FUNC(DEDynForkBase,DEStar);

DEDynForkBase::DEDynForkBase ()
{
	setDescriptor("This star is the base class for a group of stars which implement\na dynamic fork function.");


}

// DEDynForkBase is an abstract class: no KnownBlock entry
