static const char file_id[] = "POct.cc";

/**************************************************************************
Version identification:
@(#)POct.cc	1.86	10/04/99

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
		       
 Programmer:  Alan Kamas
 Date of creation: July 1993

This file implements a class that adds Ptolemy-specific Oct/Vem commands to
a Tcl interpreter.  

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

// Include standard include files to prevent conflict with
// the type definition Pointer used by "rpc.h". BLE
#include <stdio.h>
#include <string.h>
#include <unistd.h>             // getuid()

#include "POct.h"
extern POct *poct;

#include "PTcl.h"
extern PTcl *ptcl;  

#include "SimControl.h"

extern "C" {
#define Pointer screwed_Pointer		/* rpc.h and type.h define Pointer */
#include "vemInterface.h"		/* define VemLock, VemUnlock */
#include "local.h"
#include "oct.h"			/* octObject data structure */
#include <pwd.h>			/* used for Make Star */
#include "octIfc.h" 			/* define GetOrCreatePropStr */
#include "paramStructs.h"		/* ParamListType data structure */
#include "palette.h"			/* used by MkSchemIcon */
#include "util.h"			/* define UTechProp string */
#include "exec.h"			/* define ptkRun */
#include "err.h"			/* define ErrGet */
#include "ganttIfc.h"			/* define FindNameSet */
#include "icon.h"			/* define MkStar */
#include "compile.h"			/* define CompileFacet */
#include "xfunctions.h"			/* define win_msg */
#include "handle.h"			/* define ptkHandle2OctObj */
#include "kernelCalls.h"		/* define functions prefixed by Kc */
#undef Pointer
}

#include "miscFuncs.h"
#include "InfString.h"
#include "Error.h"	        	// Error class implemented by XError.c
#include "isa.h"			// define the quote macro

// Define objObjectClass which automatically handles initialization
// and deallocation of octObject data structures
#include "octObjectClass.h"

// The default number of iterations to run a universe
#define PIGI_DEFAULT_ITERATIONS	10

// The maximum number of targets for a domain
#define MAX_NUM_TARGETS 50

// Define the best way to pass back "NIL", 0, and 1 to the Tcl interpreter
// the result method is actually TclObj::result
// not sure if these strings can be passed using staticResult method -BLE
#define POCT_TCL_NIL result("NIL")
#define POCT_TCL_FALSE result("0")
#define POCT_TCL_TRUE result("1")

/////////////////////////////////////////////////////////////////
// "C" callable functions.  These functions are used to convert
// oct objects to strings and back again.  This is because TCL
// functions can only be passed strings.

// Converts an oct Facet Pointer into a string "handle" that
// can be used by a TCL interpreter
// To use this function, you must declare space for a char array that
// is at least 15 chars long (use POCT_FACET_HANDLE_LEN)
extern "C" void ptkOctObj2Handle( octObject *objPtr, char *stringValue )
{
        sprintf(stringValue, "OctObj%-.8lx", (long)objPtr->objectId);
}

// Converts a string "handle" into an oct Facet Pointer
// To use this function, you must declare space for a char array that
// is at least POCT_FACET_HANDLE_LEN chars long.
extern "C" int ptkHandle2OctObj(char* stringValue, octObject* objPtr) {
	int retval = TRUE;
        objPtr->type = OCT_UNDEFINED_OBJECT;
        objPtr->objectId = OCT_NULL_ID;

	// Make sure that stringValue is not NULL
	if (stringValue == 0) {
	    retval = FALSE;
	}
	// Convert the stringValue to an Oct identifier (address) in memory
	else if (sscanf(stringValue, "OctObj%lx", &objPtr->objectId) != 1 ||
	    objPtr->objectId == OCT_NULL_ID) {
	    retval = FALSE;
	}
	// Ask Oct to fill in the Oct information for that Oct identifier
	else if (octGetById(objPtr) == OCT_NOT_FOUND) {
	    FreeOctMembers(objPtr);
	    objPtr->type = OCT_UNDEFINED_OBJECT;
	    objPtr->objectId = OCT_NULL_ID;
	    retval = FALSE;
        }

        return retval;
}


// We want to be able to map Tcl_interp pointers to POct objects.
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

	// Initialize the states that remind users of previous inputs
	// for ptkSetSeed
        OldRandomSeed = 1;   
	// for ptkSetMkStar
	MkStarName = "";
	// g++-2.6.1 requires the (char *) in the next line
	MkStarDomain = (char *)DEFAULT_DOMAIN;
	MkStarDir = "NIL";
	MkStarPalette = "./user.pal";
	MkSchemPalette = "./user.pal";
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
int POct::SetBusParams(octObject *instPtr, ParamListType *pListp) {
    // Create uninitialized oct property object
    // cfront compiler doesn't support initialization code of the form
    //   octObject prop = {OCT_UNDEFINED_OBJECT};
    // NOTE: prop has non-dynamic members, so do not use FreeOctMembers
    // and do not define prop as an octObjectClass.
    octObject prop;
    prop.type = OCT_UNDEFINED_OBJECT;
    prop.objectId = OCT_NULL_ID;

    // Set up the Prop
    // This step may not be necessary (see above).
    GetOrInitBusProp(instPtr, &prop);

    // Fill in the props new value
    prop.contents.prop.type = OCT_STRING;

    // The line below will not compile under cfront 1.0
    //((const char*)(prop.contents.prop.value.string)) = pListp->array->value;
    prop.contents.prop.value.string = (char *)pListp->array->value;

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
int POct::SetDelayParams(octObject *instPtr, ParamListType *pListp) {
    // Create uninitialized oct property object
    // cfront compiler doesn't support initialization code of the form
    //   octObject prop = {OCT_UNDEFINED_OBJECT};
    // NOTE: prop has non-dynamic members, so do not use FreeOctMembers
    // and do not define prop as an octObjectClass.
    octObject prop;
    prop.type = OCT_UNDEFINED_OBJECT;
    prop.objectId = OCT_NULL_ID;

    // Set up the Prop
    // This step may not be necessary (see above).
    GetOrInitDelayProp(instPtr, &prop);

    // Fill in the props new value
    prop.contents.prop.type = OCT_STRING;

    // The line below will not compile under cfront1.0
    //((const char*)(prop.contents.prop.value.string)) = pListp->array->value;
    prop.contents.prop.value.string = (char *)pListp->array->value;

    // Save this new delay value;
    (void) octModify(&prop);

    return(TRUE);
}

// Deletes all of the elements of the passed parameter list pList
// The memory was allocated in POct::MakePList below 
// This does not apply to other ParamListType instances created by routines
// in compile.c or octIfc.c because these routines do not create new
// copies of name and value fields but instead are indices into a single
// string
void POct::DeletePList(ParamListType* pListp) {
    if ( pListp->flat_plist_flag ) {
	FreeFlatPList(pListp);
    }
    else if ( pListp ) {
        if ( pListp->array ) {
	    for (int i = 0; i < pListp->length; i++) {
		// strings created by savestring, which uses the new operator
		delete [] pListp->array[i].name;
		pListp->array[i].name = 0;
		delete [] pListp->array[i].type;
		pListp->array[i].type = 0;
		delete [] pListp->array[i].value;
		pListp->array[i].value = 0;
    	    }
	    FreeFlatPList(pListp);
	}
    }
}


// Converts a parameter list string of the form:
//  {name1 type1 value1} {name2 type2 value2} ...
// into the pList format.  Allocates space for the pList
// elements as well.  Note that all of the elements must
// be freed once the pList is no longer needed.  DeletePList
// does this.
int POct::MakePList(char* parameterList, ParamListType* pListp) {

    // Error checking
    // By default, return an empty parameter list (0 elements)
    pListp->length = 0;
    pListp->array = 0;
    pListp->dynamic_memory = 0;
    pListp->flat_plist_flag = FALSE;

    if (parameterList == 0 || *parameterList == 0) {
	Error::error("Empty parameter list"); 
	return FALSE;
    }

    if (strcmp(parameterList, "NIL") == 0) {
        return TRUE;
    }

    int aC = 0;
    char** aV = 0;
    if (Tcl_SplitList(interp, parameterList, &aC, &aV) != TCL_OK) {
	Error::error("Cannot parse parameter list: ", parameterList); 
	return FALSE;
    }

    pListp->length = aC;
    // Don't use new to create pListp->array, use calloc so that we
    // can easily free this from within C.
    // See FreeFlatPList(pListPtr) in paramStructs.c should be called
    // to free pListp->array.
    pListp->array = (ParamType *) calloc(aC, sizeof(ParamType));

    int ErrorFound = FALSE;
    for (int i = 0; i < pListp->length; i++) {

        int Element_aC = 0;
        char** Element_aV = 0;
        if (Tcl_SplitList(interp, aV[i], &Element_aC, &Element_aV) != TCL_OK) {
            Error::error("Error in parsing parameter list element: ", aV[i]);
            // Keep going, and then clear it all out of memory when done.
            ErrorFound = TRUE;
        }

        ParamType* pElement = &pListp->array[i];
        if (Element_aC != 3 ){
           Error::error("Parameter list element ", aV[i], 
                        " does not have 3 elements.");
           ErrorFound = TRUE;
           pElement->name = NULL;
           pElement->type = NULL;
           pElement->value = NULL;
        } else {
	   pElement->name = savestring(Element_aV[0]);
	   pElement->type = savestring(Element_aV[1]);
	   pElement->value = savestring(Element_aV[2]);
        }

        // Free the memory used by Element_aV as it is no longer needed
	// Only a single call to free is needed (see Ousterhout, p. 317)
        free ((char *) Element_aV);
    }

    // Free the memory used by aV as it is no longer needed
    // Only a single call to free is needed (see Ousterhout, p. 317)
    free((char *) aV);
    aV = 0;
    aC = 0;

    // If an error was encountered while parsing the list, free pList
    // and return FALSE
    if (ErrorFound) {
        DeletePList(pListp);
        return FALSE;
    }
    return TRUE;
}


// ------- TCL Commands --------------------------------------------

// Calls the compile.c code.  May want to update it so that
// this code uses the PTcl function calls.
// FIXME:  If a non-universe facet is passed to this function,
//         it churns ahead anyway and then crashes.  - aok
//         fix it by adding an IsUniv check.
// Do not use octObjectClass: let the memory leak happen; otherwise
// we will get an array bounds read error from Purify. -BLE
int POct::ptkCompile (int aC, char** aV) {
    // Create uninitialized facet
    // cfront compiler doesn't support initialization code of the form
    //   octObject facet = {OCT_UNDEFINED_OBJECT, OCT_NULL_ID};
    octObject facet;
    facet.type = OCT_UNDEFINED_OBJECT;
    facet.objectId = OCT_NULL_ID;

    // Error checking: number of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkCompile <OctObjectHandle>");
    }
    // FIXME: Memory leak because we never deallocate facet (see above)
    if (!ptkHandle2OctObj(aV[1], &facet)) {
	Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0], 
                         (char *) NULL);
	FreeOctMembers(&facet);
        return TCL_ERROR;
    }

    // Compile the facet
    int retval = TCL_OK;
    if ( ! CompileFacet(&facet) ) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
	retval = TCL_ERROR;
    }

    return retval;
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
int POct::ptkGetParams (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 3) {
        return usage ("ptkGetParams <OctFacetHandle> <OctInstanceHandle>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Check to see if a valid instance was passed
    ParamListType pList = {0, 0, 0, FALSE};
    char title[64];
    if (strcmp(aV[2], "NIL") == 0) {
        // If there is no instance, then this must be a Galaxy or Universe
        if (IsGalFacet(facet) || IsUnivFacet(facet)) {
            if (!GetFormalParams(facet, &pList)) {
		Tcl_AppendResult( interp, "Error getting Formal parameters. ",
		                  ErrGet(), (char *)NULL );
		return TCL_ERROR;
            }
            strcpy(title, "Edit Formal Parameters");
            // Result string will be built below
        } else {
	    win_msg ("Not a Star, Galaxy, or Universe");
	    return TCL_OK;
        }
    } else {
        // There was an instance passed
        octObjectClass instance;
        if (!ptkHandle2OctObj(aV[2], instance)) {
            Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", 
		aV[0], (char *) NULL);
	    return TCL_ERROR;
	}
    
	if ( IsDelay(instance) || IsBus(instance) || IsDelay2(instance)) {
	    // Parameters are stored differently for delays and buses.
	    // can't use the "plist" form as for Stars, Gals, and Formals
	    octObjectClass property;
	    octObject* propertyp = (octObject *) property;
	    if (IsDelay(instance)) {
		GetOrInitDelayProp(instance, propertyp);
		strcpy(title, "Edit Delay");
	    } else if (IsDelay2(instance)) {
		GetOrInitDelayProp(instance, propertyp);
		strcpy(title, "Edit Delay With Initial Values");
	    } else {
		GetOrInitBusProp(instance, propertyp);
		strcpy(title, "Edit Bus");
	    }
            // Convert "property" into Result string
            Tcl_AppendElement(interp, title);
            Tcl_AppendResult(interp, " { {", NULL);
            Tcl_AppendElement(interp, propertyp->contents.prop.name);
	    char dataTypeStr[16], size[32];
            switch(propertyp->contents.prop.type) {
              case OCT_INTEGER:
                strcpy(dataTypeStr, "INTEGER");
                Tcl_AppendElement(interp, dataTypeStr);
                sprintf(size, "%ld", propertyp->contents.prop.value.integer);
                Tcl_AppendElement(interp, size);
                break;
              case OCT_STRING:
                strcpy(dataTypeStr, "STRING");
                Tcl_AppendElement(interp, dataTypeStr);
                Tcl_AppendElement(interp,
				  propertyp->contents.prop.value.string);
                break;
              default:
                Error::error("type unknown in editing delay/bus width");
		return TCL_ERROR;
            }
            Tcl_AppendResult(interp, "} }", NULL);
            return TCL_OK;
  	}
	else if (IsGal(instance) || IsStar(instance)) {
	    // Must be a star or Galaxy
	    // Set the domain to be that of the instance
	    if (!setCurDomainInst(instance)) {
                Tcl_AppendResult(interp, "Invalid Domain Found.", (char *)NULL);
                return TCL_ERROR;
            }
	    if (!GetOrInitSogParams(instance, &pList)) {
		Tcl_AppendResult(interp, 
                                 "Error Getting Star or Galaxy parameters.  ", 
                                 ErrGet(), (char *) NULL);
                return TCL_ERROR;
            }
            strcpy(title, "Edit Actual Parameters");
            // Build results string below.
        } else {
	    win_msg("Not a star, galaxy, bus, or delay instance");
            return TCL_OK;
        }
    }

    // Clear the interp->result by setting it to "".
    Tcl_SetResult(interp,"",TCL_STATIC);

    // Convert pList into a TCL list of Strings to return as result
    Tcl_AppendElement (interp, title);
    if (pList.length > 0) {
        // There are some parameters here
        ParamType *place = pList.array;
        Tcl_AppendResult(interp, " { ", NULL);
        for (int i = 0; i < pList.length; i++) {
            Tcl_AppendResult(interp, " {", NULL);
            Tcl_AppendElement(interp, (char*)place->name);
            Tcl_AppendElement(interp, (char*)place->type);
            Tcl_AppendElement(interp, (char*)place->value);
            Tcl_AppendResult(interp, "}", NULL);
            place++;
        }
        Tcl_AppendResult(interp, " }", NULL);

	// Free pList which was created by PStrToPList via a call to
	// GetFormalParams or GetOrInitSogParams
	FreeFlatPList(&pList);
    } else {					// There are no parameters
	Tcl_AppendResult(interp, " NIL", NULL);
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
int POct::ptkSetParams (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 4) {
	return usage(
          "ptkSetParams <OctFacetHandle> <OctInstnceHandle> <Parameter_List>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Covert the parameter List string in the pList structure.
    // Be sure to free pList before returning
    ParamListType pList = {0, 0, 0, FALSE};
    if (!MakePList(aV[3], &pList)) {
        Tcl_AppendResult(interp, "Unable to parse parameter list: ",
                         aV[3], (char *) NULL);
        return TCL_ERROR;
    }

    // Check to see if a valid instance was passed
    int ErrorFound = FALSE;
    if (strcmp(aV[2], "NIL") == 0) {
        // If there is no instance, then this must be a Galaxy or Universe
        if (IsGalFacet(facet) || IsUnivFacet(facet)) {
            // Set Formal Parameters
            if (SetFormalParams(facet, &pList) == 0) {
                ErrorFound = TRUE;
                Tcl_AppendResult(interp, aV[0],
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }
        }
	else {
            ErrorFound = TRUE;
            Tcl_AppendResult(interp,
                             "Not a Star, Galaxy, Universe.",
                             (char *) NULL);
        }
    }
    else {
        // There was a valid instance passed
        octObjectClass instance;
        if (!ptkHandle2OctObj(aV[2], instance)) {
            // No way to go on if the handle is bad.  Clean up and exit.
	    ErrorFound = TRUE;
            Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", 
			     aV[0], (char *) NULL);
        }
        else if ( IsDelay(instance) || IsDelay2(instance)) {
            // Set Delay Parameters from the pList
            if (SetDelayParams(instance, &pList) == 0) {
                ErrorFound = TRUE;
		Tcl_AppendResult(interp, aV[0], 
				 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }
        }
	else if (IsBus(instance)) {
            // Set Bus Parameters from the pList
            if (SetBusParams(instance, &pList) == 0) {
                ErrorFound = TRUE;
		Tcl_AppendResult(interp, aV[0], 
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }

        }
	else if (IsGal(instance) || IsStar(instance)) {
            // Must be a star or Galaxy
	    // Set the domain to be that of the instance
	    if (!setCurDomainInst(instance)) {
		ErrorFound = TRUE;
                Tcl_AppendResult(interp,"Invalid Domain Found.",(char *) NULL);
            }
            // Set Star or Galaxy params from the pList
            else if (SetSogParams(instance, &pList) == 0) {
                ErrorFound = TRUE;
		Tcl_AppendResult(interp, aV[0],
                                 " Could not save parameters to Oct. ",
                                 ErrGet(), (char *) NULL);
            }
        }
	else {
            ErrorFound = TRUE;
            Tcl_AppendResult(interp, 
                             "Not a star, galaxy, bus, or delay instance.",
                             (char *) NULL);
        }
    }

    // Clean up memory created by MakePList
    DeletePList(&pList);

    if (ErrorFound) return TCL_ERROR;
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
int POct::ptkSetFindName (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 3) {
	return usage ("ptkSetFindName <OctObjectHandle> <Name>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Perform the Find Name function:
    char* name = aV[2];
    int retval = TCL_OK;
    if (!FindNameSet(facet, name)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        retval = TCL_ERROR;
    }

    return retval;
}

// ptkSetRunUniverse <facet-id> <ParameterList> 
//
// Saves the passed Parameter List and then runs the facet
//
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
//
int POct::ptkSetRunUniverse (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file, facet is universe
    if (aC != 3) {
	return usage("ptkSetRunUniverse <OctObjectHandle> <ParameterList>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }
    if (!IsUnivFacet(facet)){
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
    int retval = TCL_OK;
    if ( Tcl_VarEval(interp, "ptkSetParams ", aV[1], " NIL ",
		     " \"[list ", aV[2], " ]\" ", (char *)NULL) != TCL_OK ) {
        retval = TCL_ERROR; 
    }
    // Run the Facet
    else if ( ! ptkRun(facet, TRUE) ) {
        retval = TCL_ERROR; 
    }

    return retval; 
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
int POct::ptkGetStringProp (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 3) {
        return usage("ptkGetStringProp <OctObjectHandle> propName");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Get value of property
    int retval = TCL_OK;
    const char* value = 0;
    if (!GetStringProp(facet, aV[2], &value)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        retval = TCL_ERROR;
    } else if (value == NULL) {		// Make sure that value is defined
        Tcl_AppendResult(interp, "", (char *) NULL);
    } else {		// return value as list element to preserve white space
        Tcl_AppendResult(interp, value, (char *) NULL);
    }

    return retval;
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
int POct::ptkSetStringProp (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 4) {
	return usage("ptkSetStringProp <OctObjectHandle> <propName> <value>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the property value in the passed facet
    char* propName = aV[2];
    char* value = aV[3];
    if (!SetStringProp(facet, propName, value)) {
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
    // Error checking: number of arguments
    if (aC != 1) {
	return usage ("ptkGetMkSchemIcon");
    }
    Tcl_AppendResult(interp, (char *)MkSchemPalette, (char *) NULL);
    return TCL_OK;
}

// ptkSetMkSchemIcon <facet-id> <Palette>
//
// Makes a Schematic Icon of the passed facet and stores
// the new icon into the passed palette directory.
//
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
// based on original code by Edwin Goei
//
int POct::ptkSetMkSchemIcon (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 3) {
	return usage ("ptkSetMkSchemIcon <OctObjectHandle> <Palette>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    MkSchemPalette = aV[2];
    int retval = TCL_OK;
    char *palette = aV[2];
    char buf[512];
    if (!GetTildePath(facet, buf)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        retval = TCL_ERROR;
    }
    else {
        DirName(buf);
        if (IsGalFacet(facet)) {
	    if (!MkGalIconInPal(facet, buf, palette)) {
		Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
		retval = TCL_ERROR;
	    }
	}
	else if (IsPalFacet(facet)) {
	    if (!MkPalIconInPal(facet, buf, palette)) {
		Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
		retval = TCL_ERROR;
	    }
	}
	else if (!MkUnivIconInPal(facet, buf, palette)) {
	    Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
	    retval = TCL_ERROR;
	}
    }

    return retval;
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
    // Error checking: number of arguments
    if (aC != 1) {
	return usage ("ptkGetMkStar");
    }

    // Initialize star src directory to the home directory of the user
    struct passwd* pwent = 0;
    if (strcmp(MkStarDir, "NIL") == 0) {
        if ((pwent = getpwuid(getuid())) == NULL) {
            Tcl_AppendResult(interp, "Cannot get password entry",
			     (char *) NULL);
            return TCL_ERROR;
        }
        MkStarDir = "~";
	MkStarDir << pwent->pw_name << "/";
    }

    // Note: if any of the titles (i.e. "Star name") change, that
    //       change must be reflected in ptkSetMkStar
    Tcl_AppendResult(interp, 
                     "{{Star name} {", (char *)MkStarName, "}} ",
                     "{{Domain} {", (char *)MkStarDomain, "}} ",
                     "{{Star src directory} {", (char *)MkStarDir, "}} ", 
                     "{{Pathname of Palette} {", (char *)MkStarPalette, "}}", 
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
int POct::ptkSetMkStar (int aC, char** aV) {

    // Error checking: number of arguments
    if (aC != 5) {
	return usage("ptkSetMkStar <StarName> <Domain> <SrcDir> <PaleteDir>");
    }
    
    // Store these values for the next time MkStar is called
    MkStarName = aV[1];
    MkStarDomain = aV[2];
    MkStarDir = aV[3];
    MkStarPalette = aV[4];

    // Note that only need to test if MkStarDir starts with '~'
    // so that the user may enter something like "~/work/stars" where
    // '~' stands for user's own home directory.  -wtc
    // allow $PTOLEMY (kennard, Oct92)
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
// FIXME: Ideally, GetSeed and SetSeed should be PTcl functions
//        There is already a "seed" command there.
int POct::ptkGetSeed (int aC, char** /*aV*/) {

    // Error checking: number of arguments
    if (aC != 1) {
	return usage("ptkGetSeed");
    }
    
    // Convert OldRandomSeed to a string and send it to Tcl
    InfString buf = OldRandomSeed;
    Tcl_AppendResult(interp, (char *)buf, (char *) NULL);

    return TCL_OK;
}

// ptkSetSeed <Seed> 
//
// saves the random number seed into ptolemy.
// This procedure was written to work with ptkEditStrings
//
// Written by Alan Kamas  1/94
//
int POct::ptkSetSeed (int aC, char** aV) {

    // Error checking: number of arguments
    if (aC != 2) {
	return usage("ptkSetSeed <Seed>");
    }

    // Convert the seed string into an integer
    int seed;
    if (Tcl_GetInt (interp, aV[1], &seed) != TCL_OK) { 
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
int POct::ptkGetDomainNames (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage("ptkGetDomainNames <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0) return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Read domain from facet
    const char* domain = 0;	// not dynamic memory: returned via HashString
    if (!GOCDomainProp(facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // "domain" is now set to be the default domain
    int nDomains = numberOfDomains();
    if (nDomains == 0) {
        Tcl_AppendResult(interp, 
			 "No domains supported by this facet.",
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Return the list with the default Domain first

    if (nDomains == 1) {
	// Only one element means that no ordering need be done.
	Tcl_AppendElement(interp, (char *)nthDomainName(0));
	return TCL_OK;
    }

    // If the default domain matches any of our domain choices, put it first
    int i;
    for (i = 0; i < nDomains; i++) {
	if (strcmp(nthDomainName(i), domain) == 0) {
	    // The current domain has been found
            Tcl_AppendElement(interp, (char *)domain);
	}
    }

    // Now add the rest of the domain choices to the output list
    for (i = 0; i < nDomains; i++) {
        // Only add it if it has not already been used
        if (strcmp(nthDomainName(i), domain) != 0) {
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
int POct::ptkSetDomain (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 3) {
	return usage ("ptkSetDomain <OctObjectHandle> <DomainName>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the domain to be that of the passed name
    int retval = TCL_OK;
    char* domain = aV[2];
    if ( ! SetDomainProp(facet, domain) ) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        retval = TCL_ERROR;
    }

    return retval;
}

// ptkGetTargetNames <facet-id>
// returns a list of names of targets for the passed facet
//
// Written by Alan Kamas  12/93
// 
int POct::ptkGetTargetNames (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkGetTargetNames <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Read the domain from the facet
    const char* domain = 0;	// not dynamic memory: returned via HashString
    if (!GOCDomainProp(facet, &domain, DEFAULT_DOMAIN)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
	return TCL_ERROR;
    }

    // Initialize the names and number of the targets
    const char* targetNames[MAX_NUM_TARGETS];
    int nTargets = KcDomainTargets(domain, targetNames, MAX_NUM_TARGETS);
    if (nTargets == 0) {
        Tcl_AppendResult(interp, 
			 "No targets supported by current domain.",
                         (char *) NULL);
        return TCL_ERROR;
    }

    // For a galaxy, add "<parent>" as an option
    int nChoices = nTargets;
    char* defaultTarget;
    if (IsGalFacet(facet)) {
            targetNames[nTargets] = defaultTarget = "<parent>";
	    nChoices++;
    }
    else {
            defaultTarget = (char *)KcDefTarget(domain);
    }

    // Return the list with the current target first
    if (nChoices == 1) {
	// Only one element means that no ordering need be done.
	Tcl_AppendElement ( interp, (char *)targetNames[0] );
	return TCL_OK;
    }

    // Get Current Target name
    const char* target = 0;
    if (!GOCTargetProp(facet, &target, defaultTarget)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // If the Current Target matches any of our target choices, put it first
    int i;
    for (i = 0; i < nChoices; i++) {
	if (strcmp(targetNames[i], target) == 0) {
	    // The current target has been found
            Tcl_AppendElement(interp, (char *)target);
	    // mark it as having been used
	    targetNames[i] = "NIL";
	}
    }

    // Now add the rest of the target choices to the output list
    for (i = 0; i < nChoices; i++) {
        // Only add it if it has not already been used
        if (strcmp(targetNames[i], "NIL") != 0) {
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
int POct::ptkGetTargetParams (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 3) {
	return usage ("ptkGetTargetParams <OctObjectHandle> <TargetName>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to be the second argument
    char* target = aV[2];

    // Set the domain to be that of the passed facet
    const char *domain;		// not dynamic memory: returned via HashString
    if (!GOCDomainProp(facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to the passed target name
    if (!SetTargetProp(facet, target)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Now (finally) get the Target Parameters

    // No target parameters to get if the target is "<parent>"
    if (strcmp(target, "<parent>") == 0) {
	return POCT_TCL_NIL;
    }

    // Get the pList form
    ParamListType pList = {0, 0, 0, FALSE};
    if (!GetTargetParams(target, facet, &pList)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Convert the pList form into a Tcl list
    if (pList.length == 0) {
	FreeFlatPList(&pList);
	return POCT_TCL_NIL;
    }

    for (int i=0; i<pList.length; i++) {
	Tcl_AppendResult(interp, "{", (char *) NULL);
        Tcl_AppendElement(interp, (char *)pList.array[i].name);
        Tcl_AppendElement(interp, (char *)pList.array[i].type);
        Tcl_AppendElement(interp, (char *)pList.array[i].value);
	Tcl_AppendResult(interp, " } ", (char *) NULL);
    }

    // Clean up memory
    FreeFlatPList(&pList);		// allocated by GetTargetParams

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
int POct::ptkSetTargetParams (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 4) {
	return usage("ptkSetTargetParams <OctObjectHandle> <TargetName> <parameterList>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to be the second argument
    char* target = aV[2];

    // Set the domain to be that of the passed facet
    const char *domain;		// not dynamic memory: returned via HashString
    if (!GOCDomainProp(facet, &domain, DEFAULT_DOMAIN)) {
        Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Set the target to the passed target name
    if (!SetTargetProp(facet, target)) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    // Covert the parameter List string into the pList structure.
    // Now that the pList has been made, it should be freed before returning.
    ParamListType pList = {0, 0, 0, FALSE};
    if (!MakePList(aV[3], &pList)) {
        Tcl_AppendResult(interp, "Unable to parse parameter list: ",
                         aV[3], (char *) NULL);
        return TCL_ERROR;
    }

    // Now (finally) set the Target Parameters
    if (pList.length > 0 ) {
        if (!SetTargetParams(facet, &pList)) {
	    DeletePList(&pList); 	// Clean up memory created by MakePList
	    Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
            return TCL_ERROR;
        }
    }

    // Clean up memory created by MakePList
    DeletePList(&pList);

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
int POct::ptkFacetContents (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 3) {
	return usage ("ptkFacetContents <OctObjectHandle> <List_of_Types>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
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
    for (int i = 0; i < typeC; i++) {
	const char* str = typeV[i];
	switch( toupper(*str) ) {
	  case 'B':
	    if (strcasecmp(str, "BAG") == 0)
		mask |= OCT_BAG_MASK;
	    else if (strcasecmp(str, "BOX") == 0)
		mask |= OCT_BOX_MASK;
	    break;
	  case 'C':
	    if (strcasecmp(str, "CHANGE_LIST") == 0)
		mask |= OCT_CHANGE_LIST_MASK;
	    else if (strcasecmp(str, "CHANGE_RECORD")==0)
		mask |= OCT_CHANGE_RECORD_MASK;
	    else if (strcasecmp(str, "CIRCLE") == 0)
		mask |= OCT_CIRCLE_MASK;
	    break;
	  case 'E':
	    if (strcasecmp(str, "EDGE") == 0)
		mask |= OCT_EDGE_MASK;
	    break;
	  case 'F':
	    if (strcasecmp(str, "FACET") == 0)
		mask |= OCT_FACET_MASK;
	    else if (strcasecmp(str, "FORMAL") == 0)
		mask |= OCT_FORMAL_MASK;
	    break;
	  case 'I':
	    if (strcasecmp(str, "INSTANCE") == 0)
		mask |= OCT_INSTANCE_MASK;
	    break;
	  case 'L':
	    if (strcasecmp(str, "LABEL") == 0)
		mask |= OCT_LABEL_MASK;
	    else if (strcasecmp(str, "LAYER") == 0)
		mask |= OCT_LAYER_MASK;
	    break;
	  case 'N':
	    if (strcasecmp(str, "NET") == 0)
		mask |= OCT_NET_MASK;
	    break;
	  case 'P':
	    if (strcasecmp(str, "PATH") == 0)
		mask |= OCT_PATH_MASK;
	    else if (strcasecmp(str, "POINT") == 0)
		mask |= OCT_POINT_MASK;
	    else if (strcasecmp(str, "POLYGON") == 0)
		mask |= OCT_POLYGON_MASK;
	    else if (strcasecmp(str, "PROP") == 0)
		mask |= OCT_PROP_MASK;
	    break;
	  case 'T':
	    if (strcasecmp(str, "TERM") == 0)
		mask |= OCT_TERM_MASK;
	    break;
	}
    }

    // Free the memory used by typeV as it was allocated by SplitList
    // Only a single call to free is needed (see Ousterhout, p. 317)
    free ((char *) typeV);

    // Use created mask to generate the desired oct object and return them
    Tcl_AppendResult(interp, " { ", NULL );
    octGenerator gen;
    octObjectClass contentObj;
    char contentStr[POCT_FACET_HANDLE_LEN];
    octInitGenContents(facet, mask, &gen);
    while (octGenerate(&gen, contentObj) == OCT_OK) {
	ptkOctObj2Handle(contentObj, contentStr);
	Tcl_AppendResult(interp, contentStr, " ", NULL);
    }
    Tcl_AppendResult(interp, " } ", NULL );
    octFreeGenerator(&gen);

    return TCL_OK;
}

// ptkOpenMaster <OctInstanceHandle> [contents|interface]
// Returns the Oct ID of the master facet of the passed instance
// by default it returns the contents facet, but if the third
// argument is "contents" or "interface" it returns that facet.
int POct::ptkOpenMaster (int aC, char** aV) {
    octObjectClass instance;
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    // "NIL" instances have "NIL" masters.  Hope this is what the user expects
    if ( (aC < 2) || (aC > 3) ) {
	return usage("ptkOpenMaster <OctInstanceHandle> [contents|interface]");
    }
    if (strcmp(aV[1], "NIL") == 0) return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], instance)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Contents is the default master facet
    char facetType[32];
    if (aC == 2) strcpy(facetType, "contents");
    else if ( strcmp(aV[2], "interface") == 0) strcpy(facetType, "interface");
    else strcpy(facetType, "contents");

    if ( ! MyOpenMaster(facet, instance, facetType, "r") ) {
	Tcl_AppendResult(interp, ErrGet(), (char *) NULL);
        return TCL_ERROR;
    }

    char facetStr[POCT_FACET_HANDLE_LEN];
    ptkOctObj2Handle(facet, facetStr);
    Tcl_AppendResult(interp, facetStr, (char *) NULL);

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
int POct::ptkOpenFacet (int aC, char** aV) {

    // Error checking: number of arguments
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
    // do not use octObjectClass for facet, because destructor causes
    // "freeing non-heap memory" error in Purify
    // cfront compiler doesn't support initialization code of the form
    //   octObject facet = {OCT_UNDEFINED_OBJECT, OCT_NULL_ID};
    octObject facet;
    facet.type = OCT_UNDEFINED_OBJECT;
    facet.objectId = OCT_NULL_ID;
    octStatus status = OpenFacet(&facet, aV[1], viewType, facetType, "r");
    if (status == OCT_NO_EXIST) {
	// Create a new facet
        // note that the new facet must be a contents facet
        status = OpenFacet(&facet, aV[1], viewType, "contents", "a");
        if (status <= 0) {
	    // Could not create new facet
	    Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
            return TCL_ERROR;
        } else if (status == OCT_NEW_FACET) {
	    // cfront compiler doesn't support initialization code of the form
	    //   octObject prop = {OCT_UNDEFINED_OBJECT, OCT_NULL_ID};
	    octObject prop;
	    prop.type = OCT_UNDEFINED_OBJECT;
	    prop.objectId = OCT_NULL_ID;
            GetOrCreatePropStr(&facet, &prop, "TECHNOLOGY", UTechProp);
	    FreeOctMembers(&prop);
            GetOrCreatePropStr(&facet, &prop, "VIEWTYPE", "SCHEMATIC");
	    FreeOctMembers(&prop);
            // If facet is schematic:contents then use schematic editstyle,
            if ( (strcmp(facetType, "contents") == 0) && 
                 (strcmp(viewType, "schematic") == 0) ) {
                GetOrCreatePropStr(&facet, &prop, "EDITSTYLE", "SCHEMATIC");
	        FreeOctMembers(&prop);
            }
        }
    } else if (status <= 0) {
	// Unexpected Oct error when trying to open the facet
        Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
        return TCL_ERROR;
    }

    // Convert the new Facet into a string Oct ID Handle
    char facetHandle[POCT_FACET_HANDLE_LEN];
    ptkOctObj2Handle(&facet, facetHandle);
    Tcl_AppendResult(interp, facetHandle, " ", NULL );
    FreeOctMembers(&facet);

    return TCL_OK;

}

// Close an Oct facet
int POct::ptkCloseFacet(int argc, char **argv) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (argc != 2) {
	return usage("ptkCloseFacet <octObjectHandle>");
    }
    if (!ptkHandle2OctObj(argv[1], facet)) {
	Tcl_AppendResult(interp, "Can't convert Oct handle to facet",
			 (char *) NULL);
	return TCL_ERROR;
    }

    int retval = TCL_OK;
    if (octCloseFacet(facet) != OCT_OK) {
	Tcl_AppendResult(interp, octErrorString(), (char *) NULL);
	// Should we free the facet after we close it?
	retval = TCL_ERROR;
    }

    return retval;
}

// Basic Oct Facet Type checking command
int POct::ptkIsStar (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkIsStar <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_FALSE;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // We cannot use a temporary variable retval because
    // POCT_TCL_TRUE and POCT_TCL_FALSE have side effects
    if (IsStar(facet)) {
	return POCT_TCL_TRUE;
    }
    return POCT_TCL_FALSE;
}

// Basic Vem Facet Type checking command
int POct::ptkIsGalaxy (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkIsGalaxy <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_FALSE;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // We cannot use a temporary variable retval because
    // POCT_TCL_TRUE and POCT_TCL_FALSE have side effects
    if (IsGal(facet)) {
	return POCT_TCL_TRUE;
    }
    return POCT_TCL_FALSE;
}

// Basic Vem Facet Type checking command
int POct::ptkIsBus (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkIsBus <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0) return POCT_TCL_FALSE;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // We cannot use a temporary variable retval because
    // POCT_TCL_TRUE and POCT_TCL_FALSE have side effects
    if (IsBus(facet)) {
	return POCT_TCL_TRUE;
    }
    return POCT_TCL_FALSE;
}

// Basic Vem Facet Type checking command
int POct::ptkIsDelay (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage ("ptkIsDelay <OctObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0)  return POCT_TCL_FALSE;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    if (IsDelay(facet) || IsDelay2(facet)) {
	return POCT_TCL_TRUE;
    }
    return POCT_TCL_FALSE;
}

// Gets the current Iteration Number from the passed Oct Facet
int POct::ptkGetRunLength (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 2) return usage ("ptkGetRunLength <OctObjectHandle>");
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    // Read number-of-iterations from the Oct facet and convert to a string
    // If no number-of-iterations has been specified, use a default value
    int IterationNumber = PIGI_DEFAULT_ITERATIONS;
    if (GetIterateProp(facet, &IterationNumber) == -1) {
        IterationNumber = PIGI_DEFAULT_ITERATIONS;
    }

    return result(IterationNumber);
}

// Sets the current Iteration Number in the passed Oct Facet
int POct::ptkSetRunLength (int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, oct facet file
    if (aC != 3) {
        return usage ("ptkSetRunLength <OctObjectHandle> <IterationValue>");
    }
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    int IterationNumber = PIGI_DEFAULT_ITERATIONS;
    if (Tcl_GetInt(interp, aV[2], &IterationNumber) != TCL_OK) {
        Tcl_AppendResult(interp, "Number of iterations must be an integer", 
                         (char *) NULL);
        return TCL_ERROR;
    }
    SetIterateProp(facet, IterationNumber);
    return TCL_OK;
}


// Sets the Tk Event Loop checking to be on or off.  
// If the passed value is "on" or can be converted to 1, or yes, then on
// else "off" "0" or "no" turns it off.
int POct::ptkSetEventLoop (int aC, char** aV) {

    // Error checking: number of arguments
    if (aC != 2) {
        return usage("ptkSetEventLoop <on|off>");
    }

    int boolarg = FALSE;
    Tcl_GetBoolean(interp, aV[1], &boolarg);
    KcSetEventLoop(boolarg);
    return TCL_OK;
}

// returns the name associated with the passed facet handle
// written by Xavier Warzee 12/94
// installed into Ptolemy by Alan Kamas 3/95
int POct::ptkGetStarName(int aC, char** aV) {
    octObjectClass facet;

    // Error checking: number of arguments, value of arguments, oct facet file
    if (aC != 2) {
	return usage("ptkGetStarName <octObjectHandle>");
    }
    if (strcmp(aV[1], "NIL") == 0) return POCT_TCL_NIL;
    if (!ptkHandle2OctObj(aV[1], facet)) {
        Tcl_AppendResult(interp, "Bad or Stale facet Handle passed to ", 
                         aV[0], (char *) NULL);
        return TCL_ERROR;
    }

    // get the star name from the oct object and make sure it is defined
    char* starName = 0;
    GetStarName(facet, &starName);
    if (starName) {
	Tcl_AppendResult(interp, starName, (char *) NULL);
	return TCL_OK;
    }
    return POCT_TCL_NIL;
}


///////////////////////////////////////////////////////////////////////////
// The rest of the code in this file is duplicated from the PTcl class
// in $PTOLEMY/src/ptcl/PTcl.cc.
//
// An InterpFuncP is a pointer to an PTcl function that takes an argc-argv
// argument list and returns TCL_OK or TCL_ERROR.

typedef int (POct::*InterpFuncP)(int,char**);

struct InterpTableEntry {
        char* name;
        InterpFuncP func;
};

// Here is the function table and dispatcher function.
// These macros define entries for the table

#define ENTRY(verb) { quote(verb), &POct::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), &POct::action }

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
		Tcl_SetResult(interp,
		       "Internal error in POct::dispatcher!", TCL_STATIC);
		return TCL_ERROR;
	}
	// Make sure kernelCalls.cc will use the right PTcl object.
	PTcl* PTobj = PTcl::findPTcl(interp);
	if (PTobj == 0) {
		Tcl_SetResult(interp,
		       "POct::dispatcher could not find ptcl!",TCL_STATIC);
		return TCL_ERROR;
	}
	if (!VemLock()) {
		Tcl_SetResult(interp,
		       "POct::dispatcher could not lock Vem!", TCL_STATIC);
		return TCL_ERROR;
	}
	// this code makes an effective stack of active Tcl interpreters.
	Tcl_Interp* save = activeInterp;
	activeInterp = interp;
	PTcl* saveptcl = ptcl;
	ptcl = PTobj;
	int i = int(which);
	int status = (obj->*(funcTable[i].func))(argc,argv);
	activeInterp = save;
	ptcl = saveptcl;
	VemUnlock();
	return status;
}
