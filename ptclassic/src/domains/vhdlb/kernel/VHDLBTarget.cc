static const char file_id[] = "VHDLBTarget.cc";
/******************************************************************
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

 Programmer: Edward A. Lee, Michael C. Williamson

 This produces and compiles a standalone VHDLB program for a universe.
 Base target for VHDLB code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLBTarget.h"
#include "CGUtilities.h"
#include "FloatArrayState.h"
#include "VHDLBStar.h"
#include "GalIter.h"
#include "ConstIters.h"
#include "miscFuncs.h"
#include "pt_fstream.h"
#include "Linker.h"
#include "SDFScheduler.h"
#include "PortHole.h"
#include "KnownTarget.h"
#include "FixState.h"
#include "paths.h"
#include <ctype.h>
#include <time.h>
#include "Fix.h"
#include "StringList.h"

/////////////////////////////////////////
// constructor
/////////////////////////////////////////

VHDLBTarget::VHDLBTarget(const char* name,const char* starclass,
			 const char* desc) : HLLTarget(name,starclass,desc) 
{ }

/////////////////////////////////////////
// sectionComment
/////////////////////////////////////////

StringList VHDLBTarget :: sectionComment(const StringList s) {
  StringList out = "\n-- ";
  out << s;
  out << "\n\n";
  return out;
}

/////////////////////////////////////////
// headerCode
/////////////////////////////////////////

void VHDLBTarget :: headerCode () {
  StringList code = "Generated VHDLB code for target ";
  code << fullName();
  StringList vhdlCode = "";
  vhdlCode << sectionComment(code);
}

/////////////////////////////////////////
// galFunctionDef
/////////////////////////////////////////

int VHDLBTarget :: galFunctionDef(Galaxy& galaxy) {
  StringList tempCode = "";
  StringList blockName = "";

  int integerVecs = FALSE;
  int dataIns = FALSE; /* vestigial kludge - strive to remove */
  int dataOuts = FALSE; /* vestigial kludge - strive to remove */
  
  /* Temporarily store upper-level code:  ensure bottom-up code gen order */
  tempCode = vhdlCode;

  /* If className = "InterpGalaxy", then we're at the top level,
     so use universe name instead */
  if(!strcmp(galaxy.className(),"InterpGalaxy")) {
    blockName = galaxy.name();
  }
  else {
    blockName = galaxy.className();
  }

  /* Skip this galaxy if code has already been generated for it */
  StringListIter galaxyNext(galaxyList);
  const char* pgalaxy;
  int isNewGal = 1;
  while ((pgalaxy = galaxyNext++) != 0) {
    if (!strcmp(blockName,pgalaxy)) isNewGal = 0;
  }
  /* don't repeat galaxy code generation */
  if(isNewGal == 0) {
    return TRUE;
  }
  else {
    galaxyList << blockName;
  }


  /* StringLists to be used in the ENTITY DECLARATION */

  StringList genericList = "";
  StringList inputs = "";
  StringList outputs = "";
  
  /* Define the generic interface list, if any */

  /* GALAXY'S STATE SCAN */
  if(galaxy.numberStates()) {
    CBlockStateIter galStateIter(galaxy);
    const State* st;
    while ((st = galStateIter++) != 0) {
      if(genericList.numPieces() > 1) genericList << "; ";
      genericList << sanitize(st->name()) << ": ";
      genericList << translateType(st->type());
    }
  }
  /* END GALAXY'S STATE SCAN */
  

  /* Make a list of the inputs and outputs by name */
  
  /* GALAXY'S SINGLE PORT SCAN */
  if(galaxy.numberPorts()) {
    CBlockPortIter galPortIter(galaxy);
    const PortHole* ph;
    while ((ph = galPortIter++) != 0) {
      /* Check to make sure ph not a part of a MultiPortHole */
      if (!(ph->getMyMultiPortHole())) {
	/* Get to innermost level to find geodesic name */
	const VHDLBPortHole* innerPortHole = (const VHDLBPortHole*) ph;
	while(innerPortHole->alias()) {
	  innerPortHole = (const VHDLBPortHole*) innerPortHole->alias();
	}
	/* All the alias-chained ports are at the boundary if any are, but
	   only the outermost level is not aliased from aother port */
	/* Sometimes outer port is "at boundary" when inner,
	   alias port is not */
	if(!(innerPortHole->atBoundary()) || (ph->aliasFrom())) {
	  StringList tempString = sanitize(ph->name());
	  tempString << ": " << direction(ph) << " ";
	  tempString << translateType(ph->type());
	  if(ph->isItOutput()) {
	    if(outputs.numPieces() > 1) outputs << "; ";
	    outputs << tempString;
	  }
	  else {
	    if(inputs.numPieces() > 1) inputs << "; ";
	    inputs << tempString;
	  }
	}
      }
    }
  }
  /* END GALAXY'S SINGLE PORT SCAN */


  /* GALAXY'S MULTI PORT SCAN */
  if(galaxy.numberMPHs()) {
    integerVecs = TRUE;

    CBlockMPHIter galMPHIter(galaxy);
    const MultiVHDLBPort* mph;
    while ((mph = (const MultiVHDLBPort*) galMPHIter++) != 0) {

      /* Get to innermost level to find portWidthName */
      const MultiVHDLBPort* innerMultiPort = (const MultiVHDLBPort*) mph;
      while(innerMultiPort->alias()) {
	innerMultiPort = (const MultiVHDLBPort*) innerMultiPort->alias();
      }

      VHDLBStar* innerStar = (VHDLBStar*) innerMultiPort->parent();

      StringList tempString = sanitize(mph->name());
      tempString << ": " << direction(mph) << " ";
      tempString << translateType(mph->type()) << "_VECTOR(1 to ";
      tempString << sanitize(mph->name())  << "_";
      tempString << innerStar->portWidthName(innerMultiPort) << ")";

      if(mph->isItOutput()) {
	if(outputs.numPieces() > 1) outputs << "; ";
	outputs << tempString;
       }
      else { /* assume it's input */
	if(inputs.numPieces() > 1) inputs << "; ";
	inputs << tempString;
      }
      if(genericList.numPieces() > 1) genericList << "; ";
      genericList << sanitize(mph->name()) << "_";
      genericList << innerStar->portWidthName(innerMultiPort) << ": ";
      genericList << "INTEGER";
    }
  }
  /* END GALAXY'S MULTI PORT SCAN */


  /* Generate VHDL code for ENTITY DECLARATION */

  vhdlCode = "\n";
  vhdlCode << "entity " << blockName << " is\n";
  if(genericList.numPieces() > 1) {
    vhdlCode << indent(1) << "generic(" << genericList << ");\n";
  }
  if((inputs.numPieces() > 1) || (outputs.numPieces() > 1)) {
    vhdlCode << indent(1) << "port(" << inputs;
    if((inputs.numPieces() > 1) && (outputs.numPieces() > 1)) vhdlCode << "; ";
    vhdlCode << outputs << ");\n";
  }
  vhdlCode << "end " << blockName << ";\n\n";


  /* StringLists to be used in the ARCHITECTURE BODY */

  StringList signals = "";
  StringList processes = "";
  StringList parts = "";
  StringList components = "";

  StringList partList = "";

  StringList componentName = "";
  StringList componentList = "";
  StringList componentDecl = "";

  StringList wormProcs = "";
  StringList wormParts = "";

  /* Top-level block iterator and block pointer to be used both in
     ARCHITECTURE BODY scan and in subordinate galaxy definition */

  GalTopBlockIter nextTopBlock(galaxy);
  Block* b;

  /* Scan the top-level blocks to determine the ARCHITECTURE BODY */

  /* GALAXY/UNIVERSE'S TOP-LEVEL BLOCK SCAN */
  if(galaxy.numberBlocks()) {
    while ((b = nextTopBlock++) != 0) {
      if(b->isItAtomic()) {
	VHDLBStar* s = (VHDLBStar*) b;
	if (s->amIFork()) continue;
      }
      
      /* Generate component declaration */
      componentName = (*b).className();
      componentDecl = indent(1);
      componentDecl << "component " << componentName;
      
      /* Generate process declaration */
      processes << indent(1) << sanitizedName(*b) << ":" << componentName;
      
      /* Generate part declaration only if it hasn't been done already */
      StringListIter partNext(partList);
      const char* ppart;
      int isNewPart = 1;
      while ((ppart = partNext++) != 0) {
	if (!strcmp(componentName,ppart)) isNewPart = 0;
      }
      
      if(isNewPart == 1) { /* add new part */
	partList << componentName;
	parts << indent(1) << "for all:" << componentName;
	parts << " use entity work." << componentName << "(" << componentName;
	if(!b->isItAtomic()) {
	  parts << "_structure); end for;\n";
	}
	else {
	  parts << "_behavior); end for;\n";
	}
      }
      
      StringList genericDecl = "";
      StringList generics = "";
      StringList portDecl = "";
      StringList ports = "";
      

      /* BLOCK'S STATE SCAN */
      if(b->numberStates()) {
	CBlockStateIter stIter(*b);
	const State* st;
	while ((st = stIter++) != 0) {
	  /* Skip the state "procId" if present since we don't use it */
	  if(strcmp(st->name(), "procId")) {
	    /* Generic declarations for component declarations */
	    if(genericDecl.numPieces() > 1) genericDecl << "; ";
	    genericDecl << sanitize(st->name()) << ": ";
	    genericDecl << translateType(st->type());
	    /* Generic maps for process declarations */
	    if(generics.numPieces() > 1) generics << ", ";
	    generics << sanitize(st->name()) << " => " << st->currentValue();
	  }
	}
      }
      /* END BLOCK'S STATE SCAN */
      
      
      /* BLOCK'S SINGLE PORT SCAN */
      if(b->numberPorts()) {
	CBlockPortIter phIter(*b);
	const VHDLBPortHole* ph;
	while ((ph = (const VHDLBPortHole*) phIter++) != 0) {
	  /* Check to make sure ph not a part of a MultiPortHole */
	  if (!(ph->getMyMultiPortHole())) {
	    /* Get to innermost level to find geodesic name */
	    const VHDLBPortHole* terminal = ph;
	    while(terminal->alias()) {
	      terminal = (VHDLBPortHole*) terminal->alias();
	    }
	    /* All the alias-chained ports are at the boundary if any are,
	       but only the outermost level is not aliased from another port.
	       Sometimes outer port is "at boundary" when inner,
	       alias port is not */
	    const GenericPort* outer = ph->aliasFrom();
	    if(outer) {
	      outer = outer->aliasFrom();
	    }
	    else {
	      outer = (const GenericPort*) ph;
	    }
	    if((terminal->atBoundary()) && !(outer)) {
	      StringList geoName;
	      geoName = ((const VHDLBPortHole*) (ph->aliasFrom()))->getGeoReference();
	      wormProcs << indent(1) << "Worm" << geoName << ": ";
	      
	      const char *uniqueName = (const char*) geoName;
	      const int uniqueLenDiff = 30 - strlen(uniqueName);
	      StringList tempCode = geoName;
	      for(int count = 0; count < uniqueLenDiff; count++) {
		tempCode << " ";
	      }
	      if(ph->isItInput()) {
		dataIns = TRUE;
		wormProcs << "dataIn generic map(unique => \"";
		wormProcs << tempCode;
		wormProcs << "\") port map(output => ";
	      }
	      if(ph->isItOutput()) {
		dataOuts = TRUE;
		wormProcs << "dataOut generic map(unique => \"";
		wormProcs << tempCode;
		wormProcs << "\") port map(input => ";
	      }
	      wormProcs << ph->getGeoReference() << ");\n";
	    }
	    if((!(outer) && (terminal->atBoundary())) || (ph->isItOutput() && !(ph->aliasFrom()))) {
	      signals << indent(1) << "signal " << ph->getGeoReference();
	      signals << ": " << translateType(ph->type()) << ";\n";
	    }

	    /* Port declarations for component declarations */
	    if(portDecl.numPieces() > 1) portDecl << "; ";
	    portDecl << sanitize(ph->name()) << ": " << direction(ph);
	    portDecl << " " << translateType(ph->type());
	    
	    /* Port maps for process declarations */
	    if(ports.numPieces() > 1) ports << ", ";
	    ports << sanitize(ph->name()) << " => ";
	    /* Eliminate '@' chars due to delays; we ignore delays for now */
	    StringList dirtyList = ph->getGeoReference();
	    dirtyList << '\0';
	    const char* dirty = (const char*) dirtyList;
	    while(*dirty && (*dirty != '@')) {
	      ports << *dirty;
	      dirty++;
	    }
	  }
	}
      }
      /* END BLOCK'S SINGLE PORT SCAN */
      
      
      /* BLOCK'S MULTI PORT SCAN */
      if(b->numberMPHs()) {
	integerVecs = TRUE;

	BlockMPHIter mphIter(*b);
	MultiPortHole* mph;
	while ((mph = (MultiPortHole*) mphIter++) != 0) {
	  /* Generate signals ONLY for output ports (unique naming) */
	  if(mph->isItOutput()) {
	    MPHIter portsIter(*mph);
	    const VHDLBPortHole* mpPort;
	    int portNum = 0;
	    while ((mpPort = (const VHDLBPortHole*) portsIter++) != 0) {
	      portNum++;
	      signals << indent(1) << "signal " << mpPort->getGeoReference();
	      signals << ": " << translateType(mpPort->type()) << ";\n";
	    }
	  }

	  /* Get to innermost level to find port width name */
	  MultiVHDLBPort* innerMultiPort = (MultiVHDLBPort*) mph;
	  while(innerMultiPort->alias()) {
	    innerMultiPort = (MultiVHDLBPort*) innerMultiPort->alias();
	  }
	  
	  /* Port declarations for component declarations */
	  if(portDecl.numPieces() > 1) portDecl << "; ";
	  portDecl << sanitize(mph->name()) << ": ";
	  portDecl << direction((GenericPort*) mph) << " ";
	  portDecl << translateType(mph->type()) << "_VECTOR(1 to ";
	  /* If mph has an alias, then distinguish its portWidthName
	     with a unique prefix in case its parent is reused at this
	     level; else just use the portWidthName by itself */
	  if(mph->alias()) { /* not at bottom of alias chain */
	    portDecl << sanitize(mph->name()) << "_";
	  }
	  VHDLBStar* innerStar = (VHDLBStar*) innerMultiPort->parent();
	  portDecl << innerStar->portWidthName(innerMultiPort) << ")";

	  /* Port maps for process declarations */

	  /* If mph not aliased from another galaxy's port, then map
	     the individual portHoles to their signals;
	     But if we're at any level below that, map the whole
	     multiPortHole to the containing galaxy's multiPortHole */
	  if(!(mph->aliasFrom())) { /* at top of alias chain */
	    MPHIter innerPortsIter(*innerMultiPort);
	    int portNum = 0;
	    const VHDLBPortHole* mpPort;
	    while ((mpPort = (const VHDLBPortHole*) innerPortsIter++) != 0) {
	      portNum++;
	      if(ports.numPieces() > 1) ports << ", ";
	      ports << sanitize(mph->name()) << "(" << portNum << ") => ";
	      ports << mpPort->getGeoReference();
	    }
	  }
	  else { /* not at top of alias chain */
	    if(ports.numPieces() > 1) ports << ", ";
	    ports << sanitize(mph->name()) << " => ";
	    /* Analogous to what getGeoReference does for
	       single portHoles which are aliased from a terminal;
	       Could expand getGeoReference to do multiportholes also */
	    ports << sanitize(mph->aliasFrom()->name());
	  }

	  /* Generic declarations for component declarations */

	  /* If mph has an alias, then distinguish its portWidthName
	     with a unique prefix in case its parent is reused at this
	     level; else just use the portWidthName by itself */
	  if(genericDecl.numPieces() > 1) genericDecl << "; ";
	  if(mph->alias()) { /* not at bottom of alias chain */
	    genericDecl << sanitize(mph->name()) << "_";
	  }
	  genericDecl << innerStar->portWidthName(innerMultiPort) << ": ";
	  genericDecl << "INTEGER";
	  
	  /* Generic maps for process declarations */

	  /* If mph has an alias, then distinguish its portWidthName
	     with a unique prefix in case its parent is reused at this
	     level; else just use the portWidthName by itself */
	  if(generics.numPieces() > 1) generics << ", ";
	  if(mph->alias()) { /* not at bottom of alias chain */
	    generics << sanitize(mph->name()) << "_";
	  }
	  generics << innerStar->portWidthName(innerMultiPort) << " => ";

	  /* If mph not aliased from another galaxy's port, then use
	     numberPorts, determined by this mph's connections;
	     But if we're at any level below that, use the portWidthName,
	     which comes from the innerMultiPort star's portWidthName */
	  if(!(mph->aliasFrom())) { /* at top of alias chain */
	    generics << innerMultiPort->numberPorts();
	  }
	  else { /* not at top of alias chain */
	    generics << sanitize(mph->aliasFrom()->name()) << "_";
	    generics << innerStar->portWidthName(innerMultiPort);
	  }
	}
      }
      /* END BLOCK'S MULTI PORT SCAN */
      
      
      /* Add the generics and ports to both the
	 component declarations and process declarations */
      
      if(genericDecl.numPieces() > 1) {
	componentDecl << " generic(" << genericDecl << ");";
      }
      if(generics.numPieces() > 1) {
	processes << " generic map(" << generics << ")";
      }
      
      if(portDecl.numPieces() > 1) {
	componentDecl << " port(" << portDecl << ");";
      }
      if(ports.numPieces() > 1) {
	processes << " port map(" << ports << ")";
      }
      
      componentDecl << " end component;\n";
      processes << ";\n";
      
      
      /* Add component to component list and component declaration to
	 component declaration code if it is newly declared */
      StringListIter compNext(componentList);
      const char* pcomp;
      
      int isNewComp = 1;
      while ((pcomp = compNext++) != 0) {
	if (!strcmp(componentName,pcomp)) isNewComp = 0;
      }
      if(isNewComp == 1) { /* add new component */
	componentList << componentName;
	components << componentDecl;
      }
    }
  }
  
  /* Vestigial code for wormhole stars */
  if(dataIns == TRUE) {
    components << indent(1);
    components << "component dataIn generic(unique: STRING(1 to 30)); port(output: out INTEGER); end component;\n";
    wormParts << indent(1);
    wormParts << "for all:dataIn use entity work.dataIn(dataIn_behavior); end for;\n";
  }

  if(dataOuts == TRUE) {
    components << indent(1);
    components << "component dataOut generic(unique: STRING(1 to 30)); port(input: in INTEGER); end component;\n";
    wormParts << indent(1);
    wormParts << "for all:dataOut use entity work.dataOut(dataOut_behavior); end for;\n";
  }


  /* Generate VHDL code for ARCHITECTURE BODY */

  vhdlCode << "architecture " << blockName << "_structure of ";
  vhdlCode << blockName << " is" << "\n";
  vhdlCode << components;
  vhdlCode << signals;
  vhdlCode << "begin" << "\n";
  vhdlCode << processes;
  vhdlCode << wormProcs;
  vhdlCode << "end " << blockName << "_structure;" << "\n\n";

  /* Generate VHDL code for CONFIGURATION DECLARATION */

  vhdlCode << "configuration " << blockName << "_parts of ";
  vhdlCode << blockName << " is" << "\n";
  vhdlCode << "for " << blockName << "_structure" << "\n";
  vhdlCode << parts;
  vhdlCode << wormParts;
  vhdlCode << "end for;" << "\n";
  vhdlCode << "end " << blockName << "_parts;" << "\n\n";


  /* Put current-level code in front of previous (higher-level) code */

  /* Put use-clause in front of galaxy code if needed */
  StringList galaxyCode = "";
  if(integerVecs) {
    galaxyCode = "use work.datatypes.all;\n";
  }
  galaxyCode << vhdlCode;

  vhdlCode = galaxyCode;
  vhdlCode << tempCode;


  /* Now define each of the subordinate galaxies */

  nextTopBlock.reset();
  while ((b = nextTopBlock++) != 0) {
    if(!b->isItAtomic()) galFunctionDef(b->asGalaxy());
  }
  return TRUE;
}

/////////////////////////////////////////
// setup
/////////////////////////////////////////

void VHDLBTarget :: setup () {

  Galaxy* gal = galaxy();
  codeGenInit();

  // Initializations
  galId = 0;
  include << "";
  mainDeclarations << "";
  mainInitialization << "";

  galaxyList.initialize();
  includeFiles.initialize();
  globalDecls.initialize();

  StringList galaxyList = "";
  galaxyList.initialize();
  vhdlCode.initialize();

  CGTarget::setup();

  // to remove pre-existing output files
  removeOldFiles = "";
  changeOptions  = "";
  changeOptions << "cd " << expandPathName(destDirectory) << "; ";

  // Data structure declaration
  StringList leader = "Code from Universe: ";
  leader << gal->fullName();
  galFunctionDef(*gal);

  // Assemble all the code segments
  StringList runCode = "";
  runCode << sectionComment(leader) << mainInitialization << vhdlCode;
  vhdlCode = runCode;

  // Display the code and write it to a file
  writeFile(vhdlCode, ".vhdl", 1);
}

/////////////////////////////////////////
// run
/////////////////////////////////////////

int VHDLBTarget :: run () {
  return TRUE;
}

/////////////////////////////////////////
// wrapup
/////////////////////////////////////////

void VHDLBTarget :: wrapup () {
  if(Scheduler::haltRequested()) return ;
  Target::wrapup();
}

/////////////////////////////////////////
// copy constructor:  VHDLBTarget
/////////////////////////////////////////

VHDLBTarget :: VHDLBTarget (const VHDLBTarget& src) :
HLLTarget(src.name(), "VHDLBStar", src.descriptor())
{ }

/////////////////////////////////////////
// clone:  makeNew
/////////////////////////////////////////

Block* VHDLBTarget :: makeNew () const {
  LOG_NEW; return new VHDLBTarget(*this);
}

/////////////////////////////////////////
// setGeoNames
/////////////////////////////////////////

void VHDLBTarget :: setGeoNames(Galaxy& galaxy) {

  // Assign names for each geodesic according to port connections.
  // Output port names take priority, so we do the inputs first,
  // and let output names overwrite input names if necessary.
  // The galaxy name will only be used at the top level of the
  // aliases, so we can greatly shorten the names by resolving
  // the aliases upward.

  GalStarIter nextIns(galaxy);
  GalStarIter nextOuts(galaxy);
  Star* b;
  while ((b = nextIns++) != 0) {
    BlockPortIter nextPort(*b);
    VHDLBPortHole* p;
    GenericPort *gp, *gn;
    while ((p = (VHDLBPortHole*)nextPort++) != 0) {
      if ((p->isItInput())) {
	gn = p;
	while(gp = gn->aliasFrom()) gn = gp;
	StringList s = sanitizedShortName(*gn);
	p->setGeoName(savestring(s));
      }
    }
  }
  while ((b = nextOuts++) != 0) {
    BlockPortIter nextPort(*b);
    VHDLBPortHole* p;
    GenericPort *gp, *gn;
    while ((p = (VHDLBPortHole*)nextPort++) != 0) {
      if ((p->isItOutput())) {
	gn = p;
	while(gp = gn->aliasFrom()) gn = gp;
	StringList s = sanitizedShortName(*gn);
	p->setGeoName(savestring(s));
      }
    }
  }
}

/////////////////////////////////////////
// sanitizedFullName
/////////////////////////////////////////

StringList VHDLBTarget :: sanitizedFullName (const NamedObj& obj) const {
  StringList out;
  if(obj.parent() != NULL && obj.parent()->parent() != NULL) {
    out = sanitizedFullName(*obj.parent());
    out << "_";
    out << sanitizedName(obj);
  } else {
    out = sanitizedName(obj);
  }
  return out;
}

/////////////////////////////////////////
// sanitizedShortName
/////////////////////////////////////////

StringList VHDLBTarget :: sanitizedShortName (const NamedObj& obj) const {
  StringList out;
  if(obj.parent() != NULL)
    out = sanitizedName(*obj.parent());
  else
    out = "XX";
  out << "_";
  out << sanitizedName(obj);
  return out;
}

/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int VHDLBTarget :: codeGenInit() {

  // Set all geodesics to contain a symbolic name that can be
  // used as the VHDLB variable representing the buffer.  That
  // name will be of the form "gal1_gal2_star_output", which
  // designates the output port that actually produces the data.

  setGeoNames(*galaxy());
  GalStarIter nextStar(*galaxy());
  nextStar.reset();
  VHDLBStar* s;
  while ((s = (VHDLBStar*) nextStar++) != 0) {
    if (s->amIFork()) continue;
    s->initCode();
  }
  return TRUE;
}

/////////////////////////////////////////
// translateType
/////////////////////////////////////////

StringList VHDLBTarget :: translateType (const char* type) {
  StringList newType;
  if(!strcmp(type, "INT")) {
    newType = "INTEGER";
  }
  else if(!strcmp(type, "FLOAT")) {
    newType = "REAL";
  }
  else if(!strcmp(type, "ANYTYPE")) {
    newType = "REAL";
  }
  else {
    newType = sanitize(type);
  }
  return newType;
}

/////////////////////////////////////////
// direction
/////////////////////////////////////////

StringList VHDLBTarget :: direction (const GenericPort* port) {
  StringList direct;
  if(port->isItInput()) {
    direct = "in";
  }
  else if(port->isItOutput()) {
    direct = "out";
  }
  else {
    direct = "inout";
  }
  return direct;
}
