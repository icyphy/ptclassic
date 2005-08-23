/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCStar.h"
 *                                    created: 9/03/98 15:28:15 
 *                                last update: 13/05/98 9:21:53 
 *  Author: Vincent Legrand, Mathilde Roger, Frédéric Boulanger
 *  E-mail: Frederic.Boulanger@supelec.fr
 *    mail: Supélec - Service Informatique
 *          Plateau de Moulon, F-91192 Gif-sur-Yvette cedex
 *     www: http://wwwsi.supelec.fr/
 *  
 *  Thomson: Xavier Warzee <XAVIER.W.X.WARZEE@tco.thomson.fr>
 *  
 * Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in all
 * copies of this software.
 * 
 * IN NO EVENT SHALL SUPELEC OR THOMSON-CSF OPTRONIQUE BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 * SUPELEC OR THOMSON-CSF OPTRONIQUE HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * SUPELEC AND THOMSON-CSF OPTRONIQUE SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SUPELEC OR THOMSON-CSF
 * OPTRONIQUE HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 *  Description: 
 *  This is the baseclass for synchronous stars that generate C code
 * 
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */
#ifndef _SRCGCStar_h
#define _SRCGCStar_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "SRCGCPortHole.h"
#include "Attribute.h"

class SRCGCTarget;

// These constants are defined in CGCStar, we declare them extern :
// New attribute bit definition
extern const bitWord AB_VARPREC/*  = 0x40*/;
extern const Attribute ANY/* = {0,0}*/;

// New attribute for ports and states of type FIX/FIXARRAY.
// These attributes are ignored for ports/states of other types.

// fixed point precision may change at runtime;
// declare a precision variable holding the actual precision
extern const Attribute A_VARPREC/*   = {AB_VARPREC,0}*/;
// fixed point precision does not change at runtime (the default)
extern const Attribute A_CONSTPREC/* = {0,AB_VARPREC}*/;

class SRCGCStar : public CGStar {
  public:
   // List of all states pointed to in the code.
   // This is public so that SRCGCTarget and other targets can access it.
   StateList referencedStates;

   // initialize method clears out referencedStates list
   void initialize();

   // my domain
   const char* domain() const;

   virtual void tick();

   // run this star or spliceClust if there are any splice stars
   int run();
        
   // Return the status of the reactive flag
   int isItReactive() const { return isReactive; }

   // class identification
   int isA(const char*) const;

   // Generate declarations and initialization code for PortHoles
   StringList declarePortHoles(Attribute a=ANY);
   StringList initCodePortHoles(Attribute a=ANY);

   // Generate the three functions/procedures that manage 
   // the dependencies and the reactivity.
   StringList declareProcedures();

   // Generate declarations and initialization code for States
   StringList declareStates(Attribute a=ANY);
   StringList initCodeStates(Attribute a=ANY);
  
   // Generate declaration, initialization and function codes for
   // command-line arguments
   StringList cmdargStates(Attribute a=ANY);
   StringList cmdargStatesInits(Attribute a=ANY);
   StringList setargStates(Attribute a=ANY);  
   StringList setargStatesHelps(Attribute a=ANY); 

  protected:
   // access to target (cast is safe: always a SRCGCTarget)
   SRCGCTarget* targ() {
     return (SRCGCTarget*)target();
   }

   // Virtual functions. Expand State or PortHole reference macros.
   // If "name" is a state, add it to the list of referenced states.
   StringList expandRef(const char* name);
  
   // Expand State value macros. If "name" state takes a command-
   // line argumanent, add it to the list of referenced states.
   // Else return its value.
   StringList expandVal(const char* name);

   // Add a State to the list of referenced States.
   void registerState(State*);

   // Add lines to be put at the beginning of the code file
   int addInclude(const char* decl);

   // Add options to be used when compiling a C program
   int addCompileOption(const char*);

   // Add options to be used when linking a C program
   int addLinkOption(const char*);

   // Add options to be used when linking a C program on the local machine
   int addLocalLinkOption(const char*);

   // Add options to be used when linking a C program on a remote machine
   int addRemoteLinkOption(const char*);

   // Add a file to be copied over the remote machine
   int addRemoteFile(const char*, int flag = FALSE);

   // Add declarations, to be put at the beginning of the main section
   int addDeclaration(const char* decl, const char* name = NULL) {
     if (!name)
       name = decl;
     return addCode(decl, "mainDecls", name);
   }

   // Add global declarations, to be put ahead of the main section
   int addGlobal(const char* decl, const char* name = NULL) {
     if (!name)
       name = decl;
     return addCode(decl, "globalDecls", name);
   }

   // Add main initializations, to be put at the beginning of the main 
   // section. By giving the name, you can have only one initialization
   // routine among all star instances.
   int addMainInit(const char* decl, const char* name = NULL) {
     return addCode(decl, "mainInit", name);
   }

   // include a module from a C library in the Ptolemy tree
   void addModuleFromLibrary(const char* basename,
                             const char* subdirectory, const char* libraryname);

   // add the fixed-point supporting routines
   void addFixedPointSupport();

   // Mark this star as reactive
   //
   // @Description A reactive star will not produce any present inputs
   // nor change its state if none of its inputs are present.
   // 
   // <P> Usually done within a star's setup() method
   void reactive() { isReactive = TRUE; }

  private:
   // Used for the names of procedures that test the states of their inputs.
   // Replace '.pal.' by '_'.
   StringList shortName();
   
   // Generate declarations for PortHoles and States.
   StringList declareBuffer(const SRCGCPortHole*);
   StringList declareState(const State*);

   //  @ See also StringList declareProcedures();
   StringList declareDep_Inp_Known();
   StringList declareInput_Present();
   StringList declareMake_All_Absent();

   // Generate declaration, initilization and function
   // codes for command-line settable states.
   StringList cmdargState(const State*);
   StringList cmdargStatesInit(const State*);
   StringList setargState(const State*);
   StringList setargStatesHelp(const State*);

   // Generate initialization code for PortHoles and States.
   StringList initCodeBuffer(SRCGCPortHole*);
   StringList initCodeState(const State*);
   
   // Flag indicating whether the star is reactive
   //
   // @Description A reactive star only produces present outputs and
   // updates its statewhen at least one input is present
   int isReactive;
};

#endif
