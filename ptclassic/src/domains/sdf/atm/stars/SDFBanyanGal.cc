static const char file_id[] = "SDFBanyanGal.cc";
/*********************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer:  John C. Loh

This file dynamically creates a BanyanGalaxy.  The number of
inputs determines how many columns of BanyanStg's there will
be. 

**********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <math.h>
#include "VoiceData.h"
#include "SDFBanyanGal.h"
#include "GalIter.h"


#define STAR(i,j) (stars[(i)*(columns) + (j)]) 

const char *star_nm_SDFBanyanGal = "SDFBanyanGal";

const char* SDFBanyanGal :: className() const {return star_nm_SDFBanyanGal;}

Block* SDFBanyanGal :: makeNew() const { LOG_NEW; return new SDFBanyanGal; }

extern const char SDFdomainName[];

const char* SDFBanyanGal :: domain () const { return SDFdomainName; }



SDFBanyanGal::SDFBanyanGal () {
	stars = 0;
	addPort(left0.setPort("left0",this,MESSAGE));
	addPort(left1.setPort("left1",this,MESSAGE));
	addPort(right0.setPort("right0",this,MESSAGE));
	addPort(right1.setPort("right1",this,MESSAGE));
}



void SDFBanyanGal::setup() {

        int rows = int(left0.numberPorts());
        int columns = int((log10(rows*2.0))/(log10(2.0)));

	//Place the BanyanStg stars in the galaxy

	delete [] stars;
	stars = new SDFBanyanStg[rows*columns];

	//Do addBlock's
	//  Name and allocate in this order:
	//   --------------->
	//   --------------->
	//        .
	//        .
	//   --------------->
	//Also set the state called "stage" in BanyanStg instances

        for (int k=0; k<rows; k++) {
            char nameit[64];
            char nameitvalue[64];
            for (int j=0; j<columns; j++) {
                sprintf(nameit, "packetinstance#%d", (k*columns + j));
                sprintf(nameitvalue, "%d", columns-j);
                addBlock(stars[k*columns + j].setBlock(hashstring(nameit),this));
	        STAR(k,j).setState("stage", hashstring(nameitvalue));
            }
        }

	// Establish connections for MPH's, rather subtle, for each port
	// in an input or output MPH, find the "far" port and break the
	// connection, alias the MPH port to an internal star port, and
	// actually form the connection from the "far" port to the internal
	// star port

        int i = 0;
        PortHole *p, *q, *r, *s;
	PortHole *pfar, *qfar, *rfar, *sfar;

        MPHIter nextp(left0);
        MPHIter nextq(left1);
        MPHIter nextr(right0);
        MPHIter nexts(right1);

        while ((p=nextp++)!=0) {
              pfar = p->far();
              p->disconnect();
              // alias(*p,STAR(i,0).input1);
              // connect(*pfar,*p);
	      connect (*pfar,STAR(i,0).input1);
              i++;
        }

        i=0;
        while ((q=nextq++)!=0) {
              qfar = q->far();
              q->disconnect();
              // alias(*q,STAR(i,0).input2);
              // connect(*qfar,*q);
	      connect (*qfar,STAR(i,0).input2);
              i++;
        }

        i=0;
        while ((r=nextr++)!=0) {
              rfar = r->far();
              r->disconnect();
              // alias(*r,STAR(i,int(columns)-1).output1);
              // connect(*r,*rfar);
	      connect (STAR(i,int(columns)-1).output1,*rfar);
              i++;
        }

        i=0;
        while ((s=nexts++)!=0) {
              sfar = s->far();
              s->disconnect();
              // alias(*s,STAR(i,int(columns)-1).output2);
              // connect(*s,*sfar);
	      connect (STAR(i,int(columns)-1).output2, *sfar);
              i++;
        }


//Place connection code below:

//Divide into two cases.  Process the first half the of the rows
//using one routine, then use the mirror of the routine for the
//bottom half.  As far as the columns are concerned, the 
//connections are the same from column to column.  This is an
//advantage of using an OMEGA format.  

//Must note that the left-most inputs and right-most outputs
//are already connected to stars external to the BanyanGal galaxy.

//Do the connections for the top half.  Start from the
//top left and proceed down to (rows/2-1), then start
//at second column and proceed in the same way.

// l = columns, m = rows
// banyan  0-0,0-1,1-2,1-3
// banyan  2-0,2-1,3-2,3-3

	for (int l=0; l<(int(columns)-1); l++) {
	    for (int m=0; m<int(rows/2); m++) { 
	       connect(STAR(m,l).output1, STAR(2*m,l+1).input1);
	       connect(STAR(m,l).output2, STAR(2*m+1,l+1).input1);
	       connect(STAR(m+int(rows/2),l).output1, STAR(2*m,l+1).input2);
	       connect(STAR(m+int(rows/2),l).output2, STAR(2*m+1,l+1).input2);
    	}
}


}  // end setup



void
SDFBanyanGal::wrapup()

{
	PortHole  *p, *q, *qfar;

	MPHIter nextp(left0);
	MPHIter nextq(left1);
	MPHIter nextr(right0);
	MPHIter nexts(right1);

	int rows = int (left0.numberPorts());
	int columns = int ((log10(rows*2.0))/(log10(2.0)));

        int j = 0;
	while ((p=nextp++)!=0) {
	   // q = (PortHole*) (&(p->realPort()));
           q = (PortHole*) (&(STAR(j,0).input1));
	   qfar = q->far();
	   q->disconnect();
	   connect(*qfar,*p);
           j++;
        }

        j = 0;
	while ((p=nextq++)!=0) {
	   // q = (PortHole*) (&(p->realPort()));
           q = (PortHole*) (&(STAR(j,0).input2));
	   qfar = q->far();
	   q->disconnect();
	   connect(*qfar,*p);
           j++;
        }

        j = 0;
	while ((p=nextr++)!=0) {
	   // q = (PortHole*) (&(p->realPort()));
           q = (PortHole*) (&(STAR(j,int(columns)-1).output1));
	   qfar = q->far();
	   q->disconnect();
	   connect(*p, *qfar);
           j++;
        }

        j = 0;
	while ((p=nexts++)!=0) {
	   // q = (PortHole*) (&(p->realPort()));
           q = (PortHole*) (&(STAR(j,int(columns)-1).output2));
	   qfar = q->far();
	   q->disconnect();
	   connect(*p, *qfar);
           j++;
        }

	if (stars) {
	   for (int i = 0; i < rows*columns; i++) {
	       removeBlock(stars[i]);
           }
        }
}


 
SDFBanyanGal::~SDFBanyanGal() { 
    delete [] stars;
}

static SDFBanyanGal proto;
static RegisterBlock registerBlock(proto, "BanyanGal");
