//***********************************************************************
//                              DETCPSender.pl
//***********************************************************************
defstar {
 	name {TCPSender }
	domain { DE }
	author { Dorgham Sisalem}
        copyright { SEE $PTOLEMY_SIMULATIONS/tcp/copyright }

	desc {
        This star is a TCP source that is based on the 4.3Tahoe BSD TCP
        implementation and fulfills the following requirements:
        Sliding window mechanism: It only sends if the receiver has enough
                 space. Otherwise it waits until the receiver has freed some
                 buffer and reopened the transmission window.
        Timeout: If an acknowledgment for a specific packet has not arrived
                 after some time, it is resent.
        Round trip time estimation: Using the Karn's algorithm.
        Slow start: For the beginning of the transmission and after a timeout.
        Congestion avoidance: A slow increase of the window size of 1/cwnd
        Fast retransmission: After receiving 3 duplicate acks the source 
                             retransmits the lost packet. 
	}

//***********************************************************************
//                      Define Ports
//***********************************************************************
	input { name { ack}
                type { message}
	        desc { Acknowledgment packet}
	}
	input { name { input }
                type { message}
	        desc { new data packets}
        }
	input { name { timer}
                type { message}
	}

	output { name { demand}
                 type { int}
	         desc { get demand number of bytes from the WindowBuf}
        }
	output { name { output }
                 type { message }
	         desc { send data packets}
	}
	output { name { timeout}
                 type { int}
	         desc { last calculated round trip time}
	}

//***********************************************************************
//                      Define States
//***********************************************************************

	defstate {
		name { Win_Size}
		type { int  }
		default { 10 }
		desc {maximum transmission window. }
	}
	defstate {
		name { MSS}
		type { int  }
		default { 1 }
		desc {Maximum Segment Size }
	}
	defstate {
		name { init_time}
		type { float }
		default { 1 }
		desc {initial value for RTT}
	}
	defstate {
		name { Rate}
		type { float }
		default { 1 }
		desc {the sending rate of the source}
	}
	defstate {
		name { Grain}
		type { float }
		default { 2 }
		desc {the number of clock ticks in a second}
	}
	defstate {
		name { debug}
		type { int }
		default { 0 }
		desc {0 output debug infos}
	}

	hinclude { "TCPPacket.h" ,<math.h>}

	defstate {
		name { fileName }
		type { string }
		default { "<stdout>" }
		desc { Filename for output }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream *p_out;
	}

	protected { long count;
		    long Last_Sent,counter;
		    long Limit,state,Retransmission;
		    long Ack;
		    const long STOP=0;
		    const long GO=1;
	            Envelope* buffer;
		    long Last_Ack,Last_Win,Last_Limit;
		    float cwnd,ssthresh,Win;
		    long Slow_Start,Congestion_Avoidance,Time_Out;
		    long Retransmitted,Timer,First_Time;
		    float RTO,A,D,Start_Time;
		    long Last_Seq,buffered,last_seq,last_retransmitted;
		    long SEQ,Last_Retransmit,send_state;
		    float *timer_buf;
		    int ret;
		  }

//*****************************************************************************/
// initialization: cwnd=0;
//                 start with slow start
//                 RTO is set by the user
//*****************************************************************************/

	setup { delayType                                = FALSE;
	        cwnd                                     =  MSS ;
		Limit                                    = long(cwnd-1);
		state                                    = GO;
		counter                                  = 0;
	        Retransmission=ret                       = 0;
		Ack=Last_Ack=Last_Win                    = -1;
		Last_Limit                               = 0;
		Slow_Start                               = 1;
		Congestion_Avoidance=Time_Out            = 0;
		First_Time                               = 1;
		Timer=Retransmitted                      = -2;
		RTO                                      = init_time;
		Last_Seq                                 = -1;
		send_state                               = 1;
		ssthresh				 = Win_Size;
		Win_Size                                 = Win_Size/MSS;
	        last_seq				 =-1;
              }

//*****************************************************************************/
 // allocate buffer space for the local window
//*****************************************************************************/

	begin  {          //initialize the local window
                if(buffer) {LOG_DEL; delete [Win_Size] buffer; buffer=NULL;}
	        LOG_NEW;
	        buffer=new Envelope[Win_Size];
                if(timer_buf) {LOG_DEL; delete [Win_Size] timer_buf;
		                   timer_buf=NULL;}
	        LOG_NEW;
	        timer_buf=new float[Win_Size];
                if(p_out){LOG_DEL; delete p_out;p_out=NULL;}
		LOG_NEW; p_out = new pt_ofstream(fileName);
		}

//****************************************************************************/
// set scheduling priorities and initialize the memory
//****************************************************************************/

	constructor { 
		      buffer=NULL;
		      p_out=NULL;
	              timer_buf=NULL;
		      if(buffer){LOG_DEL; delete [Win_Size] buffer;
		                   buffer=NULL;}
	              if(timer_buf) {LOG_DEL; delete [Win_Size] timer_buf; 
		                   timer_buf=NULL;}
	              ack.before(input);
		      ack.before(timer);
		      input.before(timer);
              }
        wrapup {
	        if(p_out){LOG_DEL; delete p_out; p_out=NULL;}
		if(buffer){ LOG_DEL; delete [Win_Size] buffer;
		                   buffer=NULL;}
	        if(timer_buf) {LOG_DEL; delete [Win_Size] timer_buf;
		                   timer_buf=NULL;}
		}
	destructor { if(p_out){LOG_DEL; delete p_out; p_out=NULL;}
		if(buffer) {LOG_DEL; delete [Win_Size] buffer;
		                   buffer=NULL;}
	        if(timer_buf) {LOG_DEL; delete [Win_Size] timer_buf;
		                   timer_buf=NULL;}
	}
//**********************************************************************
//                    GO
//**********************************************************************
	go {
                pt_ofstream& OUTPUT = *p_out;

                completionTime= arrivalTime;
		if(ack.dataNew)  // an acknowlegment has arrived
 		{
		  Envelope Envp;
		  ack.get().getMessage(Envp);
		  const TCPPacket * packetPtr=(const TCPPacket *)
		    Envp.myData();
		  Ack=packetPtr->readAck();
		  long Seq=packetPtr->readSeq();
if(debug)
OUTPUT<<" &&&&ACK "<<Seq<<" cwnd "<<cwnd<<" WIN "<<Win<<" RETRANSMISSION "<<Retransmission<<" at "<<completionTime<<"\n";
//***************************************************************************/
// Ignore Acks for falsely retransmitted packets -i.e. packets that are
// retransmitted after a timeout even though the ack is on the way, or 
// duplicate acks after the 3rd. one was received.
//***************************************************************************/

		  if((Ack<Last_Ack)||((ret>2)&&(Ack==last_retransmitted)))
	           {
		      return;
		   }
	         if(Ack!=Last_Ack)
	           ret=0;
                 if(Ack>Last_Ack)
                     last_seq=packetPtr->readSeq();
		 Win=packetPtr->readWin();

//***************************************************************************/
// measure round trip time only if there was no timeout for this sequence
// number.
//***************************************************************************/

		  if((Timer!=-2)&&((Seq==Timer)||(Seq==Timer+1))
	                  &&(Retransmitted!=Seq)&&(Retransmitted!=Seq-1))
	          {

                    float M=arrivalTime-Start_Time;
if(debug)
OUTPUT<<" M "<<M; 
		     M=M*(float)Grain;
		     int m_int=ceil(M);
		     M=(float)m_int;
	             M=M/(float)Grain;
	             if(First_Time)   // calculate the first RTO
		     {
		       A=M+0.5;
		       D=A/2;
		       RTO=A+(4*D);
		       First_Time=0;
		     }
		     else
		     {               // calculate RTO
		       float  Err=M-A;
		       A=A+(0.125*Err);
		       if(Err<0)
		          Err*=-1;
		       D=D+(0.25*(Err-D));
		       RTO=A+(4*D);
		     }
		     Timer=-2;       //a new RTT measurement can start

                     //Round up the time measurements to the required grain
		     RTO=RTO*(float)Grain;
		     m_int=ceil(RTO);
		     RTO=(float)m_int;
	             RTO=RTO/(float)Grain;
		     if(RTO<(Grain+Grain))
		           RTO+Grain+Grain;
if(debug)
OUTPUT<<" RTO "<<RTO<<"\n"; 
		     if(RTO>64)     //set an upper limit of 64
		        RTO=64;
		     //send latest round trip estimation

		     timeout.put(completionTime)<<RTO;
	          }

//***************************************************************************/
// Increase cwnd by the size of one packet for each acknowledged packet
//***************************************************************************/
	          if(Slow_Start&&(Win>cwnd))
		  {
		     cwnd+=MSS;
		     if(cwnd<=0) cwnd=MSS;
		     if(cwnd>ssthresh) {cwnd=ssthresh; Slow_Start=0;}

		  }

//***************************************************************************/
// Increase cwnd by the size of one packet for each RTT while in the
// congestion avoidance state.
//***************************************************************************/

		  if(cwnd<Win)
	          {
	              if((cwnd>=ssthresh)&&Congestion_Avoidance)
		      {
		        Slow_Start=0;
			float cwnd_pack=cwnd/(float)MSS;
			cwnd_pack+=(1/cwnd_pack);
		        cwnd_pack*=MSS;
			cwnd=cwnd_pack;
		      }
		      Win=cwnd;
		  }

		  Limit=Win+Ack;  //update the sliding window	
//***************************************************************************/
// If 3 duplicate Acks were received then retransmit the lost packet and go into
// slow start.
//***************************************************************************/
		  if((Ack==Last_Ack)&&Last_Win) //duplicate Acks ?
	          {
		      ret++;
	              if(ret<3)
		         return;
	              if(ret==3)
		      {
	                Retransmission=ret;
			Timer=-2;         //stop the RTO calculation
		        send_state=-1;
			Limit=0;
			Retransmitted=Seq+1;

//****************************************************************************
//clear all the timeout stamps in order to avoid timeout handling for packets
//that were transmitted but can not be acknowledged until a retransmitted
//packet reaches the receiver
//****************************************************************************
			long i;
			for(i=0;i<Win_Size;i++)
		           timer_buf[i]=-1;
	                timer_buf[(Seq+1)%Win_Size]=completionTime+RTO;
if(debug)
OUTPUT<<" (Seq+1)%Win_Size= "<<(Seq+1)%Win_Size<<" RTO "<<completionTime+RTO<<"\n";
		        //read the lost packet and send it
                        output.put(completionTime)<<buffer[(Seq+1)%Win_Size];
	                last_retransmitted=Ack;
		        demand.put(completionTime)<<(int)STOP;
		        Last_Seq=Seq+1;

			Slow_Start=1;  //go into slow start
 		        Congestion_Avoidance=1; //start the congestion
		                                //avoidance state after
					        //the slow start

			if(cwnd<Win)   //reset the ssthresh value
		        {
		           ssthresh=cwnd/2;
			   ssthresh-=(long)ssthresh%MSS;
		        }
		        else
		        {
		           ssthresh=Win/2;
			   ssthresh-=(long)ssthresh%MSS;
		        }
		        if(ssthresh<MSS) ssthresh=MSS+MSS; // set the lower
		                                           // limit of ssthresh
			cwnd=MSS;           // initialize cwnd
		       
		      }
	          }
	          else
	          {
                    Last_Ack=Ack;
		    Last_Win=Win;
		    send_state=1;
		   
		    if(Retransmitted>=Seq)
		         Time_Out=0;      //reset the Time_Out state
		  }
		  if(Win<MSS) // avoid silly windows
		  {
		    state=STOP;
		    demand.put(completionTime)<<(int)state;
		  }
//All packets waiting for acknowledgements were achnowledged so clear the flags	
	          if((Retransmission>=3)&&
			  ((last_retransmitted>=Last_Sent)||(Ack>=Last_Sent)))
			{
				Retransmitted=-2;
				Retransmission=0;
			}
//There are still some packets waiting for acks after a loss so resend them
	          if((Ack<Last_Sent)&&(Retransmission>=3))
	          {
if(debug)
OUTPUT<<" Ack "<<Ack<<" Last_Sent "<<Last_Sent<<"\n";
			if(last_retransmitted<Ack)
				last_retransmitted=Ack;
			while((Limit>last_retransmitted)&&
				(last_retransmitted<Last_Sent))
			{
			       float tmp=(float)last_retransmitted/(float)MSS;
if(debug)
OUTPUT<<" RESEND "<<tmp<<" last_RET "<<last_retransmitted<<"\n";
	                       timer_buf[(int)tmp%Win_Size]=completionTime+RTO;
                     	       output.put(completionTime)<<buffer[(int)tmp%Win_Size];
				
				last_retransmitted+=MSS;
			}
			if(last_retransmitted>=Last_Sent)
			{
				Retransmitted=-2;
				Retransmission=0;
			}
			
                  }
		  if(Limit)  //update the sliding window size
		  {
		     demand.put(completionTime)<<(int)Limit;//inform the buffer
		                                       // of the possibility to
						       // send
		  }
 	      }

//****************************************************************************
//                      SEND NEW DATA
//****************************************************************************

		if(input.dataNew)
		{
	          //read new data and save it in buffer
		  input.get().getMessage(buffer[counter%Win_Size]);
	          const TCPPacket* packet=(const TCPPacket *)
	                      buffer[counter%Win_Size].myData();
		  long num=packet->readNum();
		  long seq=packet->readSeq();
if(debug)
OUTPUT<<" SEND "<<seq<<" NUM "<<num<<" at "<<completionTime<<"\n";
		  output.put(completionTime)<<buffer[counter%Win_Size];

		  if(num>=Limit-MSS) //sliding window full?
		  {
		    state=STOP;
		    demand.put(completionTime)<<(int)state;
		  }
		  if(Timer==-2) // if no RTT is being measured, start a new
	                        // measurement
	          {
	            Start_Time=arrivalTime;
		    Timer=seq;
		  }
	          timer_buf[seq%Win_Size]=completionTime+RTO;//set the time at 
	                                         //which a timeout should occur
		  Last_Sent=num;
		  counter++;

		}

//****************************************************************************
//                       Timeout for a packet has occured
//****************************************************************************

		if(timer.dataNew)
		{
	          Envelope envp;
		  timer.get().getMessage(envp);
	          const TCPPacket* packet=(const TCPPacket *)
	                      envp.myData();
	          long seq=packet->readSeq();
		  long num=packet->readNum();

	          if(num>Last_Ack) // has this packet already been acknowledged
	          {

//****************************************************************************
//Check if the timeout occured at the right time. If it is not correct then 
// this indicates that the the packet was retransmitted or can not be 
//acknowledged now as the receiver is waiting for a retransmission
//****************************************************************************

	             if((completionTime+(0.01)<timer_buf[seq%Win_Size])||
		     	(timer_buf[seq%Win_Size]<0))
		        	return;
		    Retransmitted=last_seq+1;//register the packet that got lost
if(debug)
OUTPUT<<" TIMEOUT "<<seq<<" Resend "<<Retransmitted<<" NUM "<<num<<" at "<<completionTime<<"\n";
//****************************************************************************
//clear all the timeout stamps in order to avoid timeout handling for packets
//that were transmitted but can not be acknowledged until a retransmitted
//packet reaches the receiver
//****************************************************************************
   	             long i;
		     for(i=0;i<Win_Size;i++)
		            timer_buf[i]=-1;
	             Retransmission=4;
		     Last_Seq=seq+1;
                     last_retransmitted=((float)Retransmitted)*(float)MSS;

		     Timer=-2;         //stop the RTO calculation
		     send_state=-1;
		     Limit=0;
		     Slow_Start=1;  //go into slow start
		     Congestion_Avoidance=1; //start the congestion
		                                //avoidance state after
					        //the slow start
		     if(cwnd<Win)   //reset the ssthresh value
		     {
		           ssthresh=cwnd/2;
			   ssthresh-=(long)ssthresh%MSS;
		     }
		     else
		     {
		           ssthresh=Win/2;
			   ssthresh-=(long)ssthresh%MSS;
		     }
		     if(ssthresh<=MSS) ssthresh=MSS+MSS;// set the lower
		                                       // limit of ssthresh
			cwnd=MSS;                      // initialize cwnd
		     demand.put(completionTime)<<(int)STOP;
		     Time_Out=1;
		     RTO*=2;     // exponential backoff
		     if(RTO>64)
		        RTO=64;
		     timeout.put(completionTime)<<RTO;// update the time out
		                                      // value

	             timer_buf[seq%Win_Size]=completionTime+RTO;
		     //resend the last unacknowledged packet
                    output.put(completionTime)<<buffer[(last_seq+1)%Win_Size];
		  }
		}
	} // end go
} // end defstar
