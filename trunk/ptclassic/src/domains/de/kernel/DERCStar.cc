static const char file_id[] = "DERCStar.cc";
/******************************************************************** 
Version identification: $Id$
 
Author: Mudit Goel
        Neil Smyth

Copyright (c) 1997- The Regents of the University of California.
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
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "DERCStar.h"

const char *star_nm_DERCStar = "DERCStar";
const char* DERCStar :: className() const {return star_nm_DERCStar;}
ISA_FUNC(DERCStar,DERepeatStar);
Block* DERCStar :: makeNew() const { LOG_NEW; return new DERCStar;}

FILE * DERCStar::fpoverflow;
FILE * DERCStar::fpfiring;

DERCStar::DERCStar ()
{
    setDescriptor("Common Polis(resource contention) Software Scheduler");
    isRCStar = 1;  // override to true (false in DEStar)
}


double DERCStar::getDelay ()
{
    return 0;
}


SequentialList* DERCStar::getEvents ()
{
    return emittedEvents;
}


FILE* DERCStar::Openoverflow ( char *name )
{
    if ( DERCStar::fpoverflow == NULL ) {
        DERCStar::fpoverflow = fopen( name, "w" );
        if ( DERCStar::fpoverflow ) {
            fprintf( DERCStar::fpoverflow, "Overflow report\n\n" );
            fflush( DERCStar::fpoverflow );
        }
    }
    return( fpoverflow );
}


void DERCStar::Printoverflow ( char *st )
{
    if ( DERCStar::fpoverflow ) {
        fprintf( DERCStar::fpoverflow, st );
        fflush( DERCStar::fpoverflow );
    }
}


FILE * DERCStar::Openfiring ( char *name )
{
    if ( DERCStar::fpfiring == NULL ) {
        DERCStar::fpfiring = fopen( name, "w" );
        if ( DERCStar::fpfiring ) {
            fprintf( DERCStar::fpfiring, "Firing report\n\n" );
            fflush( DERCStar::fpfiring );
        }
    }
    return( fpfiring );
}


void DERCStar::Printfiring ( char *st )
{
    if ( DERCStar::fpfiring ) {
        fprintf( DERCStar::fpfiring, st );
        fflush( DERCStar::fpfiring );
    }
}


void DERCStar::Closeflow ()
{
    if ( DERCStar::fpoverflow ) {
        fclose( DERCStar::fpoverflow );
        DERCStar::fpoverflow = NULL;
    }
    if ( DERCStar::fpfiring ) {
        fclose( DERCStar::fpfiring );
        DERCStar::fpfiring = NULL;
    }
}

/*void DERCStar::emitEventToIntQ( int outputPort, double delay ) {
    double emitTime; 
    StarLLCell* temp = (StarLLCell*)storeList->getAndRemove();
    if (temp == 0) temp = new StarLLCell(-1, -1);
    if (needsSharedResource) { 
        emitTime = now + (delay/clkFreq);
    } else { 
        emitTime = now + (1/clkFreq); }
    temp->time = emitTime;
    temp->outputPort = outputPort;
    emittedEvents->append(temp);
}*/



static DERCStar proto;
static RegisterBlock registerBlock(proto,"RCStar");



StarLLCell::StarLLCell(double tim, int number) {
    time = tim;
    outputPort= number;
}




