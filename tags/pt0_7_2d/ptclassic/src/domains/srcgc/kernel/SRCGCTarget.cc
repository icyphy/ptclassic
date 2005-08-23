/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCTarget.cc"
 *                                    created: 9/03/98 16:40:17 
 *                                last update: 13/05/98 9:21:55 
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
 *  See header file for further information
 * ###################################################################
 */
static const char file_id[] = "SRCGCTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>        // Pick up R_OK for SunOS4.1 cfront
#include <ctype.h>         // Pick up isspace
#include "SRCGCTarget.h"
#include "CGUtilities.h"
#include "SRCGCStar.h"
#include "GalIter.h"
#include "miscFuncs.h"
#include "dataType.h"
#include "EventHorizon.h"
#include "SRCGCRecursiveScheduler.h"
#include "compat.h"        // Pickup symlink()
#include "InfString.h"     // Pick up InfString for use in pragma()

// Defined in SRCGCDomain.cc
extern const char SRCGCdomainName[];

// this modification is necessary to make galaxy code a function
#define MAINDECLS (*getStream("mainDecls"))
#define MAININIT (*getStream("mainInit"))
#define PORTINIT (*getStream("portInit"))
#define MAINLOOP (*getStream("mainLoop"))
#define MAINCLOSE (*getStream("mainClose"))

// Already defined in CGCTarget, declared here as extern
#if 1
extern void prepend(StringList code, CodeStream& stream);
#else
// Add code to the beginning of a CodeStream instead of the end.
void prepend(StringList code, CodeStream& stream) {
  StringList temp;
  temp << code;
  temp << stream;
  stream.initialize();
  stream << temp;
}
#endif

// constructor
SRCGCTarget::SRCGCTarget(const char* name, const char* starclass,
                         const char* desc, const char* assocDomain)
           :HLLTarget(name, starclass, desc, assocDomain) {
  addState(
    staticBuffering.setState("staticBuffering",this,"YES",
                             "static buffering is enforced between portholes.")
  );
  addState(
    funcName.setState("funcName",this,"main",
                      "function name to be created.")
  );
  addState(
    compileCommand.setState("compileCommand",this,"gcc",
                            "command for compiling code.")
  );
  addState(
    compileOptions.setState("compileOptions",this,"",
                            "options to be specified for compiler.")
  );
  addState(
    linkOptions.setState("linkOptions",this,"-lm",
                         "options to be specified for linking.")
  );
  addState(
    resources.setState("resources",this,"STDIO",
                       "standard I/O resource")
  );
  initCodeStrings();

  targetHost.setAttributes(A_SETTABLE);
  filePrefix.setAttributes(A_SETTABLE);
  displayFlag.setAttributes(A_SETTABLE);
  compileFlag.setAttributes(A_SETTABLE);
  runFlag.setAttributes(A_SETTABLE);

  // Exported codeStreams.
  addStream("globalDecls", &globalDecls);
  addStream("include", &include);
  addStream("mainDecls", &mainDecls);
  addStream("mainInit", &mainInit);
  addStream("portInit",&portInit);
  addStream("commInit", &commInit);
  addStream("compileOptions", &compileOptionsStream);
  addStream("linkOptions", &linkOptionsStream); 
  addStream("localLinkOptions", &localLinkOptionsStream); 
  addStream("remoteLinkOptions", &remoteLinkOptionsStream); 
  addStream("remoteFiles", &remoteFilesStream); 
  addStream("remoteCFiles", &remoteCFilesStream); 
  addStream("mainClose", &mainClose);
  addStream("mainLoop",&mainLoop);
}

StringList SRCGCTarget::comment(const char* text, const char* b,
                                const char* e, const char* c) {
  const char* begin = "/* ";
  const char* end = " */";
  const char* cont = "   ";
  if (b) {
    begin = b; 
    end = e;
    cont = c;
  }
  return HLLTarget::comment(text, begin, end, cont);
}

// Galaxy declarations.
void SRCGCTarget::declareGalaxy(Galaxy& galaxy) {
  GalStarIter starIter(galaxy);
  SRCGCStar* star;
  while ((star = (SRCGCStar*)starIter++) != 0) {
    declareStar(star);
  }
}

// Star declarations.
void SRCGCTarget::declareStar(SRCGCStar* star) {
  // States must be declared after the Star has been run because running
  // the Star builds the list of referenced States.

  globalDecls << star->declareStates(A_GLOBAL);
  globalDecls << star->declarePortHoles(P_GLOBAL);
  procedures << star->declareProcedures();
  
  MAINDECLS << star->declareStates(A_LOCAL);
  globalDecls << star->declarePortHoles(P_LOCAL);

  cmdargStruct << star->cmdargStates();
  cmdargStructInit << star->cmdargStatesInits();        
  setargFunc << star->setargStates();       
  setargFuncHelp << star->setargStatesHelps();    
    
  // States must be initialized before they are used, so this code must
  // appear before other code generated by the Star.
  prepend(star->initCodeStates(),MAININIT);
}

void SRCGCTarget :: setup() {
  // Note that code strings are not initialized here since
  // parent target may want to put some code before calling
  // setup() method.
  // Instead, initCodeStrings() is called in frameCode() method
  // after contructing the whole code.

  // member initialize
  galId = 0;
  curId = 0;

  if (galaxy())
    setStarIndices(*galaxy()); 
  LOG_NEW; 
  setSched(new SRCGCRecursiveScheduler);
  HLLTarget::setup();
}

void SRCGCTarget :: initCodeStrings() {
  mainLoop.initialize();
  globalDecls.initialize();
  procedures.initialize();
  include.initialize();
  mainDecls.initialize();
  mainInit.initialize();
  portInit.initialize();
  commInit.initialize();
  wormIn.initialize();
  wormOut.initialize();
  mainClose.initialize();
  cmdargStruct.initialize();  
  cmdargStructInit.initialize();  
  setargFunc.initialize();  
  setargFuncHelp.initialize();  
}

StringList SRCGCTarget::pragma(const char* parentname,
                               const char* blockname) const {
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  const char* value = mappings.lookup(compoundname);
  if (value) {
    StringList ret = "state_name_mapping ";
    ret += value;
    return ret;
  } else {
    return "";
  }
}

StringList SRCGCTarget::pragma (const char* parentname,   
                                const char* blockname,
                                const char* pragmaname) const {
  const char* value = "";
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  if (strcmp(pragmaname, "state_name_mapping") == 0) {
    const char* lookupValue = mappings.lookup(compoundname);
    if (lookupValue)
      value = lookupValue;
  }
  return value;
}

StringList SRCGCTarget::pragma (const char* parentname,   
                                const char* blockname,
                                const char* pragmaname,
                                const char* value) {
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  if (strcmp(pragmaname, "state_name_mapping") == 0) {
    mappings.insert(compoundname, value);
  }
  return "";
}

static char* prototypeDecl =
"\n/* Define macro for prototyping functions on ANSI & non-ANSI compilers */\n"
"#ifndef ARGS\n"
"#if defined(__STDC__) || defined(__cplusplus)\n"
"#define ARGS(args) args\n"
"#else\n"
"#define ARGS(args) ()\n"
"#endif\n"
"#endif\n\n";

static char* trueFalseDecl =
"\n/* Define constants TRUE and FALSE for portability */\n"
"#ifndef TRUE\n"
"#define TRUE (1==1)\n"
"#endif\n"
"#ifndef FALSE\n"
"#define FALSE (1==0)\n"
"#endif\n";

// C Macros definition 
static char* statusDecl =
"\n/* Define constants UNKOWN ABSENT and PRESENT */\n"
"#define SRCGC_UNKNOWN 0\n"
"#define SRCGC_ABSENT -1\n"
"#define SRCGC_PRESENT 1\n";

static char* macroInitDecl =
"\n/* Define macro _SRCGC_INIT for initializing the Ports */\n"
"#define _SRCGC_INIT(nom) nom##_status = SRCGC_UNKNOWN\n";

static char* macroAccessDecl =
"\n/* SRCGC_VALUE and SRCGC_STATUS yield the value and status vars of a signal */\n"
"#define SRCGC_VALUE(nom) nom##_value\n"
"#define SRCGC_STATUS(nom) nom##_status\n";

static char* macroKnownDecl =
"\n/* Define macro KNOWN */\n"
"#define SRCGC_ISKNOWN(nom) (SRCGC_STATUS(nom) != SRCGC_UNKNOWN)\n";

static char* macroPresentDecl =
"\n/* Define macro PRESENT */\n"
"#define SRCGC_ISPRESENT(nom) (SRCGC_STATUS(nom) == SRCGC_PRESENT)\n";

static char* macroAbsentDecl =
"\n/* Define macro ABSENT */\n"
"#define SRCGC_ISABSENT(nom) (SRCGC_STATUS(nom) == SRCGC_ABSENT)\n";

static char* macroMakeAbsentDecl =
"\n/* Define macro MAKEABSENT */\n"
"#define SRCGC_MAKEABSENT(nom) SRCGC_STATUS(nom) = SRCGC_ABSENT\n";

static char* macroGetDecl =
"\n/* Define macro GET */\n"
"#define SRCGC_GET(nom) SRCGC_VALUE(nom)\n";

static char* macroEmitDecl =
"\n/* Define macro EMIT */\n"
"#define SRCGC_EMIT(nom,val)  SRCGC_STATUS(nom) = SRCGC_PRESENT;SRCGC_VALUE(nom) = val\n";


static char* setargHead =
"void set_arg_val(char *arg[]) {\n"
"\tint i;\n"
"\tfor(i = 1; arg[i]; i++) {\n"
"\t\tif((!strcmp(arg[i], \"-help\")) \\\n|| (!strcmp(arg[i], \"-HELP\")) "
"\\\n|| (!strcmp(arg[i], \"-h\"))) {\n"
"\t\t\tprintf(\"Settable states are :\\\n\\n";

// Initial stage of code generation.
void SRCGCTarget::headerCode() {
  include << prototypeDecl;
}

void SRCGCTarget::trailerCode() {
  include << trueFalseDecl
          << statusDecl << macroInitDecl << macroAccessDecl << macroKnownDecl
          << macroPresentDecl << macroAbsentDecl << macroMakeAbsentDecl
          << macroGetDecl << macroEmitDecl;

  if (galaxy() && !SimControl::haltRequested()) {
    declareGalaxy(*galaxy());
    HLLTarget::trailerCode();
  }
}

CodeStream SRCGCTarget::mainLoopBody() {
  CodeStream body;
  defaultStream = &body;
  int iterations = inWormHole() ? -1 : (int)scheduler()->getStopTime();
  beginIteration(iterations,0);
  body << wormIn;
  body << portInit;
  body << mainLoop;
  body << wormOut;
  endIteration(iterations,0);
  defaultStream = &myCode;
  return body;
}   

void SRCGCTarget::mainLoopCode() {
  defaultStream = &mainLoop;
  // It is possible that the star writer has specified items that go
  // in the main loop already to the myCode stream
  if (inWormHole())
    allWormInputCode();
  compileRun((SRCGCRecursiveScheduler*) scheduler());
  if (inWormHole())
    allWormOutputCode();
  defaultStream = &mainClose;
}

// Combine all sections of code.
void SRCGCTarget :: frameCode () {
  // Construct the header.  Include directives appear before any other
  // code.  All global variable declarations must appear before any code
  // (such as procedures) which may use those variables.  Within the
  // main function, declarations must appear before any code.

  StringList functionDeclaration = "int ";
  const char* funcArgs = "(int argc, char *argv[])";
  functionDeclaration << (const char*)funcName << funcArgs;

  myCode << headerComment() << include
         << "\nextern " << (const char*) funcName
         << " ARGS(" << funcArgs << ");\n\n"
         << globalDecls << procedures;

  // If there are command-line settable states in the target,
  // add the supporting code.
  if ((cmdargStruct.length() != 0)) {       
    myCode << "\nstruct {\n" << cmdargStruct      
           << "} arg_store = {" << cmdargStructInit
           << "};\n\n"
           << setargHead << setargFuncHelp
           << "\");\n\t\t\texit(0);\n\t\t}\n"
           << setargFunc << "\t}\n}\n\n";     
  }

  myCode << comment("main function")
         << functionDeclaration << " {\n"
         << mainDecls;
  if ((cmdargStruct.length() != 0))         
    myCode << "set_arg_val(argv);\n";         

  myCode << commInit << mainInit << mainLoopBody()
         << mainClose << "\nreturn 1;\n}\n";

  // after generating code, initialize code strings again.
  initCodeStrings();
}

// if the compilation will occur on a remote machine, then copy
// over any dependent modules to the remote machine
int SRCGCTarget :: processDependentFiles() {
  int retval = TRUE;
  if (!onHostMachine(targetHost)) {
    StringList sourceFiles = getDependentSourceFiles(TRUE);
    if (sourceFiles.length() > 0) {
      retval = rcpCopyMultipleFiles(targetHost, destDirectory,
                                    sourceFiles, FALSE);
    }
  }
  return retval;
}

// write the generated code to a file called <filePrefix>.c
// and copy over and needed files to the remote machine
void SRCGCTarget :: writeCode() {
  writeFile(myCode, ".c", displayFlag);
  processDependentFiles();
}

// compile the file <filePrefix>.c
int SRCGCTarget::compileCode() {
  // invoke the compiler
  StringList cmd, error, file;
  file << filePrefix << ".c ";
  cmd << compileLine(file) << " -o " << filePrefix;
  error << "Could not compile " << file;
  return (systemCall(cmd, error, targetHost) == 0);
}

// return compile command
StringList SRCGCTarget :: compileLine(const char* fName) {
  // Get the compiler and linker arguments with environment
  // variables expanded
  StringList compileArgs = getCompileOptions(TRUE);
  StringList linkArgs = getLinkOptions(TRUE);
  StringList srcFiles = getDependentCFiles(TRUE);
  StringList cmd = (const char*) compileCommand;
  if (compileArgs.length() > 0)
    cmd << " " << compileArgs;
  cmd << " " << fName;
  if (srcFiles.length() > 0)
    cmd << " " << srcFiles;
  if (linkArgs.length() > 0)
    cmd << " " << linkArgs;
  cmd << " ";
  return cmd;
}

// Return a concatenation of all applicable link options, and expand
// the environment variables if expandEnvironmentVars is TRUE.
StringList SRCGCTarget :: getLinkOptions(int expandEnvironmentVars) {
  StringList retLinkOptions = "";
  
  // Link options requested by stars if we're on a local machine
  // such as the ptdsp or SRCGCrtlib libraries
  if (onHostMachine(targetHost)) {
    if (localLinkOptionsStream.length() > 0)
      retLinkOptions << localLinkOptionsStream << " ";
  } else {
    if (remoteLinkOptionsStream.length() > 0)
      retLinkOptions << remoteLinkOptionsStream << " ";
  }

  // Link options requested by the user as a target parameter
  // such as what math library to use
  const char* opts = (const char*) linkOptions;
  if (opts && *opts)
    retLinkOptions << opts << " ";

  // Link options requested by stars
  if (linkOptionsStream.length() > 0)
    retLinkOptions << linkOptionsStream << " ";

  // Conditionally expand environment variables in the link options
  if (expandEnvironmentVars && retLinkOptions.length() > 0) {
    char* allLinkOptions = expandPathName(retLinkOptions);
    retLinkOptions = allLinkOptions;
    delete [] allLinkOptions;
  }

  return retLinkOptions;
}

// Return a concatenation of all applicable compile options, and expand
// the environment variables if expandEnvironmentVars is TRUE.
StringList SRCGCTarget :: getCompileOptions(int expandEnvironmentVars) {
  StringList retCompileOptions = "";
  const char* opts = (const char*) compileOptions;
  if (opts && *opts)
    retCompileOptions << opts << " ";
  if (compileOptionsStream.length() > 0)
    retCompileOptions << compileOptionsStream << " ";

  // Conditionally expand environment variables in the compile options
  if (expandEnvironmentVars && retCompileOptions.length() > 0) {
    char* allCompileOptions = expandPathName(retCompileOptions);
    retCompileOptions = allCompileOptions;
    delete [] allCompileOptions;
  }

  return retCompileOptions;
}

// Return a concatenation of all dependent C files, and expand
// the environment variables if expandEnvironmentVars is TRUE.
StringList SRCGCTarget :: getDependentCFiles(int expandEnvironmentVars) {
  StringList sourceFileList = "";
  sourceFileList << remoteCFilesStream;

  // Conditionally expand environment variables in the compile options
  if (expandEnvironmentVars && sourceFileList.length() > 0) {
    char* allSourceFiles = expandPathName(sourceFileList);
    sourceFileList = allSourceFiles;
    delete [] allSourceFiles;
  }

  return sourceFileList;
}

// Return a concatenation of all dependent C files, and expand
// the environment variables if expandEnvironmentVars is TRUE.
StringList SRCGCTarget :: getDependentSourceFiles(int expandEnvironmentVars) {
  StringList sourceFileList = "";
  sourceFileList << remoteFilesStream;

  // Conditionally expand environment variables in the compile options
  if (expandEnvironmentVars && sourceFileList.length() > 0) {
    char* allSourceFiles = expandPathName(sourceFileList);
    sourceFileList = allSourceFiles;
    delete [] allSourceFiles;
  }

  return sourceFileList;
}

// Run the code.
int SRCGCTarget :: runCode() {
  StringList cmd, error;
  cmd << "./" << filePrefix << "&";
  error << "Could not run " << filePrefix;
  return (systemCall(cmd, error, targetHost) == 0);
}

// Routines for writing code: schedulers may call these
void SRCGCTarget::beginIteration(int repetitions, int depth) {
  *defaultStream << indent(depth);
  if (repetitions == -1) {      // iterate infinitely
    *defaultStream <<  "while(1) {\n";
  } else {
    StringList iterator = symbol("srLoopCounter");
    *defaultStream << "{ int " << iterator << ";" << "for ("
                   << iterator << " = 0; " << iterator << " < "
                   << repetitions << "; " << iterator << "++) {\n";
  }
}

void SRCGCTarget :: wormInputCode(PortHole& p) {
  wormIn << "\t/* READ from wormhole port " 
         << p.fullName() << " */\n";
}

void SRCGCTarget :: wormOutputCode(PortHole& p) {
  wormOut << "\t/* WRITE to wormhole port " 
          << p.fullName() << " */\n";
}

void SRCGCTarget :: endIteration(int repetitions, int depth) {
  if ( repetitions == -1 ) {
    *defaultStream << "} /* end while, depth " << depth << "*/\n";
  } else {
    *defaultStream << "}} /* end repeat, depth " << depth << "*/\n";
  }
}

// clone
Block* SRCGCTarget :: makeNew () const {
  LOG_NEW; return new SRCGCTarget(name(), starType(), descriptor(),
                                  getAssociatedDomain());
}


// codeGenInit
int SRCGCTarget :: codeGenInit() {
  if (! galaxy())
    return FALSE;

  // call initialization code.
  defaultStream = &MAININIT;
  GalStarIter nextStar(*galaxy());
  SRCGCStar* s;
  while ((s = (SRCGCStar*) nextStar++) != 0) {
    // Generate PortHole initialization code before the Star is run
    // because running the Star modifies the PortHole indices.
    PORTINIT << s->initCodePortHoles();
    s->initCode();
  }
  return TRUE;
}

int SRCGCTarget :: allocateMemory() {
  if (! galaxy())
    return FALSE;

  Galaxy& g = *galaxy();
  
  int statBuf = useStaticBuffering();
  const char* loopLevel = (const char*)(loopingLevel);
  if (!(  (strcasecmp(loopLevel,"DEF")==0)
        ||(strcmp(loopLevel,"0")==0)
        ||(strcasecmp(loopLevel,"NO") == 0)
      )) {
    statBuf = 0;
  }

  // (1) determine the buffer size
  GalStarIter nextStar(g);
  SRCGCStar* s;
  while ((s = (SRCGCStar*)nextStar++) != 0) {
    BlockPortIter next(*s);
    SRCGCPortHole* p;
    while ((p = (SRCGCPortHole*) next++) != 0) {
      p->finalBufSize();
    }
  }

  nextStar.reset();
  while ((s = (SRCGCStar*)nextStar++) != 0) {
    BlockPortIter next(*s);
    SRCGCPortHole* p;
    while ((p = (SRCGCPortHole*) next++) != 0) {
      // (2) naming buffers.
      if (p->isItOutput() && (!p->switched())) {
        StringList sym, sName;
        sName << sanitizedName(*p);
        sym << symbol(sName);
        p->setGeoName(sym);
      }

      // (5) initialize offset pointer.
      if (!p->initOffset())
        return FALSE;
    }
  }
  return TRUE;
}

// incrementalAdd
int SRCGCTarget :: incrementalAdd(CGStar* s, int flag) {
  SRCGCStar* cs = (SRCGCStar*) s;
  cs->setTarget(this);

  if (!flag) {
    // run the star
    defaultStream = &MAINLOOP;
    writeFiring(*cs, 1);
    return TRUE;
  }

  // initialize the star
  s->initialize();

  // allocate resource
  BlockPortIter nextPort(*s);
  SRCGCPortHole* p;
  while ((p = (SRCGCPortHole*)nextPort++) != 0) {
    p->finalBufSize();

    // Assign names only for each output port
    if (p->isItOutput() && (p->switched() == 0)) {
      StringList sym, sName;
      sName << sanitizedName(*p);
      sym << symbol(sName);
      p->setGeoName(sym);
    }
  }

  // Generate PortHole initialization code before the Star is run
  // because running the Star modifies the PortHole indices.
  PORTINIT << cs->initCodePortHoles();

  defaultStream = &MAININIT;
  cs->initCode();

  // run the star
  defaultStream = &MAINLOOP;
  writeFiring(*cs, 1);

  defaultStream = &MAINCLOSE;
  cs->wrapup();
  defaultStream = &MAINLOOP;

  declareStar(cs);
  return TRUE;
}
  
// maintain the galaxy id properly.
int SRCGCTarget :: insertGalaxyCode(Galaxy* g, SRCGCRecursiveScheduler* s) {
  int saveId = galId;
  curId++;
  galId = curId;
  setStarIndices(*g);
  if (!insertGalaxyCode2(g, s))
    return FALSE;
  declareGalaxy(*g);
  galId = saveId;
  defaultStream = &MAINLOOP;
  return TRUE;
}

int SRCGCTarget :: insertGalaxyCode2(Galaxy* g, SRCGCRecursiveScheduler* s) {
  // save infos
  Galaxy* saveGal = galaxy(); // method will work fine if NULL
  setGalaxy(*g);
  Target& saveT = s->target();
  s->setTarget(*this);

  if(!allocateMemory() || !codeGenInit())
    return FALSE;
  compileRun(s);
  Target :: wrapup();

  // restore info
  if (saveGal)
    setGalaxy(*saveGal);
  s->setTarget(saveT);
  return TRUE;
}


// redefine compileRun to switch code stream of stars
void SRCGCTarget :: compileRun(SRCGCRecursiveScheduler* s) {
  defaultStream = &MAINLOOP;
  s->compileRun();
  defaultStream = &MAINCLOSE;
}

ISA_FUNC(SRCGCTarget,HLLTarget);
