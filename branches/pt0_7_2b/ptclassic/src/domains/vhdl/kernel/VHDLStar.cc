static const char file_id[] = "VHDLStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: Edward A. Lee, Michael C. Williamson

 This is the baseclass for stars that generate VHDL language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLStar.h"
#include "VHDLGeodesic.h"
#include "Tokenizer.h"
#include <ctype.h>

// The following is defined in VHDLDomain.cc -- this forces that module
// to be included if any VHDL stars are linked in.
extern const char VHDLdomainName[];

const char* VHDLStar :: domain () const { return VHDLdomainName; }

// isa

ISA_FUNC(VHDLStar,CGStar);

// Sanitize a string so that it is usable as a VHDL identifier.
const char* sanitize(const char* string)
{
    const int MAX_LENGTH = 64;
    static char clean[MAX_LENGTH];
    char* c = clean;
    int i = 0;

    // Check for leading digit.
    if (isdigit(*string))
    {
	*c++ = '_';
	i++;
    }

    // Replace strange charachters.
    while (++i < MAX_LENGTH && *string)
    {
	*c++ = isalnum(*string) ? *string : '_';
	string++;
    }
    *c = 0;

    return clean;
}

// Reference to State or PortHole.
StringList VHDLStar :: expandRef(const char* name)
{
    StringList ref;
    return ref;
}

// Reference to State or PortHole with offset.
StringList VHDLStar :: expandRef(const char* name, const char* offset)
{
    StringList ref;
    return ref;
}

void VHDLStar :: initialize() {
	CGStar::initialize();
}

int VHDLStar :: run() {
	int status = 0;
	status = runIt();
	return status;
}

int VHDLStar :: runIt() {
	StringList code = "\t{  -- star ";
	code << fullName() << " (class " << className() << ") \n";
	addCode(code);
	int status = CGStar::run();
	if (!status) return status;

	if (isItFork()) {
		addCode("\t}\n");
		return status;
	}

	return status;
}
