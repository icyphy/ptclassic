///////////////////////////////////////////////////////////////////////////
// Should become a star method
// FIX: Need to make architecture independent as well
// FIX: Sink and delay counters should be replicated on a per-sink basis!!!
///////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWunirate_sequencer(Fpga* fpga_elem, const int fpga_no)
{
  //
  // Generate some local handles to frequented structures
  //
  Sequencer* sequencer=fpga_elem->sequencer;
  ACSCGFPGACore* seq_core=sequencer->seq_sg;
  Pin* seq_pins=seq_core->pins;

  // Determine if this fpga will be sourcing and/or sinking
  int total_srcs=0;
  int total_snks=0;
  for (int mem_loop=0;mem_loop<fpga_elem->mem_count;mem_loop++)
    if ((fpga_elem->mem_connected)->query(mem_loop)==MEMORY_AVAILABLE)
      {
	Port* mem_port=arch->get_memory_handle(mem_loop);
	total_srcs+=(mem_port->source_cores)->size();
	total_snks+=(mem_port->sink_cores)->size();
      }

  // FIX:Need to switch amongst language models
  VHDL_LANG lang;

  //
  // Define common external signals
  //
  if (DEBUG_SEQUENCER)
    printf("Generating seq_ext_signals for fpga %d\n",fpga_no);
  Pin* seq_ext_signals=new Pin;
  seq_ext_signals->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  seq_ext_signals->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET,AH);
  seq_ext_signals->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL,AH);
  seq_ext_signals->add_pin("Alg_Start",0,1,STD_LOGIC,OUTPUT_PIN_START);
  seq_ext_signals->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK,AL);
  seq_ext_signals->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  seq_ext_signals->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);
  seq_ext_signals->add_pin("ADDR_CE",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCE);
  seq_ext_signals->add_pin("ADDR_CLR",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCLR);
  seq_pins->add_pin("Clock",0,1,STD_LOGIC,INPUT_PIN_CLK);
  seq_pins->add_pin("Reset",0,1,STD_LOGIC,INPUT_PIN_RESET,AH);
  seq_pins->add_pin("Go",0,1,STD_LOGIC,INPUT_PIN_EXTCTRL,AH);
  seq_pins->add_pin("Alg_Start",0,1,STD_LOGIC,OUTPUT_PIN_START);
  seq_pins->add_pin("MemBusGrant_n",0,1,STD_LOGIC,INPUT_PIN_MEMOK,AL);
  seq_pins->add_pin("MemBusReq_n",0,1,STD_LOGIC,OUTPUT_PIN_MEMREQ);
  seq_pins->add_pin("Done",0,1,STD_LOGIC,OUTPUT_PIN_DONE);
  seq_pins->add_pin("ADDR_CE",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCE);
  seq_pins->add_pin("ADDR_CLR",0,1,STD_LOGIC,OUTPUT_PIN_ADDRCLR);

  //
  // Add fpga interconnect pins
  //
  int right_port=UNASSIGNED;
  int left_port=UNASSIGNED;
  if (fpga_no != 4)
    {
      int enable_pin=seq_ext_signals->add_pin("Right_OE",0,36,STD_LOGIC,
					      OUTPUT_PIN_INTERCONNECT_ENABLE);
      int enable_pin2=seq_pins->add_pin("Right_OE",0,36,STD_LOGIC,
					OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_ext_signals->set_pinpriority(enable_pin,1);
      seq_pins->set_pinpriority(enable_pin2,1);
      right_port=fpga_elem->query_mode(fpga_no+1);
    }
  if (fpga_no != 1)
    {
      int enable_pin=seq_ext_signals->add_pin("Left_OE",0,36,STD_LOGIC,
					      OUTPUT_PIN_INTERCONNECT_ENABLE);
      int enable_pin2=seq_pins->add_pin("Left_OE",0,36,STD_LOGIC,
					OUTPUT_PIN_INTERCONNECT_ENABLE);
      seq_ext_signals->set_pinpriority(enable_pin,0);
      seq_pins->set_pinpriority(enable_pin2,0);
      left_port=fpga_elem->query_mode(fpga_no-1);
    }
    
  if (DEBUG_SEQUENCER)
    {
      printf("right_port=%d, left_port=%d\n",right_port,left_port);

      if (right_port!=UNASSIGNED)
	printf("fpga %d is connected to fpga %d as a %d\n",
	       fpga_no,
	       fpga_no+1,
	       right_port);
      if (left_port!=UNASSIGNED)
	printf("fpga %d is connected to fpga %d as a %d\n",
	       fpga_no,
	       fpga_no-1,
	       left_port);
    }

  if (DEBUG_SEQUENCER)
    {
      printf("\n\nDumping ioport timing info\n");
      fpga_elem->mode_dump();
    }

  //
  // Establish pins for any source counter
  //
  char* source_carry_name=new char[MAX_STR];
  if (total_srcs)
    {
      ACSCGFPGACore* src_add=sequencer->src_add;
      Pin* srcadd_pins=src_add->pins;
      int port_id=srcadd_pins->retrieve_type(OUTPUT_PIN_SRC_WC);
      int major_bit=srcadd_pins->query_majorbit(port_id);
      int bitlen=srcadd_pins->query_bitlen(port_id);
      seq_ext_signals->add_pin("SRC_Carry",major_bit,bitlen,STD_LOGIC,INPUT_PIN_SRC_WC,AL);
      seq_ext_signals->add_pin("SRC_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SRC_MUX);
      seq_ext_signals->add_pin("SRC_CE",0,1,STD_LOGIC,OUTPUT_PIN_SRC_CE);
      seq_pins->add_pin("SRC_Carry",major_bit,bitlen,STD_LOGIC,INPUT_PIN_SRC_WC,AL);
      seq_pins->add_pin("SRC_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SRC_MUX);
      seq_pins->add_pin("SRC_CE",0,1,STD_LOGIC,OUTPUT_PIN_SRC_CE);
      sprintf(source_carry_name,"SRC_Carry(%d)",bitlen-1);
    }

  //
  // Establish pins for any sink and delay counters
  //
  char* sink_carry_name=new char[MAX_STR];
  char* delay_carry_name=new char[MAX_STR];
  if (total_snks)
    {
      ACSCGFPGACore* snk_adder=sequencer->snk_add;
      Pin* snkadd_pins=snk_adder->pins;
      int snk_id=snkadd_pins->retrieve_type(OUTPUT_PIN_SNK_WC);
      int mbit=snkadd_pins->query_majorbit(snk_id);
      int blen=snkadd_pins->query_bitlen(snk_id);
      seq_ext_signals->add_pin("SNK_CE",0,1,STD_LOGIC,OUTPUT_PIN_SNK_CE);
      seq_ext_signals->add_pin("SNK_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SNK_MUX);
      seq_ext_signals->add_pin("SNK_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_SNK_WC,AL);
      seq_pins->add_pin("SNK_CE",0,1,STD_LOGIC,OUTPUT_PIN_SNK_CE);
      seq_pins->add_pin("SNK_MUX",0,1,STD_LOGIC,OUTPUT_PIN_SNK_MUX);
      seq_pins->add_pin("SNK_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_SNK_WC,AL);
      sprintf(sink_carry_name,"SNK_Carry(%d)",blen-1);

      ACSCGFPGACore* delay_adder=sequencer->delay_add;
      Pin* deladd_pins=delay_adder->pins;
      int del_id=deladd_pins->retrieve_type(OUTPUT_PIN_DELAY_WC);
      mbit=deladd_pins->query_majorbit(del_id);
      blen=deladd_pins->query_bitlen(del_id);
      seq_ext_signals->add_pin("Delay_CE",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_CE);
      seq_ext_signals->add_pin("Delay_MUX",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_MUX);
      seq_ext_signals->add_pin("Delay_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_DELAY_WC,AL);
      seq_pins->add_pin("Delay_CE",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_CE);
      seq_pins->add_pin("Delay_MUX",0,1,STD_LOGIC,OUTPUT_PIN_DELAY_MUX);
      seq_pins->add_pin("Delay_Carry",mbit,blen,STD_LOGIC,INPUT_PIN_DELAY_WC,AL);
      sprintf(delay_carry_name,"Delay_Carry(%d)",blen-1);
    }
	      
  Pin* seq_data_signals=NULL;

  if (DEBUG_SEQUENCER)
    printf("Generating seq_ctrl_signals\n");
  Pin* seq_ctrl_signals=new Pin;
  seq_ctrl_signals->add_pin("Go_Addr",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Wait_AlgFin",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Addr_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Addr_Ring_CE",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Go_Ring",0,1,STD_LOGIC,INTERNAL_PIN);
  
  // Word counter relevant, reduced if not applicable by synthesis
  seq_ctrl_signals->add_pin("Go_WC_SRC",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_WC_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_Addr_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SRC_Ring_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("Go_WC_SNK",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_WC_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_Addr_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_Ring_Control",0,1,STD_LOGIC,INTERNAL_PIN);
  seq_ctrl_signals->add_pin("SNK_OK",0,1,STD_LOGIC,INTERNAL_PIN);

  Pin* seq_constant_signals=new Pin;
  seq_constant_signals->add_pin("VCC",1.0,STD_LOGIC,OUTPUT_PIN_VCC);
  seq_constant_signals->add_pin("GND",1.0,STD_LOGIC,OUTPUT_PIN_GND);



  // Connectivity model
  ostrstream preamble_expr;
  ostrstream default_expr;
  ostrstream process_expr;

  /////////////////////////////
  // State machine declarations
  /////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating state machine states\n");
  StringArray* master_states=new StringArray;
  master_states->add("Init_State");
  master_states->add("Wait_For_Mem");
  master_states->add("Wait_For_Finish");
  master_states->add("End_State");
  preamble_expr << "--Master state machine states" << endl;
  preamble_expr << lang.type_def("Master_States",master_states);
  preamble_expr << endl;

  StringArray* addr_states=new StringArray;
  addr_states->add("Init_State");
  addr_states->add("Clear_Addr");
  addr_states->add("Enable_Addr");
  addr_states->add("Wait_For_Finish");
  preamble_expr << "--Address state machine states" << endl;
  preamble_expr << lang.type_def("Addr_States",addr_states);
  preamble_expr << endl;

  StringArray* ring_states=new StringArray;
  ring_states->add("Init_State");
  char* state_name=new char[MAX_STR];
  char* sig_name=new char[MAX_STR];
  for (int loop=0;loop<seqlen;loop++)
    {
      sprintf(state_name,"Phase%d",loop);
      sprintf(sig_name,"%s_Go",state_name);
      int phase_pin=seq_ext_signals->
	add_pin(sig_name,0,1,STD_LOGIC,OUTPUT_PIN_PHASE);
      seq_pins->add_pin(sig_name,0,1,STD_LOGIC,OUTPUT_PIN_PHASE);
      seq_ext_signals->set_pinpriority(phase_pin,loop);
      seq_pins->set_pinpriority(phase_pin,loop);
      ring_states->add(state_name);
    }
  delete []state_name;
  delete []sig_name;
  
  preamble_expr << "--Ring state machine states" << endl;
  preamble_expr << lang.type_def("Ring_States",ring_states);
  preamble_expr << endl;

  StringArray* src_wc_states=new StringArray;
  src_wc_states->add("Init_State");
  src_wc_states->add("Clear_WC");
  src_wc_states->add("Prep_WC");
  preamble_expr << "-- Source WC state machine states" << endl;
  preamble_expr << lang.type_def("SRCWC_States",src_wc_states);
  preamble_expr << endl;

  StringArray* snk_wc_states=new StringArray;
  snk_wc_states->add("Init_State");
  snk_wc_states->add("Clear_WC");
  snk_wc_states->add("Wait_For_Delay");
  snk_wc_states->add("Prep_WC");
  preamble_expr << "-- Sink WC state machine states" << endl;
  preamble_expr << lang.type_def("SNKWC_States",snk_wc_states);
  preamble_expr << endl;


  /////////////////////////////
  // State machine declarations
  /////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating state machine declarations\n");

  preamble_expr << "--State machine signals" << endl;
  preamble_expr << lang.signal("Master_State","Master_States");
  preamble_expr << lang.signal("Next_Master_State","Master_States");
  preamble_expr << lang.signal("Addr_State","Addr_States");
  preamble_expr << lang.signal("Next_Addr_State","Addr_States");
  preamble_expr << lang.signal("Ring_State","Ring_States");
  preamble_expr << lang.signal("Next_Ring_State","Ring_States");
  preamble_expr << lang.signal("SRCWC_State","SRCWC_States");
  preamble_expr << lang.signal("Next_SRCWC_State","SRCWC_States");
  preamble_expr << lang.signal("SNKWC_State","SNKWC_States");
  preamble_expr << lang.signal("Next_SNKWC_State","SNKWC_States");
  preamble_expr << endl;


  //////////////////////////////
  // Local variable declarations
  //////////////////////////////

  ///////////////////////////////
  // Generate default assignments
  ///////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating default assignments\n");

  // Master kickoff
  if (total_srcs)
    default_expr << "-- Source word counter control" << endl
		 << lang.equals("SRC_CE","SRC_Addr_Control") 
		 << lang.or("SRC_Ring_Control")
		 << lang.or("SRC_WC_Control")
		 << lang.end_statement << endl << endl;
  if (total_snks)
    default_expr << "-- Sink word counter control" << endl
		 << lang.equals("SNK_CE","SNK_Addr_Control") 
		 << lang.or("(SNK_Ring_Control") << lang.and("SNK_OK)")
		 << lang.or("SNK_WC_Control")
		 << lang.end_statement << endl << endl;

  default_expr << "-- Addr counter control" << endl
	       << lang.equals("ADDR_CE","Addr_Addr_CE");
  if ((total_snks) & (!total_srcs))
    default_expr << lang.or("(Addr_Ring_CE") << lang.and("SNK_OK)");
  else
    default_expr << lang.or("Addr_Ring_CE");
  default_expr << lang.end_statement << endl << endl;


  default_expr << "-- InterFPGA port control" << endl;
  if (right_port==1)
    default_expr << lang.equals("Right_OE","(others => '1')")
		 << lang.end_statement << endl;
  else
    if (fpga_no != 4)
      default_expr << lang.equals("Right_OE","(others => '0')")
		   << lang.end_statement << endl;
  if (left_port==1)
    default_expr << lang.equals("Left_OE","(others => '1')")
		 << lang.end_statement << endl << endl;
  else
    if (fpga_no != 1)
      default_expr << lang.equals("Left_OE","(others => '0')")
		   << lang.end_statement << endl << endl;
    

  ////////////////////////
  // Synchronous processes
  ////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating synchronous process\n");
  StringArray* sync_sensies=new StringArray;

  sync_sensies->add("Reset");
  sync_sensies->add("Clock");

  process_expr << "-- Synchronous process" << endl;
  process_expr << lang.process("Sync",sync_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  
  process_expr << lang.if_statement << lang.test("Reset","'1'") 
	       << lang.then_statement << endl
	       // THEN
	       << "\t" << lang.equals("Master_State","Init_State") 
	       << lang.end_statement << endl 
	       << "\t" << lang.equals("Addr_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Ring_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SRCWC_State","Init_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SNKWC_State","Init_State") 
	       << lang.end_statement << endl
	       // ELSE_IF
	       << lang.elseif_statement << lang.test("Clock","'1'") 
	       << lang.and("(Clock'event)") << lang.then_statement << endl
	       << "\t" << lang.equals("Master_State","Next_Master_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Addr_State","Next_Addr_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("Ring_State","Next_Ring_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SRCWC_State","Next_SRCWC_State") 
	       << lang.end_statement << endl
	       << "\t" << lang.equals("SNKWC_State","Next_SNKWC_State") 
	       << lang.end_statement << endl
	       << lang.endif_statement << lang.end_statement << endl;
  
  process_expr << lang.end_function_scope("Sync") << endl;
  
  ////////////////////////////////////////
  // Generate master state machine process 
  ////////////////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating master state machine process\n");
  StringArray* master_sensies=new StringArray;
  master_sensies->add("Master_State");
  master_sensies->add("MemBusGrant_n");
  master_sensies->add("Go");
  master_sensies->add("Wait_AlgFin");
/*
  if (total_srcs)
    master_sensies->add("SRC_Carry");
    */
  if (total_snks)
    {
      master_sensies->add("Delay_Carry");
//      master_sensies->add("SNK_Carry");
    }


  StringArray* master_cases=new StringArray;

  // State1 -- Init_State
  // If there are no memory requirements, then bypass all the master states and idle
  ostrstream master_state1;
  if (total_srcs || total_snks)
    master_state1 << lang.if_statement << lang.test("Go","'1'") 
		  << lang.then_statement << endl
		  << "\t" << lang.equals("Next_Master_State","Wait_For_Mem")
		  << lang.end_statement << endl;
  else
    // For FPGA's without memory accesses, then no memory sequencing needed
    master_state1 << lang.if_statement << lang.test("Go","'1'") 
		  << lang.then_statement << endl
		  << "\t" << lang.equals("Next_Master_State","End_State")
		  << lang.end_statement << endl;
    
  master_state1 << lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Init_State") 
		<< lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;

  master_cases->add(master_state1.str());
  
  // State2 -- Wait_For_Mem
  ostrstream master_state2;
  master_state2 << lang.equals("MemBusReq_n","'0'") << lang.end_statement << endl
		<< lang.if_statement << lang.test("MemBusGrant_n","'0'") 
		<< lang.then_statement << endl;
  
  // Start the address counter and word counter monitors
  master_state2 << "\t" << lang.equals("Go_Addr","'1'") << lang.end_statement << endl;
  if (total_srcs)
    master_state2 << "\t" << lang.equals("Go_WC_SRC","'1'") << lang.end_statement << endl;
  if (total_snks)
    master_state2 << "\t" << lang.equals("Go_WC_SNK","'1'") << lang.end_statement << endl;
		  
  master_state2 << "\t" << lang.equals("Next_Master_State","Wait_For_Finish") 
		<< lang.end_statement << endl
		<< lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Wait_For_Mem") 
		<< lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;
  master_cases->add(master_state2.str());

  // State3 -- Wait_For_Finish
  ostrstream master_state3;
  master_state3 << lang.if_statement << lang.test("Wait_AlgFin","'0'") 
		<< lang.then_statement << endl
		<< "\t" << lang.equals("Next_Master_State","End_State") 
		<< lang.end_statement << endl 
		<< lang.else_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Wait_For_Finish") 
		<< lang.end_statement << endl
		<< "\t" << lang.equals("MemBusReq_n","'0'") 
		<< lang.end_statement << endl << endl

		// Address kickoff
		<< "\t" << "--Address kickoff signal" << endl
		<< "\t" << lang.equals("Go_Addr","'1'") << lang.end_statement << endl
		<< lang.endif_statement << lang.end_statement << ends;
  master_cases->add(master_state3.str());


  // State4 -- End_State
  ostrstream master_state4;
  master_state4 << lang.equals("Done","'1'") << lang.end_statement << endl
		<< "\t" << lang.equals("Next_Master_State","Init_State") << lang.end_statement 
		<< endl << ends;
  master_cases->add(master_state4.str());

  process_expr << "--" << endl 
	       << "-- Master process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Master",master_sensies) << endl;

  process_expr << lang.begin_scope << endl
	       << endl << "-- Defaults" << endl 
	       << lang.equals("MemBusReq_n","'1'") << lang.end_statement << endl
	       << lang.equals("Go_Addr","'0'") << lang.end_statement << endl
	       << lang.equals("Done","'0'") << lang.end_statement << endl
	       << lang.equals("Go_WC_SRC","'0'") << lang.end_statement << endl
	       << lang.equals("Go_WC_SNK","'0'") << lang.end_statement << endl;
  process_expr << lang.l_case("Master_State",master_states,master_cases);
  process_expr << lang.end_function_scope("Master") << endl;
  



  ///////////////////////////////////////////////////
  // Generate source word counter state machine logic
  ///////////////////////////////////////////////////
  if (total_srcs)
    {
      StringArray* src_wc_sensies=new StringArray;
      src_wc_sensies->add("SRCWC_State");
      src_wc_sensies->add("Go_WC_SRC");
      StringArray* src_wc_cases=new StringArray;

      // State1 -- Init_State
      ostrstream src_state1;
      src_state1 << lang.if_statement << lang.test("Go_WC_SRC","'1'") << lang.then_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Clear_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Init_State") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state1.str());
      
      // State2 -- Clear_WC
      ostrstream src_state2;
      src_state2 << lang.equals("SRC_WC_Control","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Prep_WC") << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state2.str());

      // State3 -- Prep_WC
      ostrstream src_state3;
      src_state3 << lang.equals("SRC_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SRCWC_State","Prep_WC") << lang.end_statement << endl << ends;
      src_wc_cases->add(src_state3.str());
      
      process_expr << endl << endl
		   << "--" << endl << "-- Source word counter startup process" << endl 
		   << "--" << endl
		   << lang.process("Source_WC",src_wc_sensies) << endl
		   << lang.begin_scope << endl
		   << endl << "-- Source word counter preload default state" << endl
		   << lang.equals("SRC_MUX","'0'") << lang.end_statement << endl
		   << lang.equals("SRC_WC_Control","'0'") << lang.end_statement << endl
		   << lang.l_case("SRCWC_State",src_wc_states,src_wc_cases) << endl
		   << lang.end_function_scope("Source_WC") << endl;

      // Cleanup
      delete src_wc_sensies;
      delete src_wc_cases;
    }


  ///////////////////////////////////////////////////
  // Generate sink word counter state machine logic
  ///////////////////////////////////////////////////
  if (total_snks)
    {
      StringArray* snk_wc_sensies=new StringArray;
      snk_wc_sensies->add("SNKWC_State");
      snk_wc_sensies->add("Go_WC_SNK");
      snk_wc_sensies->add(delay_carry_name);
      StringArray* snk_wc_cases=new StringArray;
      
      // State1 -- Init_State
      ostrstream snk_state1;
      snk_state1 << lang.if_statement << lang.test("Go_WC_SNK","'1'") << lang.then_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Clear_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Init_State") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << endl << ends;
      snk_wc_cases->add(snk_state1.str());
      
      // State2 -- Clear_WC
      ostrstream snk_state2;
      snk_state2 << lang.equals("SNK_WC_Control","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Delay_CE","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Wait_For_Delay") << lang.end_statement << endl 
		 << ends;
      snk_wc_cases->add(snk_state2.str());
      
      // State3 -- Wait_For_Delay
      ostrstream snk_state3;
      snk_state3 << lang.if_statement << lang.test(delay_carry_name,"'0'") << lang.then_statement << endl
		 << "\t" << lang.equals("SNK_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Prep_WC") << lang.end_statement << endl
		 << lang.else_statement << endl
		 << "\t" << lang.equals("Delay_CE","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Delay_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Wait_For_Delay") << lang.end_statement << endl
		 << lang.endif_statement << lang.end_statement << ends;
      snk_wc_cases->add(snk_state3.str());
      
      // State4 -- Prep_WC
      ostrstream snk_state4;
      snk_state4 << lang.equals("SNK_MUX","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("SNK_OK","'1'") << lang.end_statement << endl
		 << "\t" << lang.equals("Next_SNKWC_State","Prep_WC") << lang.end_statement << endl << ends;
      snk_wc_cases->add(snk_state4.str());
      
      process_expr << endl << endl
		   << "--" << endl << "-- Sink word counter startup process" << endl 
		   << "--" << endl
		   << lang.process("Sink_WC",snk_wc_sensies) << endl
		   << lang.begin_scope << endl
		   << endl << "-- Sink word counter preload default state" << endl
		   << lang.equals("SNK_MUX","'0'") << lang.end_statement << endl
		   << lang.equals("SNK_WC_Control","'0'") << lang.end_statement << endl
		   << lang.equals("SNK_OK","'0'") << lang.end_statement << endl
		   << lang.equals("Delay_CE","'0'") << lang.end_statement << endl
		   << lang.equals("Delay_MUX","'0'") << lang.end_statement << endl
		   << lang.l_case("SNKWC_State",snk_wc_states,snk_wc_cases) << endl
		   << lang.end_function_scope("Sink_WC") << endl;

      // Cleanup
      delete snk_wc_sensies;
      delete snk_wc_cases;
    }


  ///////////////////////////////////////
  // Generate address state machine logic
  ///////////////////////////////////////
  StringArray* addr_sensies=new StringArray;
  addr_sensies->add("Addr_State");
  addr_sensies->add("Go_Addr");
  addr_sensies->add("Wait_AlgFin");

  StringArray* addr_cases=new StringArray;

  // State1 -- Init_State
  ostrstream addr_state1;
  addr_state1 << lang.if_statement << lang.test("Go_Addr","'1'") << lang.then_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Clear_Addr") << lang.end_statement << endl
	      << lang.else_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Init_State") << lang.end_statement << endl 
	      << lang.endif_statement << lang.end_statement << endl << ends;
  addr_cases->add(addr_state1.str());

  // State2 -- Clear_Addr
  ostrstream addr_state2;
  addr_state2 << lang.equals("Next_Addr_State","Enable_Addr") << lang.end_statement << endl << endl

	      // Enable the preload signal (clr on the adder)
	      << "-- Address generator preload generator" << endl
	      << lang.equals("ADDR_CLR","'1'") << lang.end_statement << endl << endl
	      << "-- Initialize algorithm" << endl
	      << lang.equals("Alg_Start","'1'") << lang.end_statement << endl << ends;
  addr_cases->add(addr_state2.str());

  // State3 -- Enable_Addr
  ostrstream addr_state3;
  addr_state3 << "-- Address count enable engaged" << endl
	      << "\t" << lang.equals("Addr_Addr_CE","'1'") << lang.end_statement << endl
	      << "\t" << "-- Ring counter enable engaged" << endl
	      << "\t" << lang.equals("Go_Ring","'1'") << lang.end_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Wait_For_Finish") << lang.end_statement << endl
	      << ends;
  addr_cases->add(addr_state3.str());

  // Stat4 -- Wait_For_Finish
  ostrstream addr_state4;
  addr_state4 << lang.if_statement << lang.test("Wait_AlgFin","'0'") << lang.then_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Init_State") << lang.end_statement << endl
	      << lang.else_statement << endl
	      << "\t" << lang.equals("Next_Addr_State","Wait_For_Finish") << lang.end_statement << endl 
	      << lang.endif_statement << lang.end_statement << endl << ends;
  addr_cases->add(addr_state4.str());


  process_expr << endl << endl;
  process_expr << "--" << endl << "-- Address startup process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Addr",addr_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  process_expr << endl << "-- Address generator preload default state" << endl
	       << lang.equals("ADDR_CLR","'0'") << lang.end_statement  << endl << endl
	       << "-- Address counter count enable default state" << endl
	       << lang.equals("Addr_Addr_CE","'0'") << lang.end_statement << endl << endl
	       << "-- Ringer counter enable default state" << endl
	       << lang.equals("Go_Ring","'0'") << lang.end_statement << endl << endl
	       << "-- Algorithm initialization default state" << endl
	       << lang.equals("Alg_Start","'0'") << lang.end_statement << endl << endl
	       << "-- Source and/or Sink Word counter kickoff default state" << endl;
  if (total_srcs)
    process_expr << lang.equals("SRC_Addr_Control","'0'") << lang.end_statement << endl;
  if (total_snks)
    process_expr << lang.equals("SNK_Addr_Control","'0'") << lang.end_statement << endl;

  process_expr << lang.l_case("Addr_State",addr_states,addr_cases) << endl;
  process_expr << lang.end_function_scope("Addr") << endl;

  // Store mux activation signals in an index stream array.  The index
  // specifies the applicable Phase.
  ostrstream* phase_streams=new ostrstream[seqlen+1];
  ostrstream mux_defaults;

  ///////////////////////////////
  // Generate output mux controls
  ///////////////////////////////
  // ASSUMPTION: MUXs are organized with the LSB
  //             assigned to the first algoritm smart generator and 
  //             the MSB assigned to the last algorithm smart generator

  if (DEBUG_SEQUENCER)
    printf("Generating the output mux controls\n");
  // FIX: Should compress method with address mux controls!
  ostrstream ram_defaults;
  ostrstream ram_init_state;
  ostrstream ctrl_defaults;
  for (int mem_loop=0;mem_loop<fpga_elem->mem_count;mem_loop++)
    if ((fpga_elem->mem_connected)->query(mem_loop)==MEMORY_AVAILABLE)
      {
	Port* mem_port=arch->get_memory_handle(mem_loop);
	ACSIntArray* mem_con=fpga_elem->mem_connected;
	int mem_connected=mem_con->get(mem_loop);
	if (DEBUG_SEQUENCER)
	  printf("Memory %d con status to fpga %d is %d\n",mem_loop,fpga_no,mem_connected);
	
	if ((mem_port->port_active()) && (mem_connected==MEMORY_AVAILABLE))
	  {
	    CoreList* source_stars=mem_port->source_cores;
	    CoreList* sink_stars=mem_port->sink_cores;
	    int src_sgs=source_stars->size();
	    int snk_sgs=sink_stars->size();
	    int total_sgs=src_sgs+snk_sgs;
	    
	    int sources_reconnected=FALSE;
	    int sinks_reconnected=FALSE;
	    
	    if (mem_port->bidir_data)
	      {
		if (src_sgs+snk_sgs > 1)
		  {
		    sources_reconnected=TRUE;
		    sinks_reconnected=TRUE;
		  }
	      }
	    else
	      {
		if (src_sgs > 1)
		  sources_reconnected=TRUE;
		if (snk_sgs > 1)
		  sinks_reconnected=TRUE;
	      }
	    
	    Port_Timing* port_timing=mem_port->port_timing;
	    
	    if (DEBUG_SEQUENCER)
	      {
		int entries=(port_timing->data_activation)->population();
		printf("Times for port %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->data_activation)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->data_activation)->query(sg_loop));
		printf("Types for memory %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mem_type)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mem_type)->query(sg_loop));
		printf("List ids for port %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mem_id)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mem_id)->query(sg_loop));
		printf("Mux_lines for memory %d:\n[",mem_loop);
		for (int sg_loop=0;sg_loop<entries;sg_loop++)
		  if (sg_loop==entries-1)
		    printf("%d]\n",
			   (port_timing->mux_line)->query(sg_loop));
		  else
		    printf("%d ",(port_timing->mux_line)->query(sg_loop));
	      }
	    


	    //////////////////////////////////////
	    // Reassign mux control pin identities
	    //////////////////////////////////////
	    int imux_cnodes=0;
	    if (sources_reconnected)
	      {
		ACSCGFPGACore* data_imux=mem_port->dataimux_star;
		Pin* imux_pins=data_imux->pins;
		ACSIntArray* imux_cnode=imux_pins->query_cnodes();
		imux_cnodes=imux_cnode->population();
		imux_pins->reclassify_pin(INPUT_PIN_CTRL,INPUT_PIN_MEMORYMUX);
	      }
	    
	    int omux_cnodes=0;
	    if (sinks_reconnected)
	      {
		ACSCGFPGACore* data_omux=mem_port->dataomux_star;
		Pin* omux_pins=data_omux->pins;
		ACSIntArray* omux_cnode=omux_pins->query_cnodes();
		omux_cnodes=omux_cnode->population();
		omux_pins->reclassify_pin(INPUT_PIN_CTRL,INPUT_PIN_MEMORYMUX);
	      }
	    
	    // Establish data mux selection control line
	    char* mux_cname=new char[MAX_STR];
	    char* mux_prefix=new char[MAX_STR];
	    
	    // Maintain a phase list for each control line
	    int mode;
	    if (mem_port->bidir_data)
	      {
		mode=1;
		strcpy(mux_prefix,"_BI_");
	      }
	    else
	      mode=2;
	    

	    // FIX: Boy, this is sure ugly stuff....
	    for (int mux_loop=1;mux_loop<=mode;mux_loop++)
	      {
		mux_defaults << "--Data MUX" << mem_loop 
			     << " Controls (defaults)" << endl;
		if (!mem_port->bidir_data)
		  if (mux_loop==1)
		    strcpy(mux_prefix,"_IN_");
		  else
		    strcpy(mux_prefix,"_OUT_");
		
		int ctrl_count=imux_cnodes;
		if (mux_loop==2)
		  ctrl_count=omux_cnodes;
		
		for (int ctrl_loop=0;ctrl_loop<ctrl_count;ctrl_loop++)
		  {
		    // ASSUMPTION: mux_sline controls an entire row of muxs in the hierarchy
		    sprintf(mux_cname,"%s%s%d_%d","MEMORY_MUX",mux_prefix,mem_loop,ctrl_loop);
		    mux_defaults << lang.equals(mux_cname,"'0'") 
				 << lang.end_statement << endl;
		    
		    // FIX: And kludge up the entry in the UniSeq port definition
		    int mux_pin=seq_pins->add_pin(mux_cname,
						  0,1,
						  STD_LOGIC,
						  OUTPUT_PIN_MEMORYMUX);
		    seq_ext_signals->add_pin(mux_cname,
					     0,1,
					     STD_LOGIC,
					     OUTPUT_PIN_MEMORYMUX);
		    seq_pins->set_pinpriority(mux_pin,ctrl_loop);
		    
		    // Test to see if data is sourced or sunked on any given phase
		    for (int sg_loop=0;sg_loop < (port_timing->data_activation)->population();sg_loop++)
		      {
			int data_act=(port_timing->data_activation)->query(sg_loop);
			int sg_pos=(port_timing->mem_id)->query(sg_loop)-1;
			if (DEBUG_SEQUENCER)
			  printf("DataMux:data_act=%d, sg_pos=%d\n",
				 data_act,
				 sg_pos);
			
			if (mem_port->bidir_data)
			  {
			    if (bselcode(sg_pos,ctrl_loop)==1)
			      phase_streams[data_act] << lang.equals(mux_cname,"'1'")
						      << lang.end_statement << endl;
			  }
			else
			  {
			    if (DEBUG_SEQUENCER)
			      printf("ctrl=%d, sg_loop=%d, bselcode=%d, mem_type=%d\n",
				     ctrl_loop,
				     sg_loop,
				     bselcode(sg_pos,ctrl_loop),
				     (port_timing->mem_type)->query(sg_loop));
			    if (mux_loop==1)
			      {
				if ((bselcode(sg_pos,ctrl_loop)==1) &&
				    ((port_timing->mem_type)->query(sg_loop)==SOURCE))
				  {
				    phase_streams[data_act] << lang.equals(mux_cname,"'1'")
							    << lang.end_statement << endl;
				    if (DEBUG_SEQUENCER)
				      {
					ACSCGFPGACore* src_star=(ACSCGFPGACore*) 
					  source_stars->elem(sg_pos+1);
					printf("Source sg %s, addr_act=%d, selected on time %d\n",
					       src_star->comment_name(),
					       src_star->act_input,
					       data_act);
				      }
				  }
			      }
			    else
			      if ((bselcode(sg_pos,ctrl_loop)==1) &&
				  ((port_timing->mem_type)->query(sg_loop)==SINK))
				{
				  phase_streams[data_act] << lang.equals(mux_cname,"'1'")
							  << lang.end_statement << endl;
				  if (DEBUG_SEQUENCER)
				    {
				      ACSCGFPGACore* snk_star=(ACSCGFPGACore*) sink_stars->elem(sg_pos+1);
				      printf("Sink sg %s, selected on time %d\n",
					     snk_star->comment_name(),
					     data_act);
				    }
				}
			  }
		      }
		  }
	      }
	    
	    // Cleanup
	    mux_defaults << endl;
	    delete []mux_cname;
	    delete []mux_prefix;
	    

	    /////////////////////////////////////////////
	    // Generate address generator control signals
	    /////////////////////////////////////////////
	    // Hookup the reset line
	    mux_defaults << "-- Address Generator " << mem_loop 
			 << " Controls (defaults)" << endl;

	    // If address is active, then engage the count enabler
	    // Trap for duplicates, although will not hinder Synplicity VHDL-comp.
	    int* dup_phase=NULL;
	    int dup_count=0;
	    for (int sg_loop=0;sg_loop<total_sgs;sg_loop++)
	      {
		// Address mux switching must occur one clock period
		// prior to its necessity
		Port_Timing* port_timing=mem_port->port_timing;
		int addr_phase=(port_timing->address_activation)->query(sg_loop)-1;
		if (addr_phase < 0)
		  addr_phase=seqlen-1;
		if (DEBUG_SEQUENCER)
		  printf("sg %d will engage addr counter on phase %d, orig %d\n",
			 sg_loop,
			 addr_phase,
			 (port_timing->address_activation)->query(sg_loop));
		
		int not_dup=TRUE;
		if (dup_count>0)
		  for (int dup_loop=0;dup_loop<dup_count;dup_loop++)
		    if (addr_phase==dup_phase[dup_loop])
		      {
			not_dup=FALSE;
			break;
		      }
		if (not_dup)
		  {
		    phase_streams[addr_phase] << lang.equals("Addr_Ring_CE","'1'")
					      << lang.end_statement << endl;
		    if (dup_count==0)
		      dup_phase=new int;
		    else
		      {
			int *tmp_dup=new int[dup_count+1];
			for (int cloop=0;cloop<dup_count;cloop++)
			  tmp_dup[cloop]=dup_phase[cloop];
			delete []dup_phase;
			dup_phase=tmp_dup;
		      }
		    dup_phase[dup_count]=addr_phase;
		    dup_count++;
		  }
	      }
	    delete []dup_phase;
	    
	    // Test for MUXs
	    if (DEBUG_SEQUENCER)
	      printf("Generating the mux/ram processes\n");
	    if (mem_port->memory_test())
	      {
		// Generate control lines for each MUX "bank"
		ACSCGFPGACore* addr_mux=mem_port->get_addrmux_star();
		Pin* mux_ctrl=addr_mux->pins;
		Port_Timing* port_timing=mem_port->port_timing;
		
		// Establish addr mux selection control line
		char* mux_name=new char[MAX_STR];
		char* dmux_name=new char[MAX_STR];
		char* tmp_name=new char[MAX_STR];
		
		// Maintain a phase list for each control line
		// ASSUMPTION:Only mux switching lines are in the cnodes list
		for (int pin_loop=0;pin_loop<mux_ctrl->query_pincount();pin_loop++)
		  if (mux_ctrl->query_pintype(pin_loop)==INPUT_PIN_CTRL)
		    mux_ctrl->set_pintype(pin_loop,INPUT_PIN_ADDRMUX);
		
		Pin* addrmux_pins=addr_mux->pins;
		ACSIntArray* cnode_pins=addrmux_pins->query_cnodes();
		int cnode_count=cnode_pins->population();
		for (int ctrl_loop=0;ctrl_loop<cnode_count;ctrl_loop++)
		  {
		    // FIX: Kludge up the entry in the UniSeq port definition
		    sprintf(mux_name,"%s%d_%d","ADDR_MUX",mem_loop,ctrl_loop);
		    mux_defaults << lang.equals(mux_name,"'0'") 
				 << lang.end_statement << endl;
		    
		    int mux_pin=seq_pins->add_pin(mux_name,
						  0,1,
						  STD_LOGIC,
						  OUTPUT_PIN_ADDRMUX);
		    seq_ext_signals->add_pin(mux_name,
					     0,1,
					     STD_LOGIC,
					     OUTPUT_PIN_ADDRMUX);
		    seq_pins->set_pinpriority(mux_pin,ctrl_loop);
		    
		    // Test each star to see if it will be selected on this phase
		    for (int time_loop=0;time_loop<total_sgs;time_loop++)
		      {
			int mux_phase=(port_timing->address_activation)->query(time_loop)-1;
			
			// Mux position is skewed since pos 0 is the initial
			// address
			int mux_pos=0;
			if (mux_phase < 0)
			  {
			    mux_phase=seqlen-1;
			    mux_pos=total_sgs;
			  }
			else
			  // Since addresses are ordered
			  mux_pos=time_loop;
			

			if (DEBUG_SEQUENCER)
			  {
			    int debug=bselcode(mux_pos,ctrl_loop);
			    printf("Port %d, ctrl_loop= %d, AddrMux:addr_act=%d, mux_phase=%d, mux_pos=%d, bselcode=%d\n",
				   mem_loop,
				   ctrl_loop,
				   (port_timing->address_activation)->query(time_loop),
				   mux_phase,
				   mux_pos,
				   debug);
			  }
			
			if (bselcode(mux_pos,ctrl_loop)==1)
			  phase_streams[mux_phase] << lang.equals(mux_name,"'1'")
						   << lang.end_statement << endl;
			
		      }
		  }
		
		// Cleanup
		mux_defaults << endl;
		delete []dmux_name;
		delete []mux_name;
		delete []tmp_name;
	      }
	    mux_defaults << endl;
	    
	    
	    //////////////////////////////////
	    // Generate memory control signals
	    //////////////////////////////////
	    char* ramg_name=new char[80];
	    char* ramw_name=new char[80];
	    char* rame_name=new char[80];
	    
	    // FIX: If a single R/W control line, which is active(H)??
	    //      Defaulting to W is active high
	    sprintf(ramg_name,"%s_%d","RAMG",mem_loop);
	    sprintf(ramw_name,"%s_%d","RAMW",mem_loop);
	    sprintf(rame_name,"%s_%d","RAME",mem_loop);
	    
	    // Add pin definitions on sequencer smart generator for available
	    // control signals
	    ram_defaults << endl << "--Address Generator" << mem_loop 
			 << " RAM Control Lines (defaults)" << endl;
	    
	    if (fpga_elem->separate_rw)
	      if ((fpga_elem->read_signal_id)->get(mem_loop)!=UNASSIGNED)
		{
		  int ramg_pin=seq_pins->add_pin(ramg_name,
						 0,1,
						 STD_LOGIC,
						 OUTPUT_PIN_RAMG);
		  seq_ext_signals->add_pin(ramg_name,
					   0,1,
					   STD_LOGIC,
					   OUTPUT_PIN_RAMG);
		  seq_pins->set_pinpriority(ramg_pin,mem_loop);
		  
		  ram_defaults << lang.equals(ramg_name,"'1'") 
			       << lang.end_statement << endl;
		}
	    
	    if ((fpga_elem->write_signal_id)->get(mem_loop)!=UNASSIGNED)
	      {
		int ramw_pin=seq_pins->add_pin(ramw_name,
					       0,1,
					       STD_LOGIC,
					       OUTPUT_PIN_RAMW);
		seq_ext_signals->add_pin(ramw_name,
					 0,1,
					 STD_LOGIC,
					 OUTPUT_PIN_RAMW);
		seq_pins->set_pinpriority(ramw_pin,mem_loop);
		
		ram_defaults << lang.equals(ramw_name,"'1'") 
			     << lang.end_statement << endl;
	      }
	    
	    if ((fpga_elem->enable_signal_id)->get(mem_loop)!=UNASSIGNED)
	      {
		int rame_pin=seq_pins->add_pin(rame_name,
					       0,1,
					       STD_LOGIC,
					       OUTPUT_PIN_RAME);
		seq_ext_signals->add_pin(rame_name,
					 0,1,
					 STD_LOGIC,
					 OUTPUT_PIN_RAME);
		seq_pins->set_pinpriority(rame_pin,mem_loop);
		
		ram_defaults << lang.equals(rame_name,"'0'") << lang.end_statement << endl;
		ram_init_state << lang.equals(rame_name,"'1'") << lang.end_statement << endl;
	      }
	    
	    for (int phase_loop=0;phase_loop<mem_port->pt_count;phase_loop++)
	      {
//		printf("phase %d, mem_port->fetch_pt=%d\n",phase_loop,
//		       mem_port->fetch_pt(phase_loop));
		if (mem_port->fetch_pt(phase_loop)==SOURCE)
		  {
//		    printf("sourcing\n");
		    if ((fpga_elem->separate_rw) && 
			((fpga_elem->read_signal_id)->get(mem_loop)!=UNASSIGNED))
		      {
//			printf("read high\n");
			phase_streams[phase_loop] << "\t" 
						  << lang.equals(ramg_name,"'1'")
						  << lang.end_statement << endl;
		      }
		  }
		else if (mem_port->fetch_pt(phase_loop)==SINK)
		  {
//		    printf("sinking\n");
		    if ((fpga_elem->write_signal_id)->get(mem_loop)!=UNASSIGNED)
		      {
//			printf("write = delay?\n");
			phase_streams[phase_loop] << "\t" 
						  << lang.equals(ramw_name,"'0'") 
						  << lang.or("(") << lang.not("SNK_OK)")
						  << lang.end_statement << endl;
		      }
		  }
		else
		  {
//		    printf("othering\n");
		    phase_streams[phase_loop] << "\t" 
					      << lang.equals(rame_name,"'1'")
					      << lang.end_statement << endl;
		  }
	      }
	    
	    
	    delete []ramg_name;
	    delete []ramw_name;
	    delete []rame_name;
	  } //if ((mem_port->port_active()) && (mem_connected==MEMORY_AVAILABLE))
	mux_defaults << endl;
	
      } // if (mem_port->port_active())
  
  ///////////////////////////////////
  // Generate default control signals
  ///////////////////////////////////
  ctrl_defaults << endl << "-- Source and/or Sink Control defaults" << endl;
  if (total_srcs)
    ctrl_defaults << lang.equals("SRC_Ring_Control","'0'") << lang.end_statement << endl;
  if (total_snks)
    ctrl_defaults << lang.equals("SNK_Ring_Control","'0'") << lang.end_statement << endl;
  
  ctrl_defaults << lang.equals("Addr_Ring_CE","'0'")
		<< lang.end_statement << endl;

  ctrl_defaults << endl << "-- Completion signals" << endl;
  ctrl_defaults << lang.equals("Wait_AlgFin","'1'") << lang.end_statement << endl;
	  
  char* rsig_name=new char[MAX_STR];
  ctrl_defaults << endl << "-- Current phase signals" << endl;
  for (int loop=0;loop<seqlen;loop++)
    {
      sprintf(rsig_name,"Phase%d_Go",loop);
      ctrl_defaults << lang.equals(rsig_name,"'0'") << lang.end_statement << endl;
    }
  ctrl_defaults << endl;
  delete []rsig_name;

  ///////////////////////////////////////////////////////
  // Generate the ring (i.e., phase0 through phase(seqlen)
  ////////////////////////////////////////////////////////
  if (DEBUG_SEQUENCER)
    printf("Generating the ring process\n");

  StringArray* ring_sensies=new StringArray;
  StringArray* ring_cases=new StringArray;
  ring_sensies->add("Ring_State");
  ring_sensies->add("Go_Ring");
  if (total_snks)
    {
      ring_sensies->add("SNK_Carry");
      ring_sensies->add("SNK_OK");
    }
  else if (total_srcs) 
    ring_sensies->add("SRC_Carry");
      
  ram_init_state << ends;
  
  ostrstream ring_state;
  ring_state << ram_init_state.str()
	     << lang.equals("Addr_Ring_CE","'1'") << lang.end_statement << endl
	     << lang.if_statement << lang.test("Go_Ring","'1'") << lang.then_statement << endl
	     << "\t" << lang.equals("Next_Ring_State","Phase0") << lang.end_statement << endl
	     << lang.else_statement << endl
	     << "\t" << lang.equals("Next_Ring_State","Init_State") << lang.end_statement << endl
	     << lang.endif_statement << lang.end_statement << endl << ends;
  ring_cases->add(ring_state.str());

  char* ring_name=new char[MAX_STR];
  char* ringsig_name=new char[MAX_STR];

  // Terminate all phase streams
  for (int loop=0;loop<seqlen;loop++)
    phase_streams[loop] << ends;

  for (int loop=0;loop<seqlen;loop++)
    {
      ostrstream lring_case;
      if (loop!=0)
	{
	  if ((loop+1)==seqlen)
	    {
	      strcpy(ring_name,"Phase0");
	      lring_case << "\t" << "-- Source and/or Sink word counter control generator" << endl;
	      if (total_snks)
		lring_case << lang.if_statement << lang.test(sink_carry_name,"'0'");
	      else if (total_srcs)
		lring_case << lang.if_statement << lang.test(source_carry_name,"'0'");

	      if (total_snks || total_srcs)
		{
		  lring_case << lang.then_statement << endl
			     << "\t" << lang.equals("Wait_AlgFin","'0'") << lang.end_statement << endl
			     << "\t" << lang.equals("Next_Ring_State","Init_State") 
			     << lang.end_statement << endl
			     << lang.else_statement << endl
			     << "\t" << lang.equals("Next_Ring_State","Phase0") << lang.end_statement
			     << endl;
		  if (total_srcs)
		    lring_case << lang.equals("SRC_Ring_Control","'1'") << lang.end_statement << endl;
		  if (total_snks)
		    lring_case << lang.equals("SNK_Ring_Control","'1'") << lang.end_statement << endl;

		  lring_case << lang.endif_statement << lang.end_statement << endl;
		}
	    }
	  else
	    {
	      if (seqlen>1)
		sprintf(ring_name,"Phase%d",loop+1);
	      else
		strcpy(ring_name,"Phase0");
	      lring_case << lang.equals("Next_Ring_State",ring_name) 
			 << lang.end_statement << endl;
	    }

	  sprintf(ringsig_name,"Phase%d_Go",loop);
	      
	  lring_case << phase_streams[loop].str() 
		     << lang.equals(ringsig_name,"'1'") << lang.end_statement
		     << ends;
	}
      else
	{
	  if (seqlen > 1)
	    lring_case << lang.equals("Next_Ring_State","Phase1"); 
	  else
	    lring_case << lang.equals("Next_Ring_State","Phase0"); 

	  lring_case << lang.end_statement << endl << endl
		     << "\t" << "-- Increment word count for this sequence" << endl
		     << "\t" << phase_streams[0].str() << endl
		     << "\t" << lang.equals("Phase0_Go","'1'") << lang.end_statement << endl 
		     << ends;
	}
      ring_cases->add(lring_case.str());
    }
  delete []ring_name;
  delete []ringsig_name;
  delete []phase_streams;
  delete []source_carry_name;
  delete []sink_carry_name;
  delete []delay_carry_name;


  // Terminate streams
  mux_defaults << ends;
  ram_defaults << ends;
  ctrl_defaults << ends;

  
  process_expr << "--" << endl 
	       << "-- Ring process" << endl 
	       << "--" << endl;
  process_expr << lang.process("Ring",ring_sensies) << endl;
  process_expr << lang.begin_scope << endl;
  process_expr << mux_defaults.str();
  process_expr << ram_defaults.str();
  process_expr << ctrl_defaults.str();
  process_expr << lang.l_case("Ring_State",ring_states,ring_cases) << endl;
  process_expr << lang.end_function_scope("Ring") << endl;
  

  ///////////////////////////
  // Now propagate bit widths
  ///////////////////////////
  /*
  HWset_bw(smart_generators,
	   seq_ext_signals,
	   root_constant_signals,
	   seq_ctrl_signals);
	   */


  ////////////////////
  // Conduct stitching
  ////////////////////
  if (DEBUG_SEQUENCER)
    printf("Exporting sequencer vhdl\n");

  // Establish filenames and open file stream
  ostrstream of_filename;
  of_filename << design_directory->retrieve_extended() 
	      << seq_core->name() << ".vhd" << ends;
  ofstream fstr(of_filename.str());

  ostrstream entity_name;
  entity_name << seq_core->name() << ends;

  // Freeze streams
  preamble_expr << ends;
  default_expr << ends;
  process_expr << ends;

  // Generate code
  fstr << lang.gen_libraries(all_libraries,all_includes);
  fstr << lang.gen_entity(entity_name.str(),seq_ext_signals);
  fstr << lang.gen_architecture(entity_name.str(),
				NULL,
				BEHAVIORAL,
				seq_ext_signals,
				seq_data_signals,
				seq_ctrl_signals,
				seq_constant_signals);
//  fstr << lang.gen_components(entity_name.str(),NULL);
//  fstr << lang.gen_configurations(NULL);
//  fstr << lang.gen_instantiations(NULL,seq_ext_signals,seq_data_signals,
//				  seq_ctrl_signals,root_constant_signals);
  fstr << preamble_expr.str() << endl << endl;
  fstr << lang.begin_scope << endl;
  fstr << default_expr.str() << endl;
  fstr << process_expr.str() << endl;
  fstr << lang.end_scope << lang.end_statement << endl;

  // Cleanup
  delete seq_ext_signals;
  delete seq_ctrl_signals;
//  delete seq_data_signals;
  delete seq_constant_signals;
  delete sync_sensies;
  delete master_states;
  delete master_sensies;
  delete master_cases;
  delete addr_states;
  delete addr_sensies;
  delete addr_cases;
  delete ring_states;
  delete ring_sensies;
  delete ring_cases;
  delete src_wc_states;
  delete snk_wc_states;
}

