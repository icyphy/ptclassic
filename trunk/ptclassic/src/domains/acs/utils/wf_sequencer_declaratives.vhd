--Master state machine states
type Master_States is (
	Init_State,
	Wait_For_Mem,
	Wait_For_Finish,
	End_State);

--Address state machine states
type Addr_States is (
	Init_State,
	Clear_Addr,
	Start_Sequencer,
	Wait_For_Finish);

--State machine signals
signal Master_State:Master_States;
signal Next_Master_State:Master_States;
signal Addr_State:Addr_States;
signal Next_Addr_State:Addr_States;

