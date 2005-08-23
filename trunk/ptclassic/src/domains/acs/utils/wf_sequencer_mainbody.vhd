-- Synchronous process
Sync: process (
	Reset,
	Clock)	

begin 
if (Reset='1') then
	Master_State <= Init_State;
	Addr_State <= Init_State;
elsif (Clock='1') and (Clock'event) then
	Master_State <= Next_Master_State;
	Addr_State <= Next_Addr_State;
end if;
end process Sync;

--
-- Master process
--
Master: process (
	Master_State,
	MemBusGrant_n,
	Go,
	Alg_Done)

begin 

-- Defaults
MemBusReq_n <= '1';
Go_Addr <= '0';
Done <= '0';
Seq_Reset <= '0';

case Master_State is
  when Init_State =>
	Seq_Reset <= '1';
	if (Go='1') then
	  Next_Master_State <= Wait_For_Mem;
	else
	  Next_Master_State <= Init_State;
	end if;
when Wait_For_Mem =>
	MemBusReq_n <= '0';
	if (MemBusGrant_n='0') then
	  Go_Addr <= '1';
	  Next_Master_State <= Wait_For_Finish;
	else
	  Next_Master_State <= Wait_For_Mem;
	end if;
when Wait_For_Finish =>
	if (Alg_Done='1') then
	  Next_Master_State <= End_State;
	else
	  Next_Master_State <= Wait_For_Finish;
	  MemBusReq_n <= '0';

	  --Address kickoff signal
	  Go_Addr <= '1';
	end if;
when End_State =>
	Seq_Reset <= '1';
	Done <= '1';
	Next_Master_State <= Init_State;

end case;
end process Master;



--
-- Address startup process
--
Addr: process (
	Addr_State,
	Go_Addr,
	Alg_Done)

begin 

-- Address generator preload default state
ADDR_CLR <= '0';

-- Algorithm initialization default state
Alg_Start <= '0';

case Addr_State is
when Init_State =>
	if (Go_Addr='1') then
	  Next_Addr_State <= Clear_Addr;
	else
	  Next_Addr_State <= Init_State;
	end if;

when Clear_Addr =>
	Next_Addr_State <= Start_Sequencer;

	-- Address generator preload generator
	ADDR_CLR <= '1';

when Start_Sequencer =>
	Next_Addr_State <= Wait_For_Finish;

	-- Initialize algorithm
	Alg_Start <= '1';

when Wait_For_Finish =>
	if (Alg_Done='1') then
	  Next_Addr_State <= Init_State;
	else
	  Next_Addr_State <= Wait_For_Finish;
	end if;

end case;

end process Addr;
