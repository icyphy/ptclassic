/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
		       
 Programmer:  Alan Kamas
 Date of creation: July 1993

This file implements a class that adds Ptolemy-specific Oct/Vem commands to
a Tcl interpreter.  

**************************************************************************/
static const char file_id[] = "POct.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "POct.h"

#include "SimControl.h"

#include <stdio.h>
#include <string.h>

extern "C" {
#define Pointer screwed_Pointer
#include "oct.h"  /* Oct Pointer Structure */
#include <pwd.h> /* Used for Make Star */
#include "pigidefine.h" /* Constants used by pigi functions */
#include "octIfc.h" 
#include "local.h"
#include "misc.h"
#include "paramStructs.h"
#include "palette.h" /* used by MkSchemIcon */
#include "util.h"
#include "exec.h"
#include "err.h"
#include "ganttIfc.h"
#include "icon.h"
#include "compile.h"
#include "octMacros.h"    // For GetOrCreatePropStr
//#include "kernelCalls.h"
void win_msg(const char*);  // for error dialog box.  FIXME: formalize this
/* Declare the functions in kernelCalls used by ptkGetTargetNames */
extern int KcDomainTargets(const char* domain, const char** names, int nMax);
extern const char*KcDefTarget(char *domain);
/* Declare the functions in kernelCalls used by ptkGetDomainNames */
extern int numberOfDomains();
extern const char* nthDomainName(int n);
/* Declare the functions in kernelCalls used by ptkSetSeed */
extern void KcEditSeed(int n);
/* Declare the functions in kernelCalls used by ptkSetRunUniverse */
extern boolean KcSetKBDomain(const char* domain);
extern void KcSetEventLoop(int on);
#undef Pointer
}
#include "miscFuncs.h"
#include "StringList.h"

// FIXME: seems to need to be here for the error functions.  
//        Will want to costomize error handeling for Tk - aok
#include "Error.h"

// FIXME: This include is only needed for the "quote" macro
//        Seems silly to include so much extra baggage - aok
#include "isa.h"


/////////////////////////////////////////////////////////////////
// "C" callable functions.  These functions are used to convert
// oct objects to strings and back again.  This is because TCL
// functions can only be passed strings.

// Converts an oct Facet Pointer into a string "handle" that
// can be used by a TCL interpreter
// To use this function, you must declare space for a char array that
// is at least 15 chars long.
extern "C" void ptkOctObj2Handle( octObject *objPtr, char *stringValue )
{
        sprintf( stringValue, "OctObj%-.8lx", (long)objPtr->objectId);
}

// Converts a string "handle" into an oct Facet Pointer
// To use this function, you must declare space for a char array that
// is at least 15 chars long.
extern "C" int ptkHandle2OctObj( char *stringValue, octObject *objPtr )
{
        objPtr->objectId = OCT_NULL_ID;
        sscanf( stringValue, "OctObj%lx", &objPtr->objectId);
        if (objPtr->objectId == OCT_NULL_ID) {
           return 0;
        } else {
           if (octGetById(objPtr) == OCT_NOT_FOUND) return 0;
        }
        return 1;
}


// we want to be able to map Tcl_interp pointers to POct objects.
// this is done with a table storing all the POct objects.
// for now, we allow up to MAX_POct POct objects at a time.
const int MAX_POct = 10;

static POct* ptable[MAX_POct];

/////////////////////////////////////////////////////////////////////

// this is a static function.
POct* POct::findPOct(Tcl_Interp* arg) {
        for (int i = 0; i < MAX_POct; i++)
                if (ptable[i]->interp == arg) return ptable[i];
        return 0;
}

void POct::makeEntry() {
        int i = 0;
        while (ptable[i] != 0 && i < MAX_POct) i++;
        if (i >= MAX_POct) return;
        ptable[i] = this;
}

void POct::removeEntry() {
        for (int i = 0; i < MAX_POct; i++) {
                if (ptable[i] == this) {
                        ptable[i] = 0;
                }
        }
}

/////////////////////////////////////////////////////////////////////


// constructor
POct::POct(Tcl_Interp* i)
{
        interp = i;
	// FIXME:  check for validity of interpreter here
	//         may want to create interp if pointer invalid
        //         if so, set myInterp True
        myInterp = FALSE;
        makeEntry();
	registerFuncs();

	// Initialize the states that remind Users of previous inputs
	// for ptkSetSeed
        OldRandomSeed = 1;   
	// for ptkSetMkStar
	MkStarName = "";
	// g++-2.6.1 requires the (char *) in the next line
	MkStarDomain = (char *)DEFAULT_DOMAIN;
	MkStarDir = "NIL";
	MkStarPalette = "./user.pal";
        // FIXME:  Add following line back for MkSchemIcon state
	// MkSchemPalette = "./user.pal";
}

// destructor
POct::~POct() {
        removeEntry();
        if (myInterp) {
                Tcl_DeleteInterp(interp);
                interp = 0;
        }
}

/////////////////////////////////////////////////////////////////////
// ------ Helper functions, not used directly as TCL commands -----

// Sets Bus Parameters in the Oct data base
// Works by first getting a prop from the oct data base, then modifying
// it and finally sending it back.
// FIXME: Is there a way to modify and send it to the base without doing
//        the get first?
int POct::SetBusParams( octObject *instPtr, ParamListType *pList) {
    octObject prop;

    // Set up the Prop
    // This step may not be necessary (see above).
    GetOrInitBusProp(instPtr, &prop);

    // Fill in the props new value
    prop.contents.prop.type = OCT_STRING;

    // The line below will not compile under cfront 1.0
    //((const char*)(prop.contents.prop.value.string)) = pList->array->value;
    prop.contents.prop.value.string = (char *)pList->array->value;

    // Save this new bus value;
    IntizeProp(&prop);
    (void) octModify(&prop);
    return(TRUE);
}

// Sets Delay Parameters in the Oct data base
// Works by first getting a prop from the oct data base, then modifying
// it and finally sending it back.
// FIXME: Is there a way to modify and send it to the base without doing
//        the get first?
int POct::SetDelayParams( octObject *instPtr, ParamListType *pList) {
    octObject prop;

    // Set up the Prop
    // This step may not be necessary (see above).
    GetOrInitDelayProp(instPtr, &prop);

    // Fill in the props new value
    prop.contents.prop.type = OCT_STRING;
    // The line below will not compile under cfront1.0
    //((const char*)(prop.contents.prop.value.string)) = pList->array->value;
    prop.contents.prop.value.string = (char *)pList->array->value;

    // Save this new delay value;
    (void) octModify(&prop);
    return(TRUE);

}

// Deletes all of the elements of the passed pList
void POct::DeletePList( ParamListType* pList) {
    for (int i=0; i < pList->length; i++) {
	
	// FIXME: a consistent decision should be made about
	// whether a ParamListType owns its strings or not,
	// and if so they should not be "const char *".
	// Note that it is not legal C++ to delete a "const T *"
	// pointer, hence the casts.

        delete (char*)(pList->array[i].name);
        pList->array[i].name = NULL;
        delete (char*)(pList->array[i].type);
        pList->array[i].type = NULL;
        delete (char*)(pList->array[i].value);
        pList->array[i].value = NULL;
    }
    delete pList->array;
    pList->array = NULL;
}


// Converts a parameter list string of the form:
//  {name1 type1 value1} {name2 type2 value2} ...
// into the pList format.  Allocates space for the pList
// elements as well.  Note that all of the elements must
// be freed once the pList is no longer needed.  DeletePList
// does this.
int POct::MakePList( char* parameterList, ParamListType* pList) {
    int aC, Element_aC;
    char **aV, **Element_aV;
    int i;
    ParamType *pElement;
    int ErrorFound;

    if (strcmp(parameterList,"NIL")==0) {
        // Create a "NIL" pList with 0 elements
        pList->length = 0;
        pList->array = NULL;
        return 1;
    }
       
    if (Tcl_SplitList( interp, parameterList, &aC, &aV ) != TCL_OK){
	Error::error("Cannot parse parameter list: ", parameterList); 
	return 0;
    }

    pList->length = aC;
    pList->array = new ParamType[aC];

    ErrorFound = 0;
    for (i=0; i< pList->length; i++) {

        pElement = &pList->array[i];

        if (Tcl_SplitList(interp,aV[i], &Element_aC, &Element_aV)!=TCL_OK){
            Error::error("Error in parsing parameter list element: ", aV[i]);
            // Keep going, and then clear it all out of memory when done.
            ErrorFound = 1;
        }

        if (Element_aC != 3 ){
            Error::error("Parameter list element ", aV[i], 
                         " does not have 3 elements.");
           ErrorFound = 1;
           pElement->name = NULL;
           pElement->type = NULL;
           pElement->value = NULL;
        } else {
           pElement->name = savestring(Element_aV[0]);
           pElement->type = savestring(Element_aV[1]);
           pElement->value = savestring(Element_aV[2]);
        }
        // Free the memory used by Element_aV as it is no longer needed
        free ((char *) Element_aV);
    }

    // Free the memory used by aV as it is no longer needed
    free((char *) aV);

    if (ErrorFound == 1) {
        // An error was encountered while parsing the list
        DeletePList( pList);
        return 0;
    } else {
        return 1;
    }
}


// ------- TCL Commands --------------------------------------------

// Calls the compile.c code.  May want to update it so that
// this code uses the PTcl function calls.
// FIXME:  If a non-universe facet is passed to this function,
//         it churns ahead anyway and then crashes.  - aok
//         fix it by added an IsUniv check.
int POct::ptkCompile (int aC,char** aV) {
    octObject facet;

    if (aC != 2) return usage ("ptkCompile <OctObjectHandle>");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
	Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0], 
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (CompileFacet(&facet)){
	return TCL_OK;
    } else {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

}

// Returns a list of lists of the parameters of a facet (if any).
// Note that "parameters" refers to the values held in the oct data base
//    and used to initialize a ptolemy run, while "states" refer to the
//    values held in memory during an ptolemy session.  Thus, the parameters
//    from a facet may be used to set the initial states of a star during
//    the compile command.  In theory, parameters could also have other uses.
// The command should be called as follows:
// command_name FacetHandle InstanceHandle
// The command returns a two element list in the following format:
// {title} { parameter_list }
// where parameter list is of the form:
//    {name1 type1 value1} {name2 type2 value2} ...
//
int POct::ptkGetParams (int aC,char** aV) {
    octObject facet,instance;
    octObject property;
    ParamType *place;
    ParamListType pList;
    char title [64], size[5], tempStr[64];
    int i;

//  FIXME:  The current pList and the strings it points to are
//  allocated in subfunctions, but never freed.  This needs to
//  be systematically fixed in pigilib, and KernelCalls. - aok

    if (aC != 3) {
        return usage ("ptkGetParams <OctFacetHandle> <OctInstanceHandle>");
    }

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Check to see if a valid instance was passed
    if (strcmp(aV[2],"NIL")==0) {
        // If there is no instance, then this must be a Galaxy or Universe
        if (IsGalFacet(&facet) || IsUnivFacet(&facet)) {
            if (!GetFormalParams(&facet, &pList)) {
		Tcl_AppendResult( interp, "Error getting Formal parameters. ",
		                  ErrGet(), (char *)NULL );
                return TCL_ERROR;
            }
            sprintf(title, "Edit Formal Parameters");
            // Result string will be built below
        } else {
	    win_msg ("Not a Star, Galaxy, or Universe");
            return TCL_OK;
        }
    } else {
        // There was an instance passed
        if (ptkHandle2OctObj(aV[2], &instance) == 0) {
            Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", 
		aV[0], (char *) NULL);
 	    return TCL_ERROR;
	}
    
	if ( IsDelay(&instance) || IsBus(&instance) || IsDelay2(&instance)) {
	    // Parameters are stored differently for delays and buses.
	    // can't use the "plist" form as for Stars, Gals, and Formals
	    if (IsDelay(&instance) ) {
		GetOrInitDelayProp(&instance, &property);
		sprintf(title, "Edit Delay");
	    } else if (IsDelay2(&instance)) {
		GetOrInitDelayProp(&instance, &property);
		sprintf(title, "Edit Delay With Initial Values");
	    } else {
		GetOrInitBusProp(&instance, &property);
		sprintf(title, "Edit Bus");
	    }
            // Convert "property" into Result string
            Tcl_AppendElement (interp, title);
            Tcl_AppendResult(interp, " { {", NULL);
            Tcl_AppendElement(interp, property.contents.prop.name);
            switch(property.contents.prop.type) {
              case OCT_INTEGER:
                sprintf( tempStr, "INTEGER" );
                Tcl_AppendElement(interp, tempStr);
                sprintf(size, "%ld", property.contents.prop.value.integer);
                Tcl_AppendElement(interp, size);
                break;
              case OCT_STRING:
                sprintf( tempStr, "STRING" );
                Tcl_AppendElement(interp, tempStr);
                Tcl_AppendElement(interp, property.contents.prop.value.string);
                break;
              default:
                Error::error("type unknown in editing delay/bus width");
                return TCL_ERROR;
            }
            Tcl_AppendResult(interp, "} }", NULL);
            return TCL_OK;
  	} else if (IsGal(&instance) || IsStar(&instance)) {
	    // Must be a star or Galaxy
	    // Set the domain to be that of the instance
	    if (!setCurDomainInst(&instance)) {
                Tcl_AppendResult(interp,"Invalid Domain Found.",(char *) NULL);
                return TCL_ERROR;
            }
	    if (!GetOrInitSogParams(&instance, &pList)) {
		Tcl_AppendResult(interp, 
                                 "Error Getting Star or Galaxy parameters.  ", 
                                  ErrGet(), (char *) NULL);
                return TCL_ERROR;
            }
            sprintf(title, "Edit Actual Parameters");
            // Build results string below.
        } else {
	    win_msg("Not a star, galaxy, bus, or delay instance");
            return TCL_OK;
        }
    }

    // Convert pList into a TCL list of Strings to return as result
    Tcl_AppendElement (interp,  title);
    if (pList.length > 0) {
        // There are some parameters here
        place = pList.array;
        Tcl_AppendResult(interp, " { ", NULL);
        for (i = 0; i < pList.length; i++) {
            Tcl_AppendResult(interp, " {", NULL);
            Tcl_AppendElement(interp, (char*)place->name);
            Tcl_AppendElement(interp, (char*)place->type);
            Tcl_AppendElement(interp, (char*)place->value);
            Tcl_AppendResult(interp, "}", NULL);
            place++;
        }
        Tcl_AppendResult(interp, " }", NULL);
    } else {
	// There are no parameters
	Tcl_AppendResult(interp, " NIL", NULL );
    }
    return TCL_OK;
}

// Given a Facet_Handle, Instance_Handle, and parameter_list,
// saves the values of the parameter_list into the Oct data base
// at the position specified by the Facet and Instance Handles.
//
// The command should be called as follows:
// command_name FacetHandle InstanceHandle { parameter_list }
// where parameter_list is of the form:
//    {name1 type1 value1} {name2 type2 value2} ...
// No return string.
//
int POct::ptkSetParams (int aC,char** aV) {
    ParamListType pList;
    int ErrorFound = 0;
    octObject facet, instance;

    if (aC != 4) return usage (
          "ptkSetParams <OctFacetHandle> <OctInstnceHandle> <Parameter_List>");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Covert the parameter List string in the pList structure.
    if (MakePList(aV[3], &pList) == 0) {
        Tcl_AppendResult(interp, "Unable to parse parameter list: ",
                         aV[3], (char *) NULL);
        return TCL_ERROR;
    }

    // Now that the pList has been made, it should be freed before
    // returning.

    // Check to see if a valid instance was passed
    if (strcmp(aV[2],"NIL")==0) {
        // If there is no instance, then this must be a Galaxy or Universe
        if (IsGalFacet(&facet) || IsUnivFacet(&facet)) {
            // Set Formal Parameters
            if (SetFormalParams(&facet, &pList) == 0) {
                ErrorFound = 1;
                Tcl_AppendResult(interp, aV[0],
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }
        } else {
            ErrorFound = 1;
            Tcl_AppendResult(interp,
                             "Not a Star, Galaxy, Universe.",
                             (char *) NULL);
        }
    } else {
        // There was a valid instance passed
        if (ptkHandle2OctObj(aV[2], &instance) == 0) {
            // No way to go on if the handle is bad.  Clean up and exit.
            DeletePList(&pList);
            Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", 
		aV[0], (char *) NULL);
            return TCL_ERROR;
        }

        if ( IsDelay(&instance) || IsDelay2(&instance)) {
            // Set Delay Parameters from the pList
            if (SetDelayParams(&instance,&pList) == 0) {
                ErrorFound = 1;
		Tcl_AppendResult(interp, aV[0], 
				 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }
        } else if (IsBus(&instance)) {
            // Set Bus Parameters from the pList
            if (SetBusParams(&instance, &pList) == 0) {
                ErrorFound = 1;
		Tcl_AppendResult(interp, aV[0], 
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }

        } else if (IsGal(&instance) || IsStar(&instance)) {
            // Must be a star or Galaxy
	    // Set the domain to be that of the instance
	    if (!setCurDomainInst(&instance)) {
                Tcl_AppendResult(interp,"Invalid Domain Found.",(char *) NULL);
                return TCL_ERROR;
            }
            // Set Star or Galaxy params from the pList
            if (SetSogParams(&instance, &pList) == 0) {
                ErrorFound = 1;
		Tcl_AppendResult(interp, aV[0],
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }

        } else {
            ErrorFound = 1;
            Tcl_AppendResult(interp, 
                             "Not a star, galaxy, bus, or delay instance.",
                             (char *) NULL);
        }
    }

    DeletePList (&pList);

    if (ErrorFound == 1) return TCL_ERROR;
    else return TCL_OK;

}

// ptkSetFindName <facet-id> <Name> 
//
// Does the Find Name Command given the passed name
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
//
int POct::ptkSetFindName (int aC,char** aV) {
    octObject facet;
    char* name;

    if (aC != 3) return 
        usage ("ptkSetFindName <OctObjectHandle> <Name>");

    name= aV[2];

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Perform the Find Name function:
    if (!FindNameSet(&facet, name)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    return TCL_OK;
}

// ptkSetRunUniverse <facet-id> <ParameterList> 
//
// Saves the passed Parameter List and then runs the facet
//
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
//
int POct::ptkSetRunUniverse (int aC,char** aV) {

    octObject facet;

    if (aC != 3) return 
        usage ("ptkSetRunUniverse <OctObjectHandle> <ParameterList>");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Redo facet checks to make sure this facet is still okay
    // before proceeding
    if (!IsUnivFacet(&facet)){
        Tcl_AppendResult(interp, "Schematic is not a universe.",
                         (char *) NULL);
        return TCL_ERROR;
    }
    if (! KcSetKBDomain((const char *)DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, "Failed to set default domain.",
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Save the new parameters into the facet
    if ( Tcl_VarEval(interp,"ptkSetParams ",
                   aV[1],
                   " NIL ",
                   " \"[list ", aV[2], " ]\" ",
                   (char *)NULL) != TCL_OK ) {
        return TCL_ERROR; 
    }
     
    // Run the Facet
    if (!ptkRun( &facet, TRUE )) return TCL_ERROR; 
       
    return TCL_OK; 
}

// ptkGetStringProp object-handle property-name
// returns the value of the string property for the 
// passed facet or instance.
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
// modified by Edward Lee and Douglas Niehaus 8/94
// to generalize for string properties rather than 
// just comments
int POct::ptkGetStringProp (int aC,char** aV) {
    octObject facet;
    char *value;

    // always takes arguments: object and property name
    //
    if (aC != 3) {
      return usage ("ptkGetStringProp <OctObjectHandle> propName");
    }

    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (!GetStringProp(&facet, aV[2], &value)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Make sure that the value is defined.
    if (value == NULL) {
        Tcl_AppendResult(interp, "", (char *) NULL);
    } else {
        // return the value as a list element to preserve whitespace
        Tcl_AppendResult(interp, value, (char *) NULL);
    }

    return TCL_OK;
}

// ptkSetStringProp facet-id propName value 
//
// saves the value of the named property into the 
// passed facet/instance
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
// modified by Edward Lee and Douglas Niehaus 8/94
// to generalize for string properties rather than 
// just comments
//
int POct::ptkSetStringProp (int aC,char** aV) {
    octObject facet;
    char* value;

    if (aC != 4) return 
        usage ("ptkSetStringProp <OctObjectHandle> <propName> <value>");

    value = aV[3];

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the value into the passed facet
    if (!SetStringProp(&facet, aV[2], value)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    return TCL_OK;

}

// ptkGetMkSchemIcon
// returns the past value of the MkSchemIcon2 Command
//
// Written by Alan Kamas  1/94
//
int POct::ptkGetMkSchemIcon (int aC, char** /*aV*/)
{
    if (aC != 1) return  usage ("ptkGetMkSchemIcon");

    // FIXME:  Add following line back for MkSchemIcon state
    // Tcl_AppendResult(interp, (char *)MkSchemPalette, (char *) NULL);
    // FIXME:  Delete following line for MkSchemIcon state
    Tcl_AppendResult(interp, "./user.pal", (char *) NULL);

    return TCL_OK;
}

// ptkSetMkSchemIcon <facet-id> <Palette>
//
// Makes a Scematic Icon of the passed facet and stores
// the new icon into the passed palette directory.
//
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
//
int POct::ptkSetMkSchemIcon (int aC,char** aV) {
    octObject facet;
    char* palette;
    char buf[512];

    if (aC != 3) return 
        usage ("ptkSetMkSchemIcon <OctObjectHandle> <Palette>");
   
    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }
    // FIXME:  Add following line back for MkSchemIcon state
    // MkSchemPalette = aV[2];
    palette = aV[2];

    if (!GetTildePath(&facet, buf)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }
    DirName(buf);
    if (IsGalFacet(&facet)) {
        if (!MkGalIconInPal(&facet, buf, palette)) {
            Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
            return TCL_ERROR;
        }
    } else if (IsPalFacet(&facet)) {
        if (!MkPalIconInPal(&facet, buf, palette)) {
            Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
            return TCL_ERROR;
        }
    } else {
        if (!MkUnivIconInPal(&facet, buf, palette)) {
            Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
            return TCL_ERROR;
        }
    }

    return TCL_OK;
}

// ptkGetMkStar
// returns the past values of the MkStar Command
//
// Return format is as follows:
// {"Star Name" StarNameValue} {Domain DomainValue} ...
// Written by Alan Kamas  1/94
// based on code by Edwin Goei
// 
int POct::ptkGetMkStar (int aC, char** /*aV*/)
{

    char buf[64];
    struct passwd *pwent;

    if (aC != 1) return  usage ("ptkGetMkStar");
    
    // Initialize star src directory to the home directory of the user
    const char* directory = MkStarDir;
    if (strcmp( directory, "NIL" ) == 0) {
        if ((pwent = getpwuid(getuid())) == NULL) {
            Tcl_AppendResult(interp, "Cannot get password entry",
                         (char *) NULL);
            return TCL_ERROR;
        }
        sprintf(buf, "~%s/", pwent->pw_name);
        MkStarDir = buf;
    }

    // Note: if any of the titles (ie "Star name") change, that
    //       change must be reflected in ptkSetMkStar
    Tcl_AppendResult(interp, 
                     "{{Star name} {", (const char *)MkStarName, "}} ",
                     "{{Domain} {", (const char *)MkStarDomain, "}} ",
                     "{{Star src directory} {", (const char *)MkStarDir, "}} ", 
                     "{{Pathname of Palette} {", (const char *)MkStarPalette, "}}", 
                     (char *) NULL);
    return TCL_OK;
}

// ptkSetMkStar <StarName> <Domain> <SrcDir> <PaleteDir>
//
// Makes a new star 
// This procedure was written to work with ptkEditStrings
// Each "List" entry is of the form "Title Value" as in "Domain SDF"
//    The titles are used to determine which entry is which.
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
//
int POct::ptkSetMkStar (int aC,char** aV) {

    if (aC != 5) return 
        usage ("ptkSetMkStar <StarName> <Domain> <SrcDir> <PaleteDir>");
    
    // Store these values for the next time MkStar is called
    MkStarName = aV[1];
    MkStarDomain = aV[2];
    MkStarDir = aV[3];
    MkStarPalette = aV[4];

    /* Note that only need to test if MkStarDir starts with '~'
       so that the user may enter something like "~/work/stars" where
       '~' stands for user's own home directory.  -wtc
       allow $PTOLEMY (kennard, Oct92)
    */

    if (aV[3][0] != '~' && aV[3][0] != '$') {
        Tcl_AppendResult(interp, 
                 "Star src directory must begin with '~user' or '$variable'",
                         (char *) NULL);
           return TCL_ERROR;
    }

    if (!MkStar( aV[1], aV[2], aV[3], aV[4] ) ){
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    return TCL_OK;
}


// ptkGetSeed 
// returns the Past value of the Random Number Seed
//
// Written by Alan Kamas  1/94
// 
// FIXME: Ideally, GetSeed and SetSeed should be PTcl funtions
//        There is already a "seed" command there.
int POct::ptkGetSeed (int aC,char** /*aV*/) {

    StringList buf;

    if (aC != 1) return  
            usage ("ptkGetSeed");
    
    buf = OldRandomSeed;

    Tcl_AppendResult(interp, (const char *)buf , (char *) NULL);

    return TCL_OK;
}

// ptkSetSeed <Seed> 
//
// saves the random number seed into ptolemy.
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
//
int POct::ptkSetSeed (int aC,char** aV) {
    char* seedString;
    int seed;

    if (aC != 2) return 
        usage ("ptkSetSeed <Seed>");

    seedString = aV[1];

    // Convert the string into an integer
    if (Tcl_GetInt (interp, seedString, &seed) != TCL_OK) { 
        Tcl_AppendResult(interp, "Seed must be an integer", 
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (seed <= 0) {
        Tcl_AppendResult(interp, "Seed must be greater than zero", 
                         (char *) NULL);
        return TCL_ERROR;
    }

    KcEditSeed(seed);

    // Set the value of the seed to be used again
    OldRandomSeed = seed;
    
    return TCL_OK;
}

// ptkGetDomainNames <facet-id>
// returns a list of names of domains for the passed facet
//
// Written by Alan Kamas  12/93
// 
int POct::ptkGetDomainNames (int aC,char** aV) {
    octObject facet;
    char *domain;
    int nDomains, i;

    if (aC != 2) return  
            usage ("ptkGetDomainNames <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (!GOCDomainProp(&facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }
    // "domain" is now set to be the default domain

    nDomains = numberOfDomains();

    if(nDomains == 0) {
        Tcl_AppendResult(interp, 
			 "No domains supported by for this facet.",
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Return the list with the default Domain first

    if (nDomains == 1) {
	// Only one element means that no ordering need be done.
	Tcl_AppendElement ( interp, (char *)nthDomainName(0) );
	return TCL_OK;
    }

    // If the default domain matches any of our domain choices, put it first
    for (i = 0; i < nDomains; i++) {
	if (strcmp(nthDomainName(i), domain)==0) {
	    // The current domain has been found
            Tcl_AppendElement(interp, domain);
	}
    }
    // Now add the rest of the domain choices to the output list
    for (i = 0; i < nDomains; i++) {
        // Only add it if it has not already been used
        if (strcmp(nthDomainName(i), domain)!=0) {
            Tcl_AppendElement(interp, (char *)nthDomainName(i) );
        }
    }   
    return TCL_OK;
}


// ptkSetDomain <facet-id> <domain_name>
// saves the domain of the passed facet to the passed domain
//
// Written by Alan Kamas  12/93
// based on original code by Edwin Goei
//
int POct::ptkSetDomain (int aC,char** aV) {
    octObject facet;
    char *domain;

    if (aC != 3) return
      usage ("ptkSetDomain <OctObjectHandle> <DomainName>");

    domain = aV[2];

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the domain to be that of the passed name
    if (!SetDomainProp(&facet, domain)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    return TCL_OK;

}

// ptkGetTargetNames <facet-id>
// returns a list of names of targets for the passed facet
//
// Written by Alan Kamas  12/93
// 
int POct::ptkGetTargetNames (int aC,char** aV) {
    octObject facet;
    char *defaultTarget, *target ;
    char *domain;
    const char *targetNames[MAX_NUM_TARGETS];
    int nTargets, nChoices, i;

    if (aC != 2) return  
            usage ("ptkGetTargetNames <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (!GOCDomainProp(&facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }
    nTargets = KcDomainTargets(domain, targetNames, MAX_NUM_TARGETS);

    if(nTargets == 0) {
        Tcl_AppendResult(interp, 
			 "No targets supported by current domain.",
                         (char *) NULL);
        return TCL_ERROR;
    }

    /* for a galaxy, add "<parent>" as an option */
    if (IsGalFacet(&facet)) {
            targetNames[nTargets] = defaultTarget = "<parent>";
	    nChoices = nTargets + 1;
    }
    else {
            defaultTarget = (char *)KcDefTarget(domain);
	    nChoices = nTargets;
    }

    // Return the list with the current target first

    if (nChoices == 1) {
	// Only one element means that no ordering need be done.
	Tcl_AppendElement ( interp, (char *)targetNames[0] );
	return TCL_OK;
    }

    // Get Current Target name
    if (!GOCTargetProp(&facet, &target, defaultTarget)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }
    // If the Current Target matches any of our target choices, put it first
    for (i = 0; i < nChoices; i++) {
	if (strcmp(targetNames[i],target)==0) {
	    // The current target has been found
            Tcl_AppendElement(interp, target);
	    // mark it as having been used
	    targetNames[i] = "NIL";
	}
    }
    // Now add the rest of the target choices to the output list
    for (i = 0; i < nChoices; i++) {
        // Only add it if it has not already been used
        if (strcmp(targetNames[i],"NIL")!=0) {
            Tcl_AppendElement(interp, (char *)targetNames[i]);
        }
    }   
    return TCL_OK;
}

// ptkGetTargetParams <facet-id> <target_name>
// Returns a list of lists of the target parameters of a facet (if any).
// The command returns a list in the following format:
// { parameter_list }
// where parameter list is of the form:
//    {name1 type1 value1} {name2 type2 value2} ...
//
//
// Written by Alan Kamas  12/93
// based on original code by Edwin Goei
//
int POct::ptkGetTargetParams (int aC,char** aV) {
    octObject facet;
    char *domain;
    char *target ;

    if (aC != 3) return
            usage ("ptkGetTargetParams <OctObjectHandle> <TargetName>");

    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    target = aV[2];

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the domain to be that of the passed facet
    if (!GOCDomainProp(&facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to the passed target name
    if (!SetTargetProp(&facet, target)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Now (finally) get the Target Parameters

    // No target parameters to get if the target is "<parent>"
    if (strcmp(target,"<parent>")==0)  return result("NIL");

    // Get the pList form
    ParamListType pList;
    if (!GetTargetParams(target, &facet, &pList)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Convert the pList form into a Tcl list
    if (pList.length == 0) return result("NIL");
    for (int i=0; i<pList.length; i++) {
	Tcl_AppendResult(interp, "{", (char *) NULL);
        Tcl_AppendElement(interp, (char *)pList.array[i].name);
        Tcl_AppendElement(interp, (char *)pList.array[i].type);
        Tcl_AppendElement(interp, (char *)pList.array[i].value);
	Tcl_AppendResult(interp, " } ", (char *) NULL);
    }

    return TCL_OK;
}

// ptkSetTargetParams <facet-id> <target_name> <parameter_list>
// saves the values of the Target parameter_list into the Oct data base
//
// The command should be called as follows:
// command_name FacetHandle Target_Name { parameter_list }
// where parameter_list is of the form:
//    {name1 type1 value1} {name2 type2 value2} ...
//
// Written by Alan Kamas  12/93
// based on original code by Edwin Goei
//
int POct::ptkSetTargetParams (int aC,char** aV) {
    octObject facet;
    char *domain;
    char *target ;

    if (aC != 4) return
      usage ("ptkSetTargetParams <OctObjectHandle> <TargetName> <parameterList>");

    target = aV[2];

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the domain to be that of the passed facet
    if (!GOCDomainProp(&facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to the passed target name
    if (!SetTargetProp(&facet, target)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Covert the parameter List string into the pList structure.
    ParamListType pList;
    if (MakePList(aV[3], &pList) == 0) {
        Tcl_AppendResult(interp, "Unable to parse parameter list: ",
                         aV[3], (char *) NULL);
        return TCL_ERROR;
    }
    // Now that the pList has been made, it should be freed before returning.

    // Now (finally) set the Target Parameters
    if (pList.length > 0 ) {
        if (!SetTargetParams(&facet, &pList)) {
	    Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
            return TCL_ERROR;
        }
    }
    
    // Free up the pList as it is no longer needed
    DeletePList (&pList);

    return TCL_OK;
}


// ptkFacetContents <facet-id> <list_of_types>
// where <facet-id> is the string Handle of the Oct ID.
// and <list_of_types> is a list of the types of contained 
//   objects desired.  Note that the list could simply be 
//   a single type.
// 
// The function returns the string Handle of the Oct IDs
// of the contained objects of the appropriate types. 
// 
// Since it wasn't much extra work, I've made the function
// work with all of the possible OCT types:
// TERM NET INSTANCE PROP BAG POLYGON BOX CIRCLE PATH
// LABEL LAYER POINT EDGE FORMAL CHANGE_LIST CHANGE_RECORD
// 
// Thanks to Joe Buck for suggesting this function (and even
// giving its specifications).
//
// Written by Alan Kamas  9/93
// 
int POct::ptkFacetContents (int aC,char** aV) {
    octObject facet;

    if (aC != 3) return  
            usage ("ptkFacetContents <OctObjectHandle> <List_of_Types>");

    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Convert the Type list into an appropriate Oct Mask
    int typeC;
    char **typeV;
    if (Tcl_SplitList( interp, aV[2], &typeC, &typeV ) != TCL_OK){
	Tcl_AppendResult(interp, "Cannot parse list of types: ", aV[2],
                         (char *) NULL);
        return TCL_ERROR;
    }
    octObjectMask mask = 0; 
    for (int i=0; i< typeC; i++) {
	if (strcasecmp(typeV[i],"FACET")==0)  mask |= OCT_FACET_MASK;
	else if (strcasecmp(typeV[i],"TERM")==0)  mask |= OCT_TERM_MASK;
	else if (strcasecmp(typeV[i],"NET")==0)  mask |= OCT_NET_MASK;
	else if (strcasecmp(typeV[i],"INSTANCE")==0)  mask |= OCT_INSTANCE_MASK;
	else if (strcasecmp(typeV[i],"PROP")==0)  mask |= OCT_PROP_MASK;
	else if (strcasecmp(typeV[i],"BAG")==0)  mask |= OCT_BAG_MASK;
	else if (strcasecmp(typeV[i],"POLYGON")==0)  mask |= OCT_POLYGON_MASK;
	else if (strcasecmp(typeV[i],"BOX")==0)  mask |= OCT_BOX_MASK;
	else if (strcasecmp(typeV[i],"CIRCLE")==0)  mask |= OCT_CIRCLE_MASK;
	else if (strcasecmp(typeV[i],"PATH")==0)  mask |= OCT_PATH_MASK;
	else if (strcasecmp(typeV[i],"LABEL")==0)  mask |= OCT_LABEL_MASK;
	else if (strcasecmp(typeV[i],"LAYER")==0)  mask |= OCT_LAYER_MASK;
	else if (strcasecmp(typeV[i],"POINT")==0)  mask |= OCT_POINT_MASK;
	else if (strcasecmp(typeV[i],"EDGE")==0)  mask |= OCT_EDGE_MASK;
	else if (strcasecmp(typeV[i],"FORMAL")==0)  mask |= OCT_FORMAL_MASK;
	else if (strcasecmp(typeV[i],"CHANGE_LIST")==0)  mask |= OCT_CHANGE_LIST_MASK;
	else if (strcasecmp(typeV[i],"CHANGE_RECORD")==0)  mask |= OCT_CHANGE_RECORD_MASK;
    }
    // Free the memory used by typeV as it was allocated by SplitList
    free ((char *) typeV);

    // Use created mask to generate the desired oct object and return them
    Tcl_AppendResult(interp, " { ", NULL );
    octGenerator gen;
    octObject contentObj;
    char contentStr[16];
    octInitGenContents( &facet , mask, &gen );
    while (octGenerate(&gen, &contentObj) == OCT_OK) {
	ptkOctObj2Handle ( &contentObj, contentStr );
	Tcl_AppendResult(interp, contentStr, " ", NULL );
    }
    Tcl_AppendResult(interp, " } ", NULL );
    octFreeGenerator(&gen);
    
    return TCL_OK;
}

// ptkOpenMaster <OctInstanceHandle> [contents|interface]
// Returns the Oct ID of the master facet of the passed instance
// by default it returns the contents facet, but if the third
// arguement is "contents" or "interface" it returns that facet.
int POct::ptkOpenMaster (int aC,char** aV) {
    octObject instance;
    octObject facet;

    if ( (aC != 2) && (aC != 3) ) return 
          usage ("ptkOpenMaster <OctInstanceHandle> [contents|interface]");

    // "NIL" instances have "NIL" masters.  Hope this is what the
    // user expects
    if (strcmp(aV[1],"NIL")==0)  return result("NIL");

    if (ptkHandle2OctObj(aV[1], &instance) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Contents is the default master facet
    char facetType[32];
    if (aC==2) strcpy(facetType, "contents");
    else if ( strcmp(aV[2],"interface") == 0) strcpy(facetType,"interface");
         else strcpy(facetType, "contents");

    if (!MyOpenMaster( &facet, &instance, facetType, "r")) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    char facetStr[16];
    ptkOctObj2Handle ( &facet, facetStr );
    Tcl_AppendResult(interp, facetStr, NULL );

    return TCL_OK;
}

// ptkOpenFacet <file_name_of_cell> [view] [facet]
// If no view is specified, the default "schematic" is used.
// If no facet is specified, the default "contents" is used.
// Returns the Oct ID of the requested facet.  If the facet
// does not exist, it creates a new facet and returns the 
// Oct ID of the new facet.
// Note that this code is a modified version of the code
// in misc.c: RpcOpenFacet
// - Alan Kamas 9/93
//   based on original code by Edwin Goei
//
int POct::ptkOpenFacet (int aC,char** aV) {

    if ( (aC != 2) && (aC != 3) && (aC != 4) ) return
          usage ("ptkOpenFacet <file_name_of_cell> [view] [facet]");

    // fill in default values
    char viewType[32], facetType[32];
    if (aC < 3) strcpy(viewType, "schematic");
    else strcpy(viewType, aV[2]);
    if (aC < 4) strcpy(facetType, "contents");
    else strcpy(facetType, aV[3]);

    // Try to open the facet at least read only first.  This will work
    // if there already is a facet to read, but will fail if the facet
    // doesn't already exist
    octObject facet;
    octStatus status;
    status = OpenFacet(&facet, aV[1], viewType, facetType, "r");
    if (status == OCT_NO_EXIST) {
	// Create a new facet
        // note that the new facet must be a contents facet
        status = OpenFacet(&facet, aV[1], viewType, "contents", "a");
        if (status <= 0) {
	    // Could not create new facet
	    Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
            return TCL_ERROR;
        } else if (status == OCT_NEW_FACET) {
	    octObject prop;
            GetOrCreatePropStr(&facet, &prop, "TECHNOLOGY", UTechProp);
            GetOrCreatePropStr(&facet, &prop, "VIEWTYPE", "SCHEMATIC");
            // If facet is schematic:contents then use schematic editstyle,
            if ( (strcmp(facetType, "contents") == 0) && 
                 (strcmp(viewType, "schematic") == 0) ) {
                GetOrCreatePropStr(&facet, &prop, "EDITSTYLE", "SCHEMATIC");
            }
        }
    } else if (status <= 0) {
	// Unexpected Oct error when trying to open the facet
        Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
        return TCL_ERROR;
    }

    // Convert the new Facet into a string Oct ID Handle
    char facetHandle[16];
    ptkOctObj2Handle ( &facet, facetHandle );
    Tcl_AppendResult(interp, facetHandle, " ", NULL );

    return TCL_OK;

}

// Close an Oct facet
int POct::ptkCloseFacet(int argc, char **argv) {
    octObject facet;

    if (argc != 2) return usage("ptkCloseFacet <octObjectHandle>");
    if (!ptkHandle2OctObj(argv[1], &facet)) {
	Tcl_AppendResult(interp, "Can't convert Oct handle to facet",
	  (char *) NULL);
	return TCL_ERROR;
    }
    if (octCloseFacet(&facet) == OCT_OK) {
	return TCL_OK;
    } else {
	Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
	return TCL_ERROR;
    }
}

// Basic Oct Facet Type checking command
int POct::ptkIsStar (int aC,char** aV) {
    octObject facet;

    if (aC != 2) return usage ("ptkIsStar <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result(0);

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (IsStar(&facet)) return result (1);
    else return result(0);
}

// Basic Vem Facet Type checking command
int POct::ptkIsGalaxy (int aC,char** aV) {
    octObject facet;

    if (aC != 2) return usage ("ptkIsGalaxy <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result(0);

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (IsGal(&facet)) return result(1);
    else return result(0);
}

// Basic Vem Facet Type checking command
int POct::ptkIsBus (int aC,char** aV) {
    octObject facet;

    if (aC != 2) return usage ("ptkIsBus <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result(0);

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (IsBus(&facet)) return result(1);
    else return result(0);
}

// Basic Vem Facet Type checking command
int POct::ptkIsDelay (int aC,char** aV) {
    octObject facet;

    if (aC != 2) return usage ("ptkIsDelay <OctObjectHandle>");

    if (strcmp(aV[1],"NIL")==0)  return result(0);

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (IsDelay(&facet) || IsDelay2(&facet)) return result(1);
    else return result(0);
}

// Gets the current Iteration Number from the passed Oct Facet
int POct::ptkGetRunLength (int aC,char** aV) {
    int oldN;
    char value[64];
    octObject facet;

    if (aC != 2) return usage ("ptkGetRunLength <OctObjectHandle>");

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    /* Set Interate value to 10 if none had been specified */
    if (GetIterateProp(&facet, &oldN) == -1) { oldN = 10; }

    sprintf(value, "%d", oldN);
    return result ( value );
}

int POct::ptkSetRunLength (int aC,char** aV) {
    int IterationNumber;
    octObject facet;

    if (aC != 3) {
        return usage ("ptkSetRunLength <OctObjectHandle> <IterationValue>");
    }

    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    Tcl_GetInt(interp, aV[2], &IterationNumber);
    SetIterateProp(&facet, IterationNumber);
    return TCL_OK;
}


// Sets the Tk Event Loop checking to be on or off.  
// If the passed value is "on" or can be converted to 1, or yes, then on
// else "off" "0" or "no" turns it off.
int POct::ptkSetEventLoop (int aC,char** aV) {

    int boolarg;

    if (aC != 2) {
        return usage ("ptkSetEventLoop <on|off>");
    }

    Tcl_GetBoolean( interp, aV[1], &boolarg);

    KcSetEventLoop( boolarg );

    return TCL_OK;
}

// returns the name associated with the passed facet handle
// written by Xavier Warzee 12/94
// installed into Ptolemy by Alan Kamas 3/95
int POct::ptkGetStarName(int aC,char** aV) {
     octObject facet;
     char *starName;

     if (aC != 2) return
          usage("ptkGetStarName <octObjectHandle>");

     if(strcmp(aV[1],"NIL")==0) return result("NIL");

     if (ptkHandle2OctObj(aV[1], &facet) == 0) {
          Tcl_AppendResult(interp, "Bad or Stale facet Handle passed to ", 
                           aV[0], (char *) NULL);
          return TCL_ERROR;
     }

     // get the name from the oct object:
     GetStarName( &facet, &starName);

     // Make sure that the starName is defined
     if (starName == NULL) {
	  return result("NIL");
     } else {
          Tcl_AppendResult(interp, starName, (char *) NULL);
     }
     return TCL_OK;
}


// An InterpFuncP is a pointer to an PTcl function that takes an argc-argv
// argument list and returns TCL_OK or TCL_ERROR.

typedef int (POct::*InterpFuncP)(int,char**);

struct InterpTableEntry {
        char* name;
        InterpFuncP func;
};

// Here is the function table and dispatcher function.
// These macros define entries for the table

#define ENTRY(verb) { quote(verb), POct::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), POct::action }

// Here is the table.  Make sure it ends with "0,0"
static InterpTableEntry funcTable[] = {
	ENTRY(ptkCompile),
	ENTRY(ptkGetParams),
	ENTRY(ptkSetParams),
	ENTRY(ptkSetFindName),
	ENTRY(ptkGetMkStar),
	ENTRY(ptkSetMkStar),
	ENTRY(ptkSetRunUniverse),
	ENTRY(ptkGetStringProp),
	ENTRY(ptkSetStringProp),
	ENTRY(ptkGetMkSchemIcon),
	ENTRY(ptkSetMkSchemIcon),
	ENTRY(ptkGetSeed),
	ENTRY(ptkSetSeed),
	ENTRY(ptkGetDomainNames),
	ENTRY(ptkSetDomain),
	ENTRY(ptkGetTargetNames),
	ENTRY(ptkGetTargetParams),
	ENTRY(ptkSetTargetParams),
	ENTRY(ptkFacetContents),
	ENTRY(ptkOpenMaster),
	ENTRY(ptkOpenFacet),
	ENTRY(ptkCloseFacet),
	ENTRY(ptkIsStar),
	ENTRY(ptkIsGalaxy),
	ENTRY(ptkIsBus),
	ENTRY(ptkIsDelay),
	ENTRY(ptkGetRunLength),
	ENTRY(ptkSetRunLength),
	ENTRY(ptkSetEventLoop),
        ENTRY(ptkGetStarName),
	{ 0, 0 }
};

// register all the functions.
void POct::registerFuncs() {
	int i = 0;
	while (funcTable[i].name) {
		Tcl_CreateCommand (interp, funcTable[i].name,
				   POct::dispatcher, (ClientData)i, 0);
		i++;
	}
}

// static member: tells which Tcl interpreter is "innermost"
Tcl_Interp* POct::activeInterp = 0;

// dispatch the functions.
int POct::dispatcher(ClientData which,Tcl_Interp* interp,int argc,char* argv[])
			   {
	POct* obj = findPOct(interp);
	if (obj == 0) {
		strcpy(interp->result,
		       "Internal error in POct::dispatcher!");
		return TCL_ERROR;
	}
	int i = int(which);
	// this code makes an effective stack of active Tcl interpreters.
	Tcl_Interp* save = activeInterp;
	activeInterp = interp;
	int status = (obj->*(funcTable[i].func))(argc,argv);
	activeInterp = save;
	return status;
}
