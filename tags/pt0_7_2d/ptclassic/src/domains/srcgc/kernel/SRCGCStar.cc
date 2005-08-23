/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCStar.cc"
 *                                    created: 17/03/98 18:13:10 
 *                                last update: 13/05/98 9:21:51 
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
static const char file_id[] = "SRCGCStar.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCTarget.h"    // To pick up the pragma method in SRCGCTarget.
#include "SRCGCStar.h"
#include "SRCGCPortHole.h"
#include "SRCGCGeodesic.h"
#include "CGUtilities.h"
#include <ctype.h>

// The following is defined in SRCGCDomain.cc -- this forces that module
// to be included if any SRCGC stars are linked in.
extern const char SRCGCdomainName[];

const char* SRCGCStar :: domain () const { return SRCGCdomainName;}

// isA
ISA_FUNC(SRCGCStar, CGStar);

// This function checks whether "state" is to be set from a command-line
// argument. If it is, returns the name to be specified on the command-
// line. The function returns "" otherwise.

// cmdArg and isCmdArg are defined in CGCStar, we declare them extern
#if 1
extern StringList cmdArg(const State* state);
extern int isCmdArg (const State* state);
#else
StringList cmdArg(const State* state) {
  StringList temp;
  // Check whether the block has a NULL parent
  // This is necessary for CGDDF
  if (!state->parent()->parent())
    return "";
  StringList mapsList = state->parent()->target()->\
    pragma(state->parent()->parent()->name(),
           state->parent()->name(),
           "state_name_mapping");
  const char* maps = (const char*)mapsList;
  maps = strstr(maps, state->name());

  if (maps) {
    while((!isspace(*maps)) && (*maps != '\0')) {
      maps++;
    }

    if (*maps != '\0') {
      while((isspace(*maps)) && (*maps != '\0')) {
        maps++;
      }
    }

    if (*maps != '\0') {
      while ((!isspace(*maps)) && (*maps != '\0')) {
        temp << *maps++;
      }
    }
  }
  return temp;
}
    
int isCmdArg (const State* state) {
  StringList pragma;
  pragma << cmdArg(state);
  return pragma.length();
}
#endif  

// Expand State value macros. If "name" state can be set via command-
// line argument, then change it to a reference and add it to the list
// of referneced states.
StringList SRCGCStar::expandVal(const char *name) {           
  StringList ref;       
  State* state;         
  
  if ((state = stateWithName(name)) != NULL) {
    if (isCmdArg(state)) {          
      registerState(state);
      ref << starSymbol.lookup(state->name());
    }             
    else {            
      ref << CGStar::expandVal(name);   
    }
  } else {              
    codeblockError(name, " is not defined as a state");
    ref.initialize();           
  }
  return ref;             
}


// Reference to State or PortHole.
StringList SRCGCStar::expandRef(const char* name) {
  StringList ref;
  GenericPort* genPort;
  SRCGCPortHole* port;
  State* state;
  StringList portName = expandPortName(name);

  // Expand State reference.
  if ((state = stateWithName(name)) != NULL) {
    registerState(state);
    ref << starSymbol.lookup(state->name());
  } else if (((genPort = genPortWithName(portName)) != NULL) 
             && genPort->isItMulti()){
    // Expand PortHole reference.
    codeblockError(name, " is a multihole name, not an actual "
                   "port name.  Ports that correspond to a "
                   "multihole need to be referenced with "
                   "<name>#<port number> syntax.");
  } else if (genPort) {
    port = (SRCGCPortHole*)genPort;
    ref << port->getGeoName();
  } else {   // Could not find State or PortHole with given name.
    codeblockError(name, " is not defined as a state or port");
    ref.initialize();
  }
  return ref;
}

void SRCGCStar::registerState(State* state) {
  // If the State is not already on the list of referenced States, add it.
  StateListIter stateIter(referencedStates);
  State* s;
  while ((s = stateIter++) != NULL) {
    if (s == state)
      return;
  }
  referencedStates.put(*state);
}

void SRCGCStar::initialize() {
  isReactive = FALSE;
  CGStar::initialize();
  referencedStates.initialize();
}


// Advance a star's state at the end of an instant
//
// @Description Default does nothing.  Derived stars should override
// this with a method that advances a star's state based on its
// inputs and computed outputs.  Schedulers call this exactly once at
// the end of each instant for every star.  This may not be called
// for reactive stars in instants where they have no present inputs. <P>
//
// Zero-input reactive stars always fire.
void SRCGCStar::tick() {
}


/////////////////////////////////////////////////////////////
// run: 
//  (1) prefix the code with a comment
//  (2) body of code
/////////////////////////////////////////////////////////////
int SRCGCStar::run() {
  int numInputs = 0;
  int numOutputs = 0;
  int dependentInputs = FALSE; 
  BlockPortIter nextPort(*((Block *) this) );
  PortHole * p;

  while ( ( p = nextPort++) != 0 ) {
    if ( p->isItInput() ) {
      numInputs++;
      if ( !((InSRCGCPort *) p)->isItIndependent()) {
        dependentInputs = TRUE;
      }
    } else {
      numOutputs++;
    }
  }

  StringList code = "\t{  /* star ";
  code << fullName() << " (class " << className() << ") */\n";
  
  if (dependentInputs) {
    code << "\tif (_" << shortName() << "_Dep_Inp_Known() ){\n";
  }
  if (isReactive && numInputs != 0) {
    code << "\tif (_" << shortName() << "_Input_Present() ){\n";
  }
  addCode(code);
  code = "";
  
  int status = Star::run();

  if (isReactive && numInputs != 0) {
    code << "\t}";
  }

  if (isReactive && numInputs != 0 && numOutputs !=0) {
    code << " else {\n";
    code << "\t_" << shortName() << "_Make_All_Absent();\n\t}\n";
  }
  if (dependentInputs) {
    code << "\t}\n";
  }
  addCode(code);
  addCode("\t}\n");
  return status;
}

// Used for the names of procedures that test the states of their inputs.
// Work out a legal C name from fullName().
static const char * toks = ".+-*/ #"; // chars to replace by '_' in C names
StringList SRCGCStar::shortName() {
  char * name;
  StringList result;

  name = fullName().newCopy(); // Get a fresh, modifiable copy for strtok

  if (!isA("SRCGCCSynchComm")) {
    char * p = name;
    p = strtok(name,toks);
    while (p != 0) {
      if (strcmp(p,"pal") == 0) { // skip ".pal" in full name
        p = strtok(0,toks);
      } else {
        result << p;
        p = strtok(0,toks);
        if (p != 0) {
          result << '_';
        }
      }
    }
  } else {
    result << name;
  }
  delete[] name;  // Get rid of strtok's scratchpad
  return result;
}

// Already defined in CGCStar, declared here as extern
#if 1
extern int operator==(bitWord b, Attribute a);
#else
// Compare.
int operator==(bitWord b, Attribute a) {
  return b == a.eval(b);
}
#endif

// Declare PortHole data structures.
StringList SRCGCStar::declarePortHoles(Attribute a) {
  StringList dec; // declarations

  SRCGCPortHole* port;
  BlockPortIter portIter(*this);
  while ((port = (SRCGCPortHole*)portIter++) != NULL) {
    if (port->attributes() == a) {
      dec << declareBuffer(port);
    }
  }
  return dec;
}

// Generate initialization code for PortHole data structures.
StringList SRCGCStar::initCodePortHoles(Attribute a) {
  StringList code;  // initialization code

  SRCGCPortHole* port;
  BlockPortIter portIter(*this);
  while ((port = (SRCGCPortHole*)portIter++) != NULL) {
    if (port->attributes() == a) {
      code << initCodeBuffer(port);
    }
  }
  return code;
}

// Declare State data structures.
StringList SRCGCStar::declareStates(Attribute a) {
  StringList dec; // declarations

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      dec << declareState(state);
    }
  }
  return dec;
}

// Declare 'struct' for states settable via command-line arguments
StringList SRCGCStar::cmdargStates(Attribute a) {
  StringList struct_mem;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      struct_mem << cmdargState(state);
    }
  }
  return struct_mem;
}

// Initialize the 'struct' declared above to the default values.
StringList SRCGCStar::cmdargStatesInits(Attribute a) {
  StringList struct_init;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      struct_init << cmdargStatesInit(state);
    }
  }
  return struct_init;
}

// Define the function codes to set the appropriate 'struct'
// member to that specified on the command-line.
StringList SRCGCStar::setargStates(Attribute a)    {
  StringList setarg_proc;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      setarg_proc << setargState(state);
    }
  }
  return setarg_proc;
}

// Define the '-help' option by adding each settable states
// and their default values.
StringList SRCGCStar::setargStatesHelps(Attribute a) {
  StringList arg_help;

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      arg_help << setargStatesHelp(state);
    }
  }
  return arg_help;
}

// Generate initialization code for State data structures.
StringList SRCGCStar::initCodeStates(Attribute a) {
  StringList code;  // initialization code

  State* state;
  StateListIter stateIter(referencedStates);
  while ((state = stateIter++) != NULL) {
    if (state->attributes() == a) {
      code << initCodeState(state);
    }
  }
  return code;
}

// Generate the three functions/procedures that manage the dependencies and the reactivity.
StringList SRCGCStar::declareProcedures() {
  StringList proc;
    
  if ( !isA("SRCGCNonStrictStar") ) {
    proc << declareDep_Inp_Known();
    proc << declareInput_Present();
    proc << declareMake_All_Absent();
  }
  return proc;
}

StringList SRCGCStar::declareDep_Inp_Known() {
  StringList proc;
    
  BlockPortIter nextPort(*((Block *) this) );
  PortHole * p;
  int dependentInputs = FALSE;
    
  while ( ( p = nextPort++) != 0 ) {
    if ( p->isItInput() ) {
      if ( !((InSRCGCPort *) p)->isItIndependent()) {
        if (dependentInputs == TRUE) {
          proc << "&& \n\t";
        } else {
          dependentInputs = TRUE;
          proc << "int _" << shortName() << "_Dep_Inp_Known() {\n"
               << "\treturn (";
        }
        proc << "(SRCGC_ISKNOWN(" << ((SRCGCPortHole *) p)->getGeoName() << ")) ";
      }
    }
  }
  if (dependentInputs == TRUE) {
    proc << ");\n}\n";
  }
  return proc;
}

StringList SRCGCStar::declareInput_Present() {
  StringList proc;
    
  BlockPortIter nextPort(*((Block *) this) );
  PortHole * p;
  int Inputs = FALSE;

  if (isReactive == 1) {
    while ( ( p = nextPort++) != 0 ) {
      if ( p->isItInput() ) {
        if (Inputs == TRUE) {
          proc << "|| \n\t";
        } else {
          Inputs = TRUE;
          proc << "int _" << shortName() << "_Input_Present() {\n"
               << "\treturn (";
        }
        proc << "(SRCGC_ISPRESENT(" << ((SRCGCPortHole *) p)->getGeoName() << ") ) ";
      }
    }
    if (Inputs == TRUE) {
      proc << ");\n}\n";
    }
  }
  return proc;
}


StringList SRCGCStar::declareMake_All_Absent() {
  StringList proc;
    
  BlockPortIter nextPort(*((Block *) this) );
  PortHole * p;
  int Outputs = FALSE;

  if (isReactive == 1) {
    while ( ( p = nextPort++) != 0 ) {
      if ( p->isItOutput() ) {
        if (Outputs == TRUE) {
          proc << ";\n\t";
        } else {
          Outputs = TRUE;
          proc << "int _" << shortName() << "_Make_All_Absent() {\n"
               << "\t";
        }
        proc << "SRCGC_MAKEABSENT(" << ((SRCGCPortHole *) p)->getGeoName() << ")";
      }
    }
    if (Outputs == TRUE) {
      proc << ";\n}\n";
    }
  }
  return proc;
}


// Declare PortHole buffer.
StringList SRCGCStar::declareBuffer(const SRCGCPortHole* port) {
  StringList dec; // declarations

  if (!port->switched() && port->isItOutput()) {
    StringList name = port->getGeoName();
    DataType type = port->resolvedType();
        
    dec << "int ";
    dec << name;        
    dec << "_status;\n";
    if (type == INT) {
      dec << "int ";
    } else if (type == STRING) {
      dec << "char *" ;
    } else {
      dec << "double";
    }
    dec << name;        
    dec << "_value;\n";
  }
  return dec;
}

// Generate initialization code for PortHole buffer.
StringList SRCGCStar::initCodeBuffer(SRCGCPortHole* port) {
  StringList code;

  if (!port->switched() && port->isItOutput()) {
    StringList name = port->getGeoName();
  
    code << "_SRCGC_INIT(" << name << ");\n";
  }
  return code;
}


// Declare State variable.
StringList SRCGCStar::declareState(const State* state) {
  StringList dec;

  if (state->isA("IntState"))
    dec << "int";
  else if (state->isA("StringState"))
    dec << "char*";
  else
    dec << "double";

  const char* name = ptSanitize(starSymbol.lookup(state->name()));
  dec << " " << name;

  // Strings cant just be declared, they have to be initialized as well
  if (state->isA("StringState"))
    dec << '=' << '"' << state->currentValue() << '"';

  dec << ";\n";
  return dec;
}

// Declare the state variable as a 'struct' member.
StringList SRCGCStar::cmdargState(const State* state) {
  StringList struct_mem;
  if (isCmdArg(state)) {
    if (state->isA("IntState"))
      struct_mem << "\tint";
    else
      struct_mem << "\tdouble";

    struct_mem << " " << cmdArg(state);
    struct_mem << ";\n";
  }
  return struct_mem;
}

// Initialize the 'struct' member as declared above.
StringList SRCGCStar::cmdargStatesInit(const State* state) {
  StringList struct_init;
  if (isCmdArg(state))
    struct_init << state->currentValue() << ", ";
  return struct_init;
}

// Generate code segment to set the 'struct' member using
// command-line specified values.
StringList SRCGCStar::setargState(const State* state)  {
  StringList setarg_proc;
  if (isCmdArg(state)) {
    StringList temp;
    temp << cmdArg(state);
    setarg_proc << "\t\t\tif(!strcmp(arg[i], \"-" << temp << "\")) {\n";
    setarg_proc << "\t\t\t\tif(arg[i+1])\n";
    setarg_proc << "\t\t\t\t\targ_store." << temp << "=";
    if(state->isA("IntState"))
      setarg_proc << "atoi";
    else
      setarg_proc << "atof";
    setarg_proc << "(arg[i+1]);\n\t\t\t\tcontinue;\n\t\t\t}\n";
  }
  return setarg_proc;
}

// Adds each settable state and their default values to the
// code segment that generates the help message.
StringList SRCGCStar::setargStatesHelp(const State* state) {
  StringList arg_help;
  if (isCmdArg(state))
    arg_help << "\t" << cmdArg(state) << "\\tdefault : "
             << state->currentValue() << "\\\n\\n";
  return arg_help;
}


StringList SRCGCStar::initCodeState(const State* state) {
  StringList code;
  StringList val;
    
  StringList name = starSymbol.lookup(state->name());
    
  // If "state" is to be setted using command-line arguments,
  // initialize it from the 'struct' member.
  if (isCmdArg(state)) {  
    val = "arg_store.";   
    val << cmdArg(state);   
  } else {
    val = state->currentValue();  
  }
  if (state->isA("StringState")) {
    code << name << '=' << '"' << val << '"' << ";\n";
  } else {
    code << name << '=' << val << ";\n";
  }
    
  return code;
}

// Add lines to be put at the beginning of the code file
int SRCGCStar :: addInclude(const char* decl) {
  StringList temp = "#include ";
  temp << decl << "\n";
  return addCode(temp, "include", decl);
}

// Add options to be used when compiling a C program
// We don't run process code on this - it would add a \n
int SRCGCStar::addCompileOption(const char* decl) {
  CodeStream* compileOptions;
  if ((compileOptions = getStream("compileOptions")) == 0)
    return FALSE;

  StringList temp;
  temp << " " << decl << " ";
  return compileOptions->put(temp,decl);
}

// Add options to be used when linking a C program
// We don't run process code on this - it would add a \n
int SRCGCStar::addLinkOption(const char* decl) {
  CodeStream* linkOptions;
  if ((linkOptions = getStream("linkOptions")) == 0)
    return FALSE;

  StringList temp;
  temp << " " << decl << " ";
  return linkOptions->put(temp,decl);
}

// Add options to be used when linking a C program on the local machine.
// We don't run process code on this - it would add a \n
int SRCGCStar::addLocalLinkOption(const char* decl) {
  CodeStream* linkOptions;
  if ((linkOptions = getStream("localLinkOptions")) == 0)
    return FALSE;

  StringList temp;
  temp << " " << decl << " ";
  return linkOptions->put(temp,decl);
}

// Add options to be used when linking a C program on a remote machine.
// We don't run process code on this - it would add a \n
int SRCGCStar::addRemoteLinkOption(const char* decl) {
  CodeStream* linkOptions;
  if ((linkOptions = getStream("remoteLinkOptions")) == 0)
    return FALSE;

  StringList temp;
  temp << " " << decl << " ";
  return linkOptions->put(temp,decl);
}

// Add a file that will be copied over to a remote machine
int SRCGCStar::addRemoteFile(const char* filename, int CcodeFlag) {
  CodeStream* remoteFiles = getStream("remoteFiles");
  StringList temp = filename;
  temp << " ";
  int retval = FALSE;
  if (remoteFiles) {
    retval = remoteFiles->put(temp, filename);
    if (retval && CcodeFlag) {
      CodeStream* remoteCFiles = getStream("remoteCFiles");
      if (remoteCFiles)
        retval = remoteCFiles->put(temp, filename);
    }
  }
  return retval;
}

// Pull in a supporting C module from a library
void SRCGCStar::addModuleFromLibrary(const char* module,
                                   const char* subdirectory,
                                   const char* libraryName) {
  StringList cFile, cPath, includeDir, includeFileQuoted;
  StringList includePath, includeDirSpec, libSpec, path;
  StringList ptolemyLibDirSpec;
  char* ptolemy = "$PTOLEMY";
  char* ptarch = "$PTARCH";
  cFile << module << ".c";
  includeFileQuoted << "\"" << module << ".h\"";
  includeDir << ptolemy << "/" << subdirectory;
  includeDirSpec << "-I" << includeDir;
  ptolemyLibDirSpec << "-L" << ptolemy << "/lib." << ptarch;
  libSpec << "-l" << libraryName;
  path << ptolemy << "/" << subdirectory << "/";
  includePath << path << module << ".h";
  cPath << path << cFile;
  
  addInclude(includeFileQuoted);
  addCompileOption(includeDirSpec);
  addLocalLinkOption(ptolemyLibDirSpec);
  addLocalLinkOption(libSpec);
  addRemoteFile(cPath, TRUE);
  addRemoteFile(includePath, FALSE);
}

// Pull in a supporting C module from a library
void SRCGCStar::addFixedPointSupport() {
  addModuleFromLibrary("SRCGCrtlib", "src/domains/SRCGC/rtlib", "SRCGCrtlib");
}
