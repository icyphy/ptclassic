#ifndef _Scope_h
#define _Scope_h 1

#ifdef __GNUG__
#pragma interface
#endif

/**************************************************************************
Version identification:
@(#)Scope.h	1.3	6/21/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  Jose Luis Pino
 Date of creation: 10/17/95

**************************************************************************/

#include "Galaxy.h"
#include "DynamicGalaxy.h"
#include "StringList.h"

class Scope:private Galaxy {
public:
    // the main routine - to set up state and name scoping on a galaxy
    // hierarchy, call the childScope method.  This method will create
    // a scoping hierarchy if one does not exist.  This new hierarchy
    // will be automatically deleted after the corresponding galaxy is
    // deleted
    static Scope* createScope(Galaxy&);
    
    // remove a block that is using this scoping hierarchy, this
    // should only be called in Block::~Block
    void remove(Block& b);

    // return the name of just this galaxy
    Galaxy::name;

    // lookup a state using the scoping hierarchy
    State* lookup(const char*);

    // return the fullName using the scoping hierarchy
    /*virtual*/ StringList fullName() const;
        
protected:
    Scope(Galaxy&);
    ~Scope();
    
    // return the parent of this scope
    Scope* parentScope() const { return prntScope; }

    // set the parentScope
    void setParentScope(Scope* scope) { prntScope = scope; }

private:
    // Parent scope
    Scope* prntScope;

    void removeChild(Scope&);
    void optionalDestructor();
    BlockList childScopes;
};

#endif
