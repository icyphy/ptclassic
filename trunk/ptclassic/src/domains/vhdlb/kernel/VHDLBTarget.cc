static const char file_id[] = "VHDLBTarget.cc";
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

 This produces and compiles a standalone VHDLB program for a universe.
 Base target for VHDLB code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "HLLTarget.h"
#include "Galaxy.h"
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
// offsetName
/////////////////////////////////////////

StringList VHDLBTarget::offsetName(VHDLBPortHole* p) {
  StringList out = sanitizedFullName(*p);
  out << "_ix";
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
  char IntGal[15] = "InterpGalaxy";
  char intType[10] = "INT";
  char floatType[10] = "FLOAT";
  char anytypeType[10] = "ANYTYPE";
  StringList tempCode = "";
  StringList blockName = "";
  int dataIns = FALSE;
  int dataOuts = FALSE;
  
  /* Temporarily store upper-level code:  ensure bottom-up code gen order */
  tempCode = vhdlCode;
  
  /* If top level, className = "InterpGalaxy", so use universe name instead */
  if(!strcmp(galaxy.className(),IntGal)) {
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
  if(isNewGal == 0) { /* don't repeat galaxy code generation */
    return TRUE;
  }
  else {
    galaxyList << blockName;
  }
  
  vhdlCode = "\n";
  vhdlCode << "entity " << blockName << " is\n";
  
  /* Make a list of the inputs and outputs by name */
  CBlockPortIter galPortIter(galaxy);
  const PortHole* ph;
  StringList phTypeDecl = "";
  StringList inputs = "";
  StringList outputs = "";
  
  while ((ph = galPortIter++) != 0) {
    const PortHole* phAlias;
    if(ph->alias()) {
      phAlias = (const PortHole*) ph->alias();
    }
    else {
      phAlias = ph;
    }
    /* Get to innermost level to find geodesic name */
    const VHDLBPortHole* innermost = (const VHDLBPortHole*) ph;
    while(innermost->alias()) {
      innermost = (VHDLBPortHole*) innermost->alias();
    }
    /* All the alias-chained ports are at the boundary if any are, but only
       the outermost level is not aliased from aother port */
    /* Sometimes outer port is "at boundary" when inner, alias port is not */
    if(!(innermost->atBoundary()) || (ph->aliasFrom())) {
      if(!strcmp(ph->type(), intType)) {
	phTypeDecl = "INTEGER";
      }
      else if(!strcmp(ph->type(), floatType)) {
	phTypeDecl = "REAL";
      }
      else {
	phTypeDecl << sanitize(ph->type());
      }
      if(ph->isItOutput()) {
	/* FIXME: The following line should use the datatype of the portHole */
	if(outputs.numPieces() > 1) outputs << "; ";
	outputs << sanitize(ph->name()) << ": out " << phTypeDecl;
      }
      else {
	/* FIXME: The following line should use the datatype of the portHole */
	if(inputs.numPieces() > 1) inputs << "; ";
	inputs << sanitize(ph->name()) << ": in " << phTypeDecl;
      }
    }
  }

  if(inputs.numPieces() > 1 | outputs.numPieces() > 1) {
    vhdlCode << indent(1) << "port(" << inputs;
    if(inputs.numPieces() > 1 && outputs.numPieces() > 1) vhdlCode << "; ";
    vhdlCode << outputs << ");\n";
  }
  vhdlCode << "end " << blockName << ";\n\n";
  
  StringList signals = "";
  StringList processes = "";
  StringList parts = "";
  StringList components = "";
  
  StringList partName = "";
  StringList partList = "";
  StringList partDecl = "";
  
  StringList componentName = "";
  StringList componentList = "";
  StringList componentDecl = "";
  
  StringList wormProcs = "";
  StringList wormParts = "";

  GalTopBlockIter next(galaxy);
  Block* b;
  while ((b = next++) != 0) {
    if(b->isItAtomic()) {
      VHDLBStar* s = (VHDLBStar*) b;
      if (s->amIFork()) continue;
    }
    
    /* Generate component declaration */
    componentName = (*b).className();
    componentDecl = indent(1);
    componentDecl << "component " << (*b).className();
    
    /* Generate process declaration */
    processes << indent(1) << sanitizedName(*b) << ":" << (*b).className();
    
    /* Generate part declaration */
    partName = (*b).className();
    partDecl = indent(1);
    partDecl << "for all:" << (*b).className();
    partDecl << " use entity work." << (*b).className() << "(";
    partDecl << (*b).className();
    if(!b->isItAtomic()) {
      partDecl << "_structure); end for;\n";
    }
    else {
      partDecl << "_behavior); end for;\n";
    }

    /* check whether the part is included or not. */
    StringListIter partNext(partList);
    const char* ppart;
    
    int isNewPart = 1;
    while ((ppart = partNext++) != 0) {
      if (!strcmp(partName,ppart)) isNewPart = 0;
    }
    
    if(isNewPart == 1) { /* add new part */
      partList << partName;
      parts << partDecl;
    }

    StringList genericDecl = "";
    StringList generics = "";
    StringList portDecl = "";
    StringList ports = "";
    
    /* Find the names of the parameters and their values */
    CBlockStateIter stIter(*b);
    const State* st;
    /* Skip the first state, procId, since we don't use it here */
    st = stIter++;
    while ((st = stIter++) != 0) {
      /* Generic declarations for component declarations */
      if(genericDecl.numPieces() > 1) genericDecl << "; ";
      genericDecl << sanitize(st->name()) << ": ";
      if(!strcmp(st->type(), intType)) {
	genericDecl << "INTEGER";
      }
      else if(!strcmp(st->type(), floatType)) {
	genericDecl << "REAL";
      }
      else {
	genericDecl << sanitize(st->type());
      }
      
      /* Generic maps for process declarations */
      if(generics.numPieces() > 1) generics << ", ";
      generics << sanitize(st->name()) << " => " << st->currentValue();
    }
    if(generics.numPieces() > 1) {
      processes << " generic map(" << generics << ")";
    }
    if(genericDecl.numPieces() > 1) {
      componentDecl << " generic(" << genericDecl << ");";
    }
    
    /* Find the names of the geodesics of all the outputs. */
    CBlockPortIter phIter(*b);
    const VHDLBPortHole* ph;
    while ((ph = (const VHDLBPortHole*) phIter++) != 0) {
      const VHDLBPortHole* terminal = ph;
      /* Get to innermost level to find geodesic name */
      while(terminal->alias()) {
	terminal = (VHDLBPortHole*) terminal->alias();
      }
      /* All the alias-chained ports are at the boundary if any are, but only
	 the outermost level is not aliased from aother port */
      /* Sometimes outer port is "at boundary" when inner, alias port is not */
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

	if(ph->isItInput()) {
	  wormProcs << "dataIn generic map(unique => \"";
	  const char *uniqueName = (const char*) geoName;
	  const int uniqueLenDiff = 30 - strlen(uniqueName);
	  wormProcs << geoName;
	  for(int count = 0; count < uniqueLenDiff; count++) {
	    wormProcs << " ";
	  }
	  wormProcs << "\") port map(output => ";
	  if(ph->isItInput() && (dataIns == FALSE)) dataIns = TRUE;
	}
	if(ph->isItOutput()) {
	  wormProcs << "dataOut generic map(unique => \"";
	  const char *uniqueName = (const char*) geoName;
	  const int uniqueLenDiff = 30 - strlen(uniqueName);
	  wormProcs << geoName;
	  for(int count = 0; count < uniqueLenDiff; count++) {
	    wormProcs << " ";
	  }
	  wormProcs << "\") port map(input => ";
	  if(ph->isItOutput() && (dataOuts == FALSE)) dataOuts = TRUE;
	}
	wormProcs << ph->getGeoReference() << ");\n";
      }
      if((!(outer) && (terminal->atBoundary())) || (ph->isItOutput() && !(ph->aliasFrom()))) {
	StringList geoName;

	geoName = terminal->getGeoReference();

	signals << indent(1) << "signal " << ph->getGeoReference();
	if(!strcmp(ph->type(), intType)) {
	  signals << ": INTEGER;\n";
	}
	else if(!strcmp(ph->type(), floatType)) {
	  signals << ": REAL;\n";
	}
	else if(!strcmp(ph->type(), anytypeType)) {
	  signals << ": REAL;\n";
	}
	else {
	  signals << ": BIT;\n";
	}
      }
      /* Port declarations for component declarations */
      if(portDecl.numPieces() > 1) portDecl << "; ";
      portDecl << sanitize(ph->name()) << ": ";
      if(ph->isItOutput()) {
	portDecl << "out ";
      }
      else {
	portDecl << "in ";
      }
      if(!strcmp(ph->type(), intType)) {
	portDecl << "INTEGER";
      }
      else if(!strcmp(ph->type(), floatType)) {
	portDecl << "REAL";
      }
      else if(!strcmp(ph->type(), anytypeType)) {
	portDecl << "REAL";
      }
      else {
	portDecl << "BIT";
      }
      
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
    if(ports.numPieces() > 1) {
      processes << " port map(" << ports << ")";
    }
    if(portDecl.numPieces() > 1) {
      componentDecl << " port(" << portDecl << ");";
    }
    
    processes << ";\n";
    componentDecl << " end component;\n";
    
    /* check whether the component is included or not. */
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
  if(dataIns == TRUE) {
    componentDecl = indent(1);
    componentDecl << "component dataIn generic(unique: STRING(1 to 30)); port(output: out INTEGER); end component;\n";
    components << componentDecl;
    wormParts << indent(1);
    wormParts << "for all:dataIn use entity work.dataIn(dataIn_behavior); end for;\n";
  }
  if(dataOuts == TRUE) {
    componentDecl = indent(1);
    componentDecl << "component dataOut generic(unique: STRING(1 to 30)); port(input: in INTEGER); end component;\n";
    components << componentDecl;
    wormParts << indent(1);
    wormParts << "for all:dataOut use entity work.dataOut(dataOut_behavior); end for;\n";
  }
  
  /* Generate the architecture definition */
  vhdlCode << "architecture " << blockName << "_structure of " << blockName << " is\n";
  vhdlCode << components;
  vhdlCode << signals;
  vhdlCode << "begin\n";
  vhdlCode << processes;
  vhdlCode << wormProcs;
  vhdlCode << "end " << blockName << "_structure;\n\n";

  /* Generate the configuration definition */
  vhdlCode << "configuration " << blockName << "_parts of " << blockName << " is\n";
  vhdlCode << "for " << blockName << "_structure\n";
  vhdlCode << parts;
  vhdlCode << wormParts;
  vhdlCode << "end for;\n";
  vhdlCode << "end " << blockName << "_parts;\n";
  vhdlCode << "\n";

  /* Put current-level code in front of previous (higher-level) code */
  vhdlCode << tempCode;

  /* Now define each of the galaxies */
  next.reset();
  while ((b = next++) != 0) {
    if(!b->isItAtomic()) galFunctionDef(b->asGalaxy());
  }
  return TRUE;
}

void VHDLBTarget :: setup () {

    Galaxy* gal = galaxy();
    codeGenInit();
    // Initializations
    galId = 0;
    include << "";
    mainDeclarations << "";
    mainInitialization << "";
//    unique = 0;

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
	StringList runCode = include;
	runCode << sectionComment(leader) << mainInitialization << vhdlCode;
	vhdlCode = runCode;

// get the name of the universe to write to that file.vhdl
	StringList codeName;
	StringList univ = gal->name();
	codeName = univ;
	univ << ".vhdl";
	char* codeFileName = writeFileName(univ);
	pt_ofstream codeFile(codeFileName);
	if(!codeFile) return;
	myCode << vhdlCode;
        codeFile << myCode;
        codeFile.flush();

// Display the code
//	Error::message(vhdlCode);
//        writeFile(vhdlCode, codeFileName);

        writeFile(vhdlCode, "", 1);
}

// The following was copied from Target.cc before this method was removed
// Method to set a file name for writing.
// Prepends dirFullName to fileName with "/" between.
// Always returns a pointer to a string in new memory.
// It is up to the user to delete the memory when no longer needed.
// If dirFullName or fileName is NULL then it returns a
// pointer to a new copy of the string "/dev/null".
char* VHDLBTarget :: writeFileName(const char* fileName) {
	if((const char*)destDirectory && *(const char*)destDirectory && fileName && *fileName) {
		StringList fullName = (const char*)destDirectory;
		fullName << "/";
		fullName << fileName;
		return fullName.newCopy();
	}
	return savestring("/dev/null");
}

int VHDLBTarget :: run () {

/*
	system(removeOldFiles);
        system(runOptions);
	system(changeOptions);
*/
    return TRUE;
}

void VHDLBTarget :: wrapup () {
	if(Scheduler::haltRequested()) return ;
	Target::wrapup();
// remove all files created in this run???
}


// copy constructor
VHDLBTarget :: VHDLBTarget (const VHDLBTarget& src) :
HLLTarget(src.name(), "VHDLBStar", src.descriptor())
{ }

// clone
Block* VHDLBTarget :: makeNew () const {
	LOG_NEW; return new VHDLBTarget(*this);
}

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
	// used as the VHDLB variable representing the buffer.
	// That name will
	// be of the form "gal1_gal2_star_output", which designates
	// the output port that actually produces the data.
	//
	// ALERT: FORM OF NAME SHOULD CHANGE ######
	//
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
