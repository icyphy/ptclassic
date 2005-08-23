/**********************************************************************
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE SANDERS, A LOCKHEED MARTIN COMPANY BE LIABLE TO ANY 
PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

SANDERS, A LOCKHEED MARTIN COMPANY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SANDERS, A LOCKHEED MARTIN
COMPANY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
                                                        COPYRIGHTENDKEY


 Programmers:  Ken Smith
 Date of creation: 3/23/98
 Version: @(#)acs_vhdl_lang.cc      1.0     06/16/99
***********************************************************************/
#include "acs_vhdl_lang.h"


VHDL_LANG::VHDL_LANG() 
{
  // Should add a mode switch for selecting amongst
  // VHDL dialects.

  nc_sizes=new ACSIntArray;
}


VHDL_LANG::~VHDL_LANG()
{
  delete nc_sizes;
}


////////////////////////////////////////////
// Add callouts to VHDL libraries and usages
////////////////////////////////////////////
char* VHDL_LANG::gen_libraries(StringArray* libraries,
			       StringArray* includes)
{
  ostrstream expression;
  expression << "--" << endl;
  expression << "-- Stitcher-generated library definitions" << endl;
  expression << "--" << endl;

  StringArray* orig_libs=new StringArray;
  StringArray* orig_incs=new StringArray;

  if (DEBUG_LIB)
    if (libraries->population() != includes->population())
      {
	printf("VHDL_LANG::gen_libraries:Mismatch, libraries and includes!!\n");
	for (int loop=0;loop<libraries->population();loop++)
	  printf("lib[%d]=%s\n",loop,libraries->get(loop));
	for (int loop=0;loop<includes->population();loop++)
	  printf("inc[%d]=%s\n",loop,includes->get(loop));
      }

  char* lib_name=new char[MAX_STR];
  char* inc_name=new char[MAX_STR];
  for (int loop=0;loop<libraries->population();loop++)
    {
      strcpy(lib_name,libraries->get(loop));
      sprintf(inc_name,"%s%s",lib_name,includes->get(loop));

      char lib_dup=FALSE;
      char inc_dup=FALSE;
      
      if (loop!=0)
	{
	  lib_dup=dup_string(lib_name,orig_libs);
	  
	  // FIX: Need to be able to distinguish amongst include extensions 
	  //      that overlap.
	  inc_dup=dup_string(inc_name,orig_incs);
	}
      
      if (!lib_dup)
	{
	  expression << "library " << lib_name << end_statement << endl;
	  orig_libs->add(lib_name);
	}
      if (!inc_dup)
	{
	  expression << "use " << inc_name << end_statement << endl;
	  orig_incs->add(inc_name);
	}
    }  

  if (DEBUG_LIB)
    printf("Libraries built\n");


  // Cleanup
  delete orig_libs;
  delete orig_incs;
  delete []lib_name;
  delete []inc_name;

  // Terminate stream
  expression << endl << ends;

  return(expression.str());
}


////////////////////////////////////////////////////
// Construct the root vhdl entity for all components
////////////////////////////////////////////////////
char* VHDL_LANG::gen_entity(const char* entity_str,
			    Pin* ext_signals)
{
  // Result
  ostrstream expression;

  // Define root structure
  expression << "--" << endl;
  expression << "-- Stitcher-generated I/O ports" << endl;
  expression << "--" << endl;
  expression << set_entity(entity_str);

  // Define I/O signals
  // IMPORTANT:There must always be external signals!
  expression << start_port() << set_port(ext_signals) << end_port(entity_str);

  // Terminate stream
  expression << endl << ends;

  return(expression.str());
}


char* VHDL_LANG::gen_architecture(const char* entity_name,
				  CoreList* sg_list,
				  const int mode,
				  Pin* ext_signals,
				  Pin* data_signals,
				  Pin* ctrl_signals,
				  Pin* constant_signals)
{
  // Result
  ostrstream expression;


  // Generate architecture
  expression << "--" << endl;
  expression << "-- Stitcher-generated architecture" << endl;
  expression << "--" << endl;
  
  expression << start_architecture(mode,entity_name);

  // Add all intermediate data signals
  if (data_signals != NULL)
    if (data_signals->query_pincount() != 0)
      {
	expression << "--" << endl;
	expression << "-- Stitcher-generated intermediate data signals" 
		   << endl;
	expression << "--" << endl;

	if (DEBUG_SIGNALS)
	  printf("Exporting %d data signals\n",data_signals->query_pincount());
	for (int loop=0;loop<data_signals->query_pincount();loop++)
	  {
	    if (DEBUG_SIGNALS)
	      printf("data_signals: name=%s, data_type=%d, vlength=%d\n",
		     data_signals->query_pinname(loop),
		     data_signals->query_datatype(loop),
		     data_signals->query_bitlen(loop));
		   
	    expression << signal(data_signals->query_pinname(loop),
				 data_signals->query_datatype(loop),
				 data_signals->query_bitlen(loop));
	  }
	expression << endl;
      }

  // Add all intermediate control signals
  if (ctrl_signals != NULL)
    if (ctrl_signals->query_pincount() != 0)
      {
	expression << "--" << endl;
	expression << "-- Stitcher-generated intermediate control signals" 
		   << endl;
	expression << "--" << endl;
	if (DEBUG_SIGNALS)
	  printf("Exporting %d ctrl signals\n",ctrl_signals->query_pincount());
	for (int loop=0;loop<ctrl_signals->query_pincount();loop++)
	  {
	    if (DEBUG_SIGNALS)
	      printf("ctrl_signals: name=%s, data_type=%d, vlength=%d\n",
		     ctrl_signals->query_pinname(loop),
		     ctrl_signals->query_datatype(loop),
		     ctrl_signals->query_bitlen(loop));
		   
	    expression << signal(ctrl_signals->query_pinname(loop),
				 ctrl_signals->query_datatype(loop),
				 ctrl_signals->query_bitlen(loop));
	  }
	expression << endl;
      }
  
  if (sg_list != NULL)
  {
    expression << gen_components(entity_name,sg_list);
    if (mode==STRUCTURAL)
      expression << gen_configurations(sg_list);
    expression << gen_instantiations(mode,
				     sg_list,
				     ext_signals,
				     data_signals,
				     ctrl_signals,
				     constant_signals);
    expression << end_architecture(mode) << endl;
  }
  else
    expression << const_definitions(constant_signals);
  
  // Terminate stream
  expression << endl << ends;

  return(expression.str());
}


char* VHDL_LANG::gen_components(const char* entity_name,
				CoreList* sg_list)
{
  ostrstream expression;

  // Generate a component token for each child star
  if (sg_list != NULL) 
    {
      int sg_count=sg_list->size();
      for (int loop=0;loop<sg_count;loop++)
	{
	  ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) sg_list->elem(loop);
	  
	  // Only generate components for algorithmic stars
	  if (smart_generator->acs_type==BOTH)
	    {
	      if (DEBUG_SETPORT)
		printf("Set port definitions for sg %s\n",
		       smart_generator->comment_name());

	      // For multiple implementations, instruct the cores to rename their pins now!
	      smart_generator->revisit_pins();
	  
	      // Generate components
	      expression << "-- " << smart_generator->comment_name() << endl;
	      if ((smart_generator->unique_component) && 
		  (smart_generator->declaration_flag))
		expression << start_component(smart_generator->unique_name.str());
	      else
		expression << start_component(smart_generator->name());
	      
	      // Start the port declaration
	      expression << start_port();
	      
	      // Now query each port and generate a port for each
	      Pin* queried_port=smart_generator->pins;
	      expression << set_port(queried_port);
	      
	      // Eqnd the port declaration
	      expression << end_port();
	      expression << end_component();
	      expression << endl;
	      
	      // Cleanup
	    }
	} //      for (int loop=0;loop<smart_generators->size();loop++)

    } //  if (smart_generators != NULL) 
  
  // Terminate stream
  expression << endl << ends;

  return(expression.str());
}


char* VHDL_LANG::gen_configurations(CoreList* smart_generators)
{
  ostrstream expression;

  // For all star instances
  if (smart_generators != NULL)
    {
      // Generate architecture
      expression << "--" << endl;
      expression << "-- Stitcher-generated configurations" << endl;
      expression << "--" << endl;
      
      char* unique_star_name=new char[MAX_STR];
      char* output_file=new char[MAX_STR];
      for (int loop=0;loop<smart_generators->size();loop++)
	{
	  ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) smart_generators->elem(loop);
	  
	  // Only generate configurations for algorithmic stars
	  if (smart_generator->acs_type==BOTH)
	    {
	      // ASSUMPTION: Only one output file generated per single VHDL smart generator
	      sprintf(unique_star_name,"U_%s",smart_generator->name());
	      if (smart_generator->sg_language==VHDL_BEHAVIORAL)
		sprintf(output_file,"WORK.%s (behavioral)",smart_generator->name());
	      else
		sprintf(output_file,"WORK.%s (structural)",smart_generator->name());
//	      expression << for_use(unique_star_name,
//				    smart_generator->name(),
//				    output_file);
	    }
	}
      // Cleanup
      delete []unique_star_name;
      delete []output_file;
    }

  // Terminate stream
  expression << endl << ends;

  return(expression.str());
}


char* VHDL_LANG::gen_instantiations(const int mode,
				    CoreList* smart_generators,
				    Pin* ext_signals,
				    Pin* data_signals,
				    Pin* ctrl_signals,
				    Pin* constant_signals)
{
  ostrstream constant_expression;
  ostrstream expression;

  // For all star instances
  if (smart_generators != NULL) 
    {
      // Generate architecture
      expression << "--" << endl;
      expression << "-- Stitcher-generated component instantiations" << endl;
      expression << "--" << endl << endl;

      char* unique_star_name=new char[MAX_STR];
      
      expression << begin_scope << endl;
      for (int loop=0;loop<smart_generators->size();loop++)
	{
	  ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) smart_generators->elem(loop);

	  // Only generate configurations for algorithmic stars
	  if (smart_generator->acs_type==BOTH)
	    {
	      // Develop names, as done in add_configurations [Hint: use procedure dummy;) ]
	      sprintf(unique_star_name,"U_%s",smart_generator->name());

	      // Now query each port and generate a port for each
	      // FIX: Ignoring biports for now...
	      StringArray* lh_names=new StringArray;
	      StringArray* rh_names=new StringArray;
	      
	      // Assign ports
	      bind_ports(smart_generator,lh_names,rh_names,
			 ext_signals,data_signals,ctrl_signals,constant_signals);
	      
	      if (DEBUG_BIND)
		{
		  printf("Binding for sg %s\n",smart_generator->name());
		  for (int loop=0;loop<lh_names->population();loop++)
		    printf("Binding %s to %s\n",
			   lh_names->get(loop),
			   rh_names->get(loop));
		}

	      expression << "-- " << smart_generator->comment_name() << endl;

	      if (smart_generator->unique_component)
		expression << instantiate(mode,
					  unique_star_name,
					  smart_generator->unique_name.str(),
					  lh_names,
					  rh_names);
	      else
		expression << instantiate(mode,
					  unique_star_name,
					  smart_generator->name(),
					  lh_names,
					  rh_names);
	      // Cleanup
	      delete lh_names;
	      delete rh_names;

	    } // if (smart_generator->acs_type==BOTH)
	} // for (int loop=0;loop<smart_generators->size();loop++)

      // Cleanup
      delete []unique_star_name;
	
    } // if (smart_generators != NULL) 

  // Merge and terminate stream
  ostrstream result_stream;
  expression << ends;
  result_stream << const_definitions(constant_signals) << endl << expression.str() << endl << ends;

  return(result_stream.str());
}

char* VHDL_LANG::const_definitions(Pin* constant_signals)
{
  ostrstream constant_expression;

  constant_expression << "--" << endl;
  constant_expression << "-- Stitcher-generated constant signals" << endl;
  constant_expression << "--" << endl;
  
  constant_expression << "signal NC:std_logic;" << endl;
  constant_expression << "constant VCC:std_ulogic:='1';" << endl;
  constant_expression << "constant GND:std_ulogic:='0';" << endl;
  
  if (constant_signals!=NULL)
    for (int loop=0;loop<constant_signals->query_pincount();loop++)
      {
	// FIX: VCC, GND traps should be removed once assignments like above can be made
	//      they are still kept as constant signals for binding purposes, but need to 
	//      preclude multiple definitions
	char* pin_name=constant_signals->query_pinname(loop);
	if ((strcmp(pin_name,"GND")!=0) && (strcmp(pin_name,"VCC")!=0))
	  constant_expression << signal(constant_signals->query_pinname(loop),
					constant_signals->query_datatype(loop),
					constant_signals->query_bitlen(loop));
      }
	
  // Terminate streams
  constant_expression << endl << ends;

  return(constant_expression.str());
}


// FIX:This is langauge independent stuff and should be moved back to the target!
int VHDL_LANG::bind_ports(ACSCGFPGACore* smart_generator,
			  StringArray* lh_names,
			  StringArray* rh_names,
			  Pin* ext_signals,
			  Pin* data_signals,
			  Pin* ctrl_signals,
			  Pin* constant_signals)
{
  if (DEBUG_BIND)
    printf("Binding smart generator %s\n",smart_generator->name());

  Pin* smartgen_pins=smart_generator->pins;
  for (int pin_loop=0;pin_loop<smartgen_pins->query_pincount();pin_loop++)
    if (!smartgen_pins->query_skip(pin_loop))
      {
	lh_names->add(smartgen_pins->query_pinname(pin_loop));
	if (DEBUG_BIND)
	  printf("Port %s is being bound to ",lh_names->get(pin_loop));
	
	Connectivity* smartgen_connector=smartgen_pins->query_connection(pin_loop);
	
	// Signals above 0 are redundant and are only useful for resolving connections
	if (smartgen_connector->node_count > 0)
	  {
	    int signal=smartgen_connector->query_pinsignal(0);
	    if (DEBUG_BIND)
	      printf("signal %d of type %d \n",
		     signal,
		     smartgen_connector->query_pintype(0));
	    
	    // Bind proper signal names
	    // FIX:Instead of the 1bit GND, this function should analyze for larger sized inputs
	    //     and add sufficient bitwidth grounds!!
	    if (signal==-1)
	      if (smartgen_pins->pin_classtype(pin_loop)==INPUT_PIN)
		rh_names->add("GND");
	      else
		{
		  if (smartgen_pins->query_bitlen(pin_loop)==1)
		    rh_names->add("NC");
		  else
		    rh_names->add(set_nc(smartgen_pins->query_bitlen(pin_loop),
					 constant_signals));
		}
	    else if (smartgen_connector->query_pintype(0)==DATA_NODE)
	      rh_names->add(data_signals->query_pinname(signal));
	    else if (smartgen_connector->query_pintype(0)==CTRL_NODE)
	      rh_names->add(ctrl_signals->query_pinname(signal));
	    else if (smartgen_connector->query_pintype(0)==EXT_NODE)
	      {
		if (DEBUG_BIND)
		  printf("Binding to external signal\n");
		if (ext_signals->query_altpinflag(signal))
		  {
		    if (ext_signals->req_pintype(pin_loop)==INPUT_PIN)
		      {
			if (DEBUG_BIND)
			  printf("Split-type, will use primary pinname %s\n",
				 ext_signals->query_pinname(signal));
			rh_names->add(ext_signals->query_pinname(signal));
		      }
		    else
		      {
			if (DEBUG_BIND)
			  printf("Split-type, will use alternate pinname %s\n",
				 ext_signals->query_altpinname(signal));
			rh_names->add(ext_signals->query_altpinname(signal));
		      }
		  }
		else
		  {
		    if (DEBUG_BIND)
		      printf("Will use primary pinname %s\n",
			     ext_signals->query_pinname(signal));
		    rh_names->add(ext_signals->query_pinname(signal));
		  }
	      }
	    else if (smartgen_connector->query_pintype(0)==CONSTANT_NODE)
	      rh_names->add(constant_signals->query_pinname(signal));
	    else
	      {
		if (smartgen_pins->query_bitlen(pin_loop)==1)
		  rh_names->add("NC");
		else
		  rh_names->add(set_nc(smartgen_pins->query_bitlen(pin_loop),
				       constant_signals));
		if (DEBUG_BIND)
		  printf("VHDL_LANG::bind_ports, error unknown dnode type\n");
	      }
	  }
	else
	  {
	    if (smartgen_pins->pin_classtype(pin_loop)==INPUT_PIN)
	      rh_names->add("GND");
	    else
	      {
		if (smartgen_pins->query_bitlen(pin_loop)==1)
		  rh_names->add("NC");
		else
		  rh_names->add(set_nc(smartgen_pins->query_bitlen(pin_loop),
				       constant_signals));
	      }
	    if (DEBUG_BIND)
	      printf(" NC\n");
	  }
	
	// Complete the binding
	if (DEBUG_BIND)
	  printf("that is named %s\n",rh_names->get(pin_loop));
      }
  
  // Return happy condition
  return(1);
}


// FIX:NC should be tracked for both INPUT and OUTPUTS separately!!!
char* VHDL_LANG::set_nc(const int nc_size, Pin* constants)
{
  ostrstream expression;
  
  // Form NC expression
  expression << "NC" << nc_size << ends;

  if (!nc_sizes->find(nc_size))
    {
      nc_sizes->add(nc_size);

      constants->add_pin(expression.str(),
			 0,
			 nc_size,
			 STD_LOGIC,
			 INTERNAL_PIN);
    }

  return(expression.str());
}



///////////////////////////////////////////////////////////
// VHDL construct to generate the start of an entity block:
// entity (name) is
//
///////////////////////////////////////////////////////////
char* VHDL_LANG::set_entity(const char* name)
{
  ostrstream expression;
  expression << "entity " << name << " is" << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////////////
// VHDL construct to generate the start of an architecture block:
// architecture (structural or behavioral) of (name) is
//
// mode determine whether it is a structural or behavioral definition
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::start_architecture(const int mode,const char* name)
{
  ostrstream expression;
  if (mode==STRUCTURAL)
    expression << "architecture structural of " << name << " is" << endl;
  else if (mode==BEHAVIORAL)
    expression << "architecture behavioral of " << name << " is" << endl;
  else if (mode==BLANK)
    expression << "architecture Blank of " << name << " is" << endl;
  else
    expression << "VHDL_LANG::set_architecture-error:Undefined architecture type" 
	       << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::start_architecture(const int mode,const strstreambuf* name)
{
  ostrstream expression;
  if (mode==STRUCTURAL)
    expression << "architecture structural of " << name << " is" << endl;
  else if (mode==BEHAVIORAL)
    expression << "architecture behavioral of " << name << " is" << endl;
  else if (mode==BLANK)
    expression << "architecture Blank of " << name << " is" << endl;
  else
    expression << "VHDL_LANG::set_architecture-error:Undefined architecture type" 
	       << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

/////////////////////////////////////////////////////////////////////
// VHDL construct to generate the ending of an architecture block:
// end (structural or behavioral);
//
// mode determine whether it is a structural or behavioral definition
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::end_architecture(const int mode)
{
  ostrstream expression;
  if (mode==STRUCTURAL)
    expression << "end structural;" << endl;
  else if (mode==BEHAVIORAL)
    expression << "end behavioral;" << endl;
  else if (mode==BLANK)
    expression << "end Blank;" << endl;
  else
    expression << "VHDL_LANG::set_architecture-error:Undefined architecture type" 
	       << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

////////////////////////////////////////////////////////
// VHDL construct to generate the start of a port block:
// port (
//
////////////////////////////////////////////////////////
char* VHDL_LANG::start_port()
{
  ostrstream expression;
  expression << "port (" << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////
// VHDL construct to generate the body of a port block:
// For each port within a given Pin, generate a signal identifier for the port:
// name1:in (data_type,vector_length);
// name2:out (data_type,vector_length);
// name3:inout (data_type,vector_length);
//
///////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::set_port(Pin* pin_ref)
{
  ostrstream expression;

  if (pin_ref->query_pincount() != 0)
    for (int loop=0;loop<pin_ref->query_pincount();loop++)
      if (!pin_ref->query_skip(loop))
	{
	  
	  // Determine the pin's type
	  switch (pin_ref->pin_classtype(loop))
	    {
	    case INPUT_PIN:
	      if (pin_ref->query_altpinflag(loop))
		{
		  expression << "\t" << pin_ref->query_altpinname(loop);
		  expression << " :in ";
		  expression << pin_declaration(pin_ref,loop);
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :out ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      else
		{
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :in ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      break;
	    case OUTPUT_PIN:
	      if (pin_ref->query_altpinflag(loop))
		{
		  expression << "\t" << pin_ref->query_altpinname(loop);
		  expression << " :in ";
		  expression << pin_declaration(pin_ref,loop);
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :out ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      else
		{
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :out ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      break;
	    case BIDIR_PIN:
	      if (pin_ref->query_altpinflag(loop))
		{
		  expression << "\t" << pin_ref->query_altpinname(loop);
		  expression << " :in ";
		  expression << pin_declaration(pin_ref,loop);
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :out ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      else
		{
		  expression << "\t" << pin_ref->query_pinname(loop);
		  expression << " :inout ";
		  expression << pin_declaration(pin_ref,loop);
		}
	      break;
	    default:
	      if (DEBUG_SETPORT)
		printf("VHDL::set_port:Error unknown pin_classtype %d\n",
		       pin_ref->query_pintype(loop));
	      expression << " :UNKNOWN ";
	    }
	}
  
  // Terminate stream
  expression << ends;

  return(expression.str());
}

char* VHDL_LANG::pin_declaration(Pin* pin_ref,const int index)
{
  ostrstream expression;

  // Determine signal type
  if (DEBUG_SETPORT)
    printf("data_type[%d]=%d, vector_length[%d]=%d\n",
	   index,pin_ref->query_datatype(index),
	   index,pin_ref->query_bitlen(index));
  expression << signal_type(pin_ref->query_datatype(index),
			    pin_ref->query_bitlen(index));
  
  // Determine the number of bits involved and their classification
  if (index < pin_ref->query_pincount()-1)
    {
      // Ensure that this truely isnt the last one due to pin skips
      int skip=1;
      int pin_index=index+1;
      while ((skip==1) && (pin_index < pin_ref->query_pincount()))
	if (!pin_ref->query_skip(pin_index++))
	  skip=0;
      if (!skip)
	expression << end_statement << endl;
    }

  // Terminate stream
  expression << ends;

  return(expression.str());
}


//////////////////////////////////////////////////////
// VHDL construct to generate the end of a port block:
// );
//
//////////////////////////////////////////////////////
char* VHDL_LANG::end_port()
{
  ostrstream expression;
  expression << ")" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
//////////////////////////////////////////////////////
// VHDL construct to generate the end of a port block:
// ) (entity_name);
//
//////////////////////////////////////////////////////
char* VHDL_LANG::end_port(const char* entity_name)
{
  ostrstream expression;
  expression << ")" << end_statement << endl;
  expression << end_scope << entity_name << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::end_port(const strstreambuf* entity_name)
{
  ostrstream expression;
  expression << ")" << end_statement << endl;
  expression << end_scope << entity_name << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


//////////////////////////////////////////////////////
// VHDL construct to generate a signal definition:
// signal (signal_name_str):(data_type,vector_length);
//
//////////////////////////////////////////////////////
char* VHDL_LANG::signal(const char* signal_name,
			const int data_type,
			const int vector_length)
{
  ostrstream expression;
  expression << signal_declaration << signal_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::signal(const strstreambuf* signal_name,
			const int data_type,
			const int vector_length)
{
  ostrstream expression;
  expression << signal_declaration << signal_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
/////////////////////////////////////////////////////////////////////
// VHDL construct to generate a signal definition:
// signal (signal_name_str):(data_type,vector_length):=initial_value;
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::signal(const strstreambuf* signal_name,
			const int data_type,
			const int vector_length,
			const strstreambuf* initial_value)
{
  ostrstream expression;
  expression << signal_declaration << signal_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << initial_value_assignment << initial_value;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::signal(const char* signal_name,
			const int data_type,
			const int vector_length,
			const char* initial_value)
{
  ostrstream expression;
  expression << signal_declaration << signal_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << initial_value_assignment << initial_value;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

/////////////////////////////////////////////////////////////////////
// VHDL construct to generate a signal definition:
// signal (signal_name_str):(type_str)
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::signal(const char* signal_name,
			const char* type_str)
{
  ostrstream expression;
  expression << signal_declaration << signal_name << ":" << type_str;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


//////////////////////////////////////////////////////
// VHDL construct to generate a variable definition:
// variable (signal_name_str):(data_type,vector_length);
//
//////////////////////////////////////////////////////
char* VHDL_LANG::variable(const char* variable_name,
			  const int data_type,
			  const int vector_length)
{
  ostrstream expression;
  expression << variable_declaration << variable_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::variable(const strstreambuf* variable_name,
			  const int data_type,
			  const int vector_length)
{
  ostrstream expression;
  expression << variable_declaration << variable_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
/////////////////////////////////////////////////////////////////////
// VHDL construct to generate a variable definition:
// variable (signal_name_str):(data_type,vector_length):=initial_value;
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::variable(const strstreambuf* variable_name,
			  const int data_type,
			  const int vector_length,
			  const strstreambuf* initial_value)
{
  ostrstream expression;
  expression << variable_declaration << variable_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << initial_value_assignment << initial_value;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::variable(const char* variable_name,
			  const int data_type,
			  const int vector_length,
			  const char* initial_value)
{
  ostrstream expression;
  expression << variable_declaration << variable_name << ":";
  expression << signal_type(data_type,vector_length);
  expression << initial_value_assignment << initial_value;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
/////////////////////////////////////////////////////////////////////
// VHDL construct to generate a variable definition:
// variable (signal_name_str):(type_str)
//
/////////////////////////////////////////////////////////////////////
char* VHDL_LANG::variable(const char* variable_name,
			  const char* type_str)
{
  ostrstream expression;
  expression << variable_declaration << variable_name << ":" << type_str;
  expression << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////
// VHDL construct to generate the start of a component block:
// component (component_name)
//
/////////////////////////////////////////////////////////////
char* VHDL_LANG::start_component(const char* component_name)
{
  ostrstream expression;
  expression << "component " << component_name << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////
// VHDL construct to generate the end of a component block:
// end component
//
/////////////////////////////////////////////////////////////
char* VHDL_LANG::end_component()
{
  ostrstream expression;
  expression << end_scope << "component" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate a signal type definition
// (std_ulogic_vector,std_ulogic,bit_vector,bit) ((vector_length) downto 0)
//
// data_type switches amongst the different data types.
///////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::signal_type(const int data_type,
			     const int vector_length)
{
  ostrstream expression;
  switch (data_type)
    {
    case STD_ULOGIC:
      if (vector_length > 1)
	expression << "std_ulogic_vector (" << vector_length-1 
		   << vector_line << ")";
      else
	expression << "std_ulogic";
      break;
    case STD_LOGIC:
      if (vector_length > 1)
	expression << "std_logic_vector (" << vector_length-1 
		   << vector_line << ")";
      else
	expression << "std_logic";
      break;
    case BIT:
      if (vector_length > 1)
	expression << "bit_vector (" << vector_length-1 
		   << vector_line << ")";
      else
	expression << "bit";
      break;
    case INTEGER:
      if (vector_length > 1)
	expression << "integer range " << vector_length-1 << vector_line;
      else
	expression << "integer";
      break;
    default:
      if (DEBUG_SETSIGNAL)
	printf("VHDL::signal_type:Error unknown data_type %d\n",data_type);
      expression << "UNKNOWN";
    }

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////
// VHDL constructor to generate a for-use invokation
//
// for (acs_name):(acs_classname) use entity (output_file);
//
///////////////////////////////////////////////////////////
char* VHDL_LANG::for_use(const char* acs_name,
			 const char* acs_classname,
			 const char* output_file)
{
  ostrstream expression;
  expression << "for " << acs_name << " : " << acs_classname << " use entity "
	     << output_file << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


//////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an instantiation statement
//
// (acs_name):(acs_classname port map (lh_name1=>rh_name1,lh_name2=>rh_name2);
//////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::instantiate(const int mode,
			     const char* acs_name,
			     const char* acs_classname,
			     StringArray* lh_names,
			     StringArray* rh_names)
{
  ostrstream expression;
  
  if (mode==STRUCTURAL)
    {
      // Preamble the instance
//      expression << "for ";
    }

  expression << acs_name << ":" << acs_classname << " port map (" << endl;

  for (int loop=0;loop<lh_names->population();loop++)
    {
      if (DEBUG_INST)
	printf("instantiation %d is named %s\n",
	       loop,
	       rh_names->get(loop));

      // Retrieve bound names and generate binding expression
      expression << "\t" << lh_names->get(loop) << "=>" 
		 << rh_names->get(loop);
      if (loop<lh_names->population()-1)
	expression << "," << endl;

    }
  expression << ")" << end_statement << endl;

  // Terminate stream
  expression << ends;

  // Return happy condition
  return(expression.str());
}


///////////////////////////////////////////////////////
// VHDL constructor to generate a type definition block
//
// type (state_str) is (
//     state_list0,
//     state_list1,
//         ...
//     state_listn
// );
///////////////////////////////////////////////////////
char* VHDL_LANG::type_def(const char* state_str,
			  StringArray* state_list)
{
  ostrstream expression;

  expression << "type " << state_str << " is (" << endl;

  int num_states=state_list->population()-1;
  for (int loop=0;loop<=num_states;loop++)
    if (loop!=num_states)
      expression << "\t" << state_list->get(loop) << "," << endl;
    else
      expression << "\t" << state_list->get(loop);
      
  expression << ")" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate a process sensitivity declaration
//
// (process_name):process(sensitivity1, sensitivity2, ..., sensitivityN)
////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::process(const char* process_name,
			 StringArray* sensitivities)
{
  ostrstream expression;
  
  expression << process_name <<": process (" << endl;

  int num_states=sensitivities->population()-1;
  for (int loop=0;loop<=num_states;loop++)
    if (loop!=num_states)
      expression << "\t" << sensitivities->get(loop) << "," << endl;
    else
      expression << "\t" << sensitivities->get(loop);
      
  expression << ")" << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////
// VHDL constructor to generate the beginning of a code block
/////////////////////////////////////////////////////////////
char* VHDL_LANG::begin_function_scope(const char* function_name)
{
  ostrstream expression;
  expression << "begin process " << function_name << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


//////////////////////////////////////////////////////////////
// VHDL constructor to generate the completion of a code block
//////////////////////////////////////////////////////////////
char* VHDL_LANG::end_function_scope(const char* function_name)
{
  ostrstream expression;
  expression << "end process " << function_name << end_statement <<endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an actual value in VHDL syntax given a string
/////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::val(const char* val_str)
{
  ostrstream expression;
  if (strlen(val_str) > 1)
    expression << "\"" << val_str << "\"";
  else
    expression << "'" << val_str << "'";

  // Terminate stream
  expression << ends;

  return(expression.str());
}


////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an assignment.  Expression is returned to caller
//
// "lh_name <= (rh_name) or <= if void arg
////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::equals(void)
{
  ostrstream expression;
  expression << " <= ";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::equals(const char* lh_name,const char* rh_name)
{
  ostrstream expression;

  expression << lh_name << " <= " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::equals(const strstreambuf* lh_name,const char* rh_name)
{
  ostrstream expression;

  expression << lh_name << " <= " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::equals(const char* lh_name,const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << lh_name << " <= " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::equals(const strstreambuf* lh_name,const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << lh_name << " <= " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

//////////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate a nested expression.  Expression is returned to caller
//
// "(lh_name) expr  (rh_name)
//////////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::expr(const char* lh_name,const char* rh_name)
{
  ostrstream expression;

  expression << lh_name << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::expr(const strstreambuf* lh_name,const char* rh_name)
{
  ostrstream expression;

  expression << lh_name << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::expr(const char* lh_name,const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << lh_name << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::expr(const strstreambuf* lh_name,const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << lh_name << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an if_clause expression
//
// variable = value
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::test(const char* lh_name,const char* rh_name)
{
  ostrstream expression;
  
  expression << "(" << lh_name << "=" << rh_name << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::test(const strstreambuf* lh_name,const char* rh_name)
{
  ostrstream expression;
  
  expression << "(" << lh_name << "=" << rh_name << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::test(const strstreambuf* lh_name,const strstreambuf* rh_name)
{
  ostrstream expression;
  
  expression << "(" << lh_name << "=" << rh_name << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}

///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an if expression.  Expression is returned to caller
//
// if (if_clause) then
//   (do_expression)
// end if;
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::l_if(const strstreambuf* if_clause,
		      const strstreambuf* do_expression)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << "then" << endl;
  expression << "\t" << do_expression << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::l_if(const char* if_clause,
		      const strstreambuf* do_expression)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << "then" << endl;
  expression << "\t" << do_expression << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::l_if(const char* if_clause,
		      const char* do_expression)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << "then" << endl;
  expression << "\t" << do_expression << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


////////////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an if-else expression.  Expression is returned to caller
//
// if (if_clause) then
//   (do_expression1)
// else
//   (do_expression2)
// end if;
////////////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::l_if(const char* if_clause,
		      StringArray* do_expressions)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << ") then" << endl;
  expression << "\t" << do_expressions->get(0) << endl;
  expression << "else" << endl;
  expression << "\t" << do_expressions->get(1) << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::l_if(const strstreambuf* if_clause, 
		      StringArray* do_expressions)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << ") then" << endl;
  expression << "\t" << do_expressions->get(0) << endl;
  expression << "else" << endl;
  expression << "\t" << do_expressions->get(1) << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::l_if(const strstreambuf* if_clause, 
		      StringArray& do_expressions)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << ") then" << endl;
  expression << "\t" << do_expressions.get(0) << endl;
  expression << "else" << endl;
  expression << "\t" << do_expressions.get(2) << endl;
  expression << "end if" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

////////////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an if-else expression.  Expression is returned to caller
//
// if (if_clause) then
//   (do_expression1)
// elsif
//   (do_expression2) then
// elsif
//   ... then
// elsif
//   (do_expressionN)
// end if;
////////////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::l_if(const char* if_clause, 
		      StringArray* do_expressions, 
		      StringArray* else_expressions)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << ") then" << endl;
  expression << do_expressions->get(0) << endl;
  
  int num_dos=do_expressions->population()-1;
  for (int loop=0;loop<num_dos;loop++)
    {
      expression << "elsif (" << else_expressions->get(loop) << ")" 
		 << " then" << endl;
      expression << do_expressions->get(loop+1) << endl;
    }
  expression << "end if" << end_statement << endl;
  
  // Terminate stream
  expression << ends;
  
  return(expression.str());
}
char* VHDL_LANG::l_if(const strstreambuf* if_clause, 
		      StringArray* do_expressions, 
		      StringArray* else_expressions)
{
  ostrstream expression;
  
  expression << "if (" << if_clause << ") then" << endl;
  expression << do_expressions->get(0) << endl;

  int num_dos=do_expressions->population()-1;
  for (int loop=0;loop<num_dos;loop++)
    {
      expression << "elsif (" << else_expressions->get(loop) << ")" 
		 << " then" << endl;
      expression << do_expressions->get(loop+1) << endl;
    }
  expression << "end if" << end_statement << endl;
  
  // Terminate stream
  expression << ends;
  
  return(expression.str());
}


//////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate a case expression Expression is returned to caller
//
// case (switch_name) is
// when (state1) =>
// (do_expr1)
// when (state2) =>
// (do_expr2)
// ...
// when (stateN) =>
// (do_exprN)
// others =>
// (do_exprN+1)
// end case;
//////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::l_case(const char* switch_name,
			StringArray* state,
			StringArray* do_expr)
{
  ostrstream expression;

  expression << "case " << switch_name << " is" << endl;
  int num_states=state->population();
  for (int loop=0;loop<num_states;loop++)
    {
      expression << "when " << state->get(loop) << " =>" << endl;
      expression << "\t" << do_expr->get(loop) << endl;
    }
  
  // Check for 'others' case
  if (do_expr->population() > num_states)
    {
      expression << "others =>" << endl;
      expression << "\t" << do_expr->get(do_expr->population()-1) << endl;
    }
  
  expression << "end case" << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an and expression  Expression is returned to caller
//
// "and (rh_name) or " and " if void argument
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::and(void)
{
  ostrstream expression;

  expression << " and ";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::and(const char* rh_name)
{
  ostrstream expression;

  expression << " and " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::and(const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << " and " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


/////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate not assignment.  Expression is returned to caller
//
// "not(rh_name)"
/////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::not(void)
{
  ostrstream expression;

  expression << " not";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::not(const char* rh_name)
{
  ostrstream expression;

  expression << " not(" << rh_name << ") ";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::not(const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << " not(" << rh_name << ") ";

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an and expression  Expression is returned to caller
//
// "or (rh_name) or " or " if void argument
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::or(void)
{
  ostrstream expression;

  expression << " or ";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::or(const char* rh_name)
{
  ostrstream expression;

  expression << " or " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::or(const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << " or " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an XOR expression  Expression is returned to caller
//
// "xor (rh_name)
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::xor(const char* rh_name)
{
  ostrstream expression;

  expression << " xor " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::xor(const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << " xor " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to generate an XOR expression  Expression is returned to caller
//
// "xnor (rh_name)
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::xnor(const char* rh_name)
{
  ostrstream expression;

  expression << " xnor " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::xnor(const strstreambuf* rh_name)
{
  ostrstream expression;

  expression << " xnor " << rh_name;

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to slice a signal from hi_bits to lo_bits, or individual bit.
// Expression is returned to caller
//
// "(sig_name) (hi_bits downto lo_bits)  or
// "(sig_name) (bit)
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::slice(const char* sig_name, const int hi_bits, const int lo_bits)
{
  ostrstream expression;

  if (hi_bits != lo_bits)
    expression << sig_name << "(" << hi_bits << " downto " << lo_bits << ")";
  else
    expression << sig_name << "(" << lo_bits << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::slice(const strstreambuf* sig_name, const int hi_bits, const int lo_bits)
{
  ostrstream expression;

  if (hi_bits != lo_bits)
    expression << sig_name << "(" << hi_bits << " downto " << lo_bits << ")";
  else
    expression << sig_name << "(" << lo_bits << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////////////////
// VHDL constructor to slice a signal for an individual bit
// Expression is returned to caller
//
// "(sig_name) (bit)
///////////////////////////////////////////////////////////////////////////////////
char* VHDL_LANG::slice(const char* sig_name, const int bit)
{
  ostrstream expression;

  expression << sig_name << "(" << bit << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}
char* VHDL_LANG::slice(const strstreambuf* sig_name, const int bit)
{
  ostrstream expression;

  expression << sig_name << "(" << bit << ")";

  // Terminate stream
  expression << ends;

  return(expression.str());
}


///////////////////////////////////////////////////////////////////////
// Tie a series of individual signal slices to a particular signal name
///////////////////////////////////////////////////////////////////////
char* VHDL_LANG::tie_it(const char* snk_signal,
			const int hi_bit,
			const int lo_bit,
			const char* src_signal)
{
  ostrstream expression;
  
  for (int loop=lo_bit;loop<=hi_bit;loop++)
    expression << equals(slice(snk_signal,loop),src_signal)
               << end_statement << endl;

  // Terminate stream
  expression << ends;

  return(expression.str());
}

///////////////////////////////////////////////////////////////////
// Check an existing string_list for the existance of a string that
// is equivalent to src_string.  Return true if this is the case
///////////////////////////////////////////////////////////////////
int VHDL_LANG::dup_string(const char* src_string,
			  StringArray* string_list)
{
  int dup=FALSE;
  for (int dup_loop=0;dup_loop<string_list->population();dup_loop++)
    if (strcmp(src_string,string_list->get(dup_loop))==0)
      {
	dup=TRUE;
	break;
      }
  return(dup);
}












