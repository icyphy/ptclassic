defstar {
	name { HostOut }
	domain { CG56 }
	desc { Data output. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
Output data from DSP to host via host port.
        }
	input	{
		name { input }
		type { fix }
		}
	state {
		name { samplesConsumed }
		type { int }
		desc { number of samples consumed. }
		default { 1 }
	}
	state {
		name { samplesOutput }
		type { int }
		desc { number of samples outputed.  }
		default { 1 }
	}
 	state  {
		name { blockOnHost }
		type { string }
		desc {  }
		default { "yes" }
	}
 	state  {
		name { command }
		type { string }
		desc {  }
		default { "" }
	}
        start {
               !$val(command);
               input.setSDFParams(int(samplesConsumed),int(samplesConsumed)-1);
        }

        codeblock(yeshostBlock) {
$label(l)
        jclr    #m_htde,x:m_hsr,$label(l)
        jclr    #0,x:m_pbddr,$label(l)
        movep   $ref(input),x:m_htx
	}
        codeblock(elsehostBlock) {
        jclr    #m_htde,x:m_hsr,$label(l)
        jclr    #0,x:m_pbddr,$label(l)
        movep   $ref(input),x:m_htx
$label(l)
        }
        codeblock(nohostBlock) {
        jclr    #m_htde,x:m_hsr,$label(l3)		      
        jclr    #0,x:m_pbddr,$label(l3)
        move    #$addr(input),r0
        do      #$val(samplesOutput),$label(l)
$label(l2)
        jclr    #m_htde,x:m_hsr,$label(l2)
        jclr    #0,x:m_pbddr,$label(l2)
        movep   x:(r0)+,x:m_htx
$label(l)
$label(l3)
        }
        codeblock(done) {
        move    #$addr(input),r0
        do      #$val(samplesOutput),$label(l)
$label(l2)
        jclr    #m_htde,x:m_hsr,$label(l2)
        jclr    #0,x:m_pbddr,$label(l2)
        movep   x:(r0)+,x:m_htx
$label(l1)
$label(l3)
        }        
        go { 
        const char* p=blockOnHost;		
        if (samplesConsumed==1) {
                if(p[0]=='y` || p[0]=='Y') 
	              gencode(yeshostBlock);
		else
	              gencode(elsehostBlock);
		}
        else {
               if (p[0]=='n' || p[0]== 'N')
	              gencode(nohostBlock);
	       else
	              gencodeblock(done);
 	}

	execTime { 
		return 2;
	}
 }
