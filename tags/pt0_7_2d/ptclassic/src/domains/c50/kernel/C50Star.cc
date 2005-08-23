static const char file_id[] = "C50Star.cc";

/*
Version:
@(#)C50Star.cc	1.4	05/26/98

@Copyright (c) 1994-1998 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

Programmer: Andreas Baensch, G. Arslan
Date of Creation: 25 May 1994

Modeled on code by T.M. Parks dated 5 January 1992.

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "C50Star.h"
#include "C50Target.h"
#include "StringList.h"

extern const char C50domainName[];


// Domain identification.
const char* C50Star::domain() const
{
    return C50domainName;
}

// State or PortHole reference.
StringList C50Star::expandRef(const char* name)
{
    return expandRef(name, "0");
}

// State or PortHole reference with offset.
StringList C50Star::expandRef(const char* name, const char* offset)
{
    StringList ref;
    ref << lookupMem(name) << ':' << lookupAddress(name, offset);
    return ref;
}


StringList C50Star::expandMacro(const char* func, const StringList& argList)
{
	StringList s,tmp;
	StringListIter arg(argList);
	const char* arg1 = arg++;
	const char* arg2 = arg++;



	if (matchMacro(func, argList, "addr2", 2)){
	  tmp = lookupAddress(arg1, arg2);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "addr", 2)){
	  tmp = lookupAddress(arg1, arg2);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "addr", 1)){
	  tmp = lookupAddress(arg1);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "ref2", 2)){ 
	  tmp = expandRef(arg1, arg2);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "ref", 2)){ 
	  tmp = expandRef(arg1, arg2);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "ref", 1)){
	  tmp = expandRef(arg1);
	  s << "addr" << tmp;
	}
	else if (matchMacro(func, argList, "valfix", 1)){
	  s << int(atof(expandVal(arg1))*(pow(2,15)-1));
	}
	else s = AsmStar::expandMacro(func, argList);

	return s;
}

// Class identification.
ISA_FUNC(C50Star,Star);
