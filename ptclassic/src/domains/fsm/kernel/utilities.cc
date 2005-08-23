static const char file_id[] = "utilities.cc";

/**************************************************************************
Version identification:
@(#)utilities.cc	1.8 11/13/98

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:  Bilung Lee
 Date of creation: 9/26/96

 strParser parses a string using either double-quotes or curly-braces as
 the delimitter, and returns an array of parsed strings. 
 NOTE: User is responsible for deleting the memory allocated to those
 parsed strings by using "delete [] arrayName".

*************************************************************************/

#include <string.h>
#include "Error.h"
#include "Geodesic.h"
#include "InfString.h"
#include "PortHole.h"
#include "tcl.h"
#include "type.h"

// Translate the data from PortHole to Tcl Interp depending on ghostDoamin.
int port2interp(PortHole* port,Tcl_Interp* myInterp,const char* ghostDomain) {

  if (!strcmp(ghostDomain,"SDF")) {
    // Set value to "name(v)"
    InfString buf = port->name();
    buf << "(v)";
    InfString valueStr;
    Particle* p = port->geo()->get(); // Get Particle directly from Geodesic.
    valueStr = p->print();
    Tcl_SetVar(myInterp,buf,valueStr,TCL_GLOBAL_ONLY);

    // Recycle the Particle by putting it into Plasma.
    p->die();
    // Clear those Particles that is still in Geodesic.
    while (port->geo()->size()) {
      p = port->geo()->get();
      p->die();
    }

    // Set status to "name(s)" i.e set name(s) [expr $name(v) != 0]
    buf = "set ";
    buf << port->name();
    buf << "(s) [expr $";
    buf << port->name();
    buf << "(v)";
    buf << " != 0]";
    Tcl_Eval(myInterp,buf);

  } else if (!strcmp(ghostDomain,"DE") || !strcmp(ghostDomain,"FSM") ||
	     !strcmp(ghostDomain,"DDF")) {

    if (!port->geo()->size()) {
      // There is no token, i.e. the event is absent.
      // Then set status to 0 but keep previous value.
      InfString statusStr = "0";
      // Register into Tcl interp.
      InfString buf = port->name();
      buf << "(s)";
      Tcl_SetVar(myInterp,buf,statusStr,TCL_GLOBAL_ONLY);

    } else {
      // There exist tokens: First set status to 1.
      InfString statusStr = "1";
      // Register into Tcl interp.
      InfString buf = port->name();
      buf << "(s)";
      Tcl_SetVar(myInterp,buf,statusStr,TCL_GLOBAL_ONLY);

      // Then grab Particle directly from Geodesic, and set value.
      Particle* p = port->geo()->get();
      InfString valueStr = p->print();
      // Register into Tcl interp.
      buf = port->name();
      buf << "(v)";
      Tcl_SetVar(myInterp,buf,valueStr,TCL_GLOBAL_ONLY);

      // Recycle the Particle by putting it into Plasma.
      p->die();
      // Clear those Particles that is still in Geodesic.
      while (port->geo()->size()) {
	p = port->geo()->get();
	p->die();
      }
    }

  } else {
    InfString buf = "port2interp: ";
    buf << "Interaction with domain ";
    buf << ghostDomain;
    buf << " is not supported yet!";
    Error::abortRun(buf);
    return FALSE;
  }

  return TRUE;
}

// Translate the data from Tcl Interp to PortHole depending on ghostDoamin.
int interp2port(Tcl_Interp* myInterp,PortHole* port,const char* ghostDomain) {

  InfString buf = port->name();
  buf << "(s)";
  InfString statusStr = Tcl_GetVar(myInterp,buf,TCL_GLOBAL_ONLY);
  buf = port->name();
  buf << "(v)";
  InfString valueStr = Tcl_GetVar(myInterp,buf,TCL_GLOBAL_ONLY);

  int status;
  if (Tcl_GetInt(myInterp,statusStr,&status) != TCL_OK) {
    Error::abortRun("FSMScheduler: ","Tcl_GetInt for status failed: ",
		    myInterp->result);
    return FALSE;
  }

  if (status == 0) {
    // If the event is absent:

    if (!strcmp(ghostDomain,"SDF")) {
      // When the ghostDomain is SDF, need to pass a token with value 0.
      if (!strcmp(port->type(),"INT")) {
	if (port->isItOutput()) {
	  // FSM galaxy -> Outer domain (for FSMScheduler::sendData)
	  (*port)%0 << (int)0;
	  port->sendData();
	} else {
	  // FSM galaxy -> Inner domain of slave (for FSMStateStar::execSlave)
	  Particle* p = new IntParticle(0);
	  port->geo()->put(p);
	}

      } else if (!strcmp(port->type(),"FLOAT")) {
	if (port->isItOutput()) {
	  // FSM galaxy -> Outer domain (for FSMScheduler::sendData)
	  (*port)%0 << (double)0;
	  port->sendData();
	} else {
	  // FSM galaxy -> Inner domain of slave (for FSMStateStar::execSlave)
	  Particle* p = new FloatParticle(0);
	  port->geo()->put(p);
	}
	
      } else {
	Error::abortRun("interp2port: ",port->type()," is not supported yet!");
	return FALSE;
      }
    }  // end of if (!strcmp(ghostDomain,"SDF"))
    
    // For other domains, pass NOTHING. 

  } else {
    // If the event is present, set the token value
    if (!strcmp(port->type(),"INT")) {
      // First cast "valueStr" to integer string.
      buf = "int(";
      buf << valueStr;
      buf << ")";
      if (Tcl_ExprString(myInterp,buf) != TCL_OK) {
	Error::abortRun("FSMScheduler: ","Tcl_ExprString failed: ",
			myInterp->result) ;
	return FALSE;
      }
      // Then get the integer value.
      int value;
      if (Tcl_GetInt(myInterp,myInterp->result,&value) != TCL_OK) {
	Error::abortRun("FSMScheduler: ",
			"Tcl_GetInt for value failed: ",
			myInterp->result);
	return FALSE;
      }
      if (port->isItOutput()) {
	// FSM galaxy -> Outer domain (for FSMScheduler::sendData)
	(*port)%0 << value;
	port->sendData();
      }  else {
	// FSM galaxy -> Inner domain of slave (for FSMStateStar::execSlave)
	Particle* p = new IntParticle(value);
	port->geo()->put(p);
      }
      
    } else if (!strcmp(port->type(),"FLOAT")) {
      double value;
      if (Tcl_GetDouble(myInterp,valueStr,&value) != TCL_OK) {
	Error::abortRun("FSMScheduler: ",
			"Tcl_GetDouble for value failed: ",
			myInterp->result);
	return FALSE;
      } 
      if (port->isItOutput()) {
	// FSM galaxy -> Outer domain (for FSMScheduler::sendData)
	(*port)%0 << value;
	port->sendData();
      } else {
	// FSM galaxy -> Inner domain of slave (for FSMStateStar::execSlave)
	Particle* p = new FloatParticle(value);
	port->geo()->put(p);
      }
      
    } else {
      Error::abortRun("interp2port: Data type ", port->type(),
		      " is not supported yet!");
      return FALSE;
    }
    
  } // end of else for if (status == 0)
    
  return TRUE;
}


// Parse a string using either double-quotes or curly-braces as the delimitter.
char** strParser(const char* string,int& numStr,const char* type) {
    char** parsedStr = 0;
    int start = 0;
    int end = 0;
    int i;

    if (strcmp(type, "double-quote") == 0) {
      int numQuote = 0;
      while (string[start] != '\0') {
	if (string[start] == '\"') numQuote++;
	start++;
      }
      if (numQuote%2 != 0) {
	Error::abortRun("Cannot parse the string: Unmatched double-quote. ",
			"Original string: ", string);
	return 0;	      
      }
      numStr = numQuote/2;

    }
    else if (strcmp(type, "curly-brace") == 0) {
      numStr = 0;
      while (string[start] != '\0') {
	if (string[start] == '{') numStr++;
	start++;
      }
    }
    else {
	Error::abortRun("Cannot parse the string: Unrecognized delimitter ",
			"type for strParser. Original string: ", string);
	return 0;
    }
    parsedStr = new char *[numStr];

    start = 0;
    end = 0;
    numStr = 0;
    while (1) {
      // Skip all the prefix spaces.
      while (string[start] == ' ') start++;
      
      if (string[start] == '\0') break ; // Hit end of string.
      
      if (strcmp(type, "double-quote") == 0) {
	// Find the left-side quote.
	if (string[start] != '\"') {
	  Error::abortRun("Cannot parse the string: Each string must be ",
	   "surrounded by a pair of double-quotes. Original string: ", string);
	  return 0;
	}
	start++;
	end = start;

 	// Find the right-side quote.
	while (string[end] != '\"') end++;
      }
      else if (strcmp(type, "curly-brace") == 0) {
	// Find the left-side curly-brace.
	if (string[start] != '{') {
	  Error::abortRun("Cannot parse the string: Each string must be ",
	    "surrounded by a pair of curly-braces. Original string: ", string);
	  return 0;
	}
	start++;
	end = start;

       	// Find the right-side curly-brace.
	while (string[end] != '}' && string[end] != '\0' ) end++;

	if (string[end] == '\0') {
	  Error::abortRun("Cannot parse the string: Missing right-side ",
			  "curly-brace. Original string: ", string);
	  return 0;
	}
      }
	
      parsedStr[numStr] = (char *) new char[end-start+1];
      for (i = 0; i < end-start; i++) {
	parsedStr[numStr][i] = string[start+i];
      }
      parsedStr[numStr][i] = '\0'; 

      numStr++;

      start = end+1;
    }
    return parsedStr;
}
