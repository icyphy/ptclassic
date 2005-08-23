defstar {
	name	{ SunVideo }
	domain	{ SDF }
	version	{ @(#)SDFSunVideo.pl	1.5 12/08/97}
	author	{ Luis Gutierrez }
        copyright {
Copyright (c) 1996-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF image library }
	desc {
Reads frames from the SunVideo card and outputs them as 3 matrices: one for
Y,U and V components.
	}
	htmldoc {
	}

	ccinclude { "Matrix.h", <std.h>, <stdio.h>, "Error.h", "StringList.h", "miscFuncs.h" <sys/time.h> <iostream.h>}

	output { name {output1} type {FLOAT_MATRIX_ENV} desc { Y image. } }
	output { name {output2} type {FLOAT_MATRIX_ENV} desc { U image. } }
	output { name {output3} type {FLOAT_MATRIX_ENV} desc { V image. } }
        output { name {frameIdOut} type { int } }

	state {
		name	{ frameId }
		type	{ int }
		default	{ 0 }
		desc	{ Starting frame ID value. }
	}

	state {
	    name { devNo }
	    type { int }
	    default { 0 }
	    desc { Device number of the card.  The OS will identify
		   the card as /dev/rtvc<devNo>
		 }
	    }
	state {
	    name { port }
	    type { string }
	    default { "composite-1" }
	    desc { Port to which the camera is connected.}
	}
	state {
	    name { frames_per_second }
	    type { float }
	    default { 30.0 }
	    desc { The maximum number of frames captured in one second.  The
		   actual capture rate might be lower.  The maximun is 30.
		 }
	}
	state {
	    name { decimate }
	    type { int }
	    default { 2 }
	    desc { downsample image by this factor in x and y directions. }
	}
	hinclude { "rtvc.h" }
	ccinclude { "rtvc.cc" }
	protected{
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
	    u_int width, height, bufsize;
	    u_char* buffer;
	    const char* port_str;
	    struct timeval timePtr;
	    double now, next, deltaT;
	    RTVCGrabber* grabber;
#endif // sun
	}
	constructor{
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
	    grabber = 0;
#endif // sun
	}

	begin{
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
	    if (grabber) delete grabber;
	    grabber = new RTVCGrabber((int)devNo);
	    if (!grabber) {
	      Error::abortRun(*this,"invalid device number\n");
	    }
	    port_str = (const char*) port;
	    if (grabber->command("port",(void*)port_str) < 0){
	      Error::abortRun(*this,"set port command failed,\n"
		 "Perhaps you don't have a Sun Camera on your machine?\n");
	      return;
	    }
	    u_int dec;
	    dec = (int)decimate;
	    if (grabber->command("decimate",(void*)&dec) < 0){
	      Error::abortRun(*this,"set decimation command failed\n");
	      return;
	    }
	    grabber->returnGeometry(width,height);
	    width = width/dec;
	    height = height/dec;
	    buffer = grabber->returnBuffer();
	    grabber->fps(30);
// set the camera to the highest fps rate.  The time between output frames
// for this star depends on how often the go method is called.
	    double fps = frames_per_second;
	    if (fps <= 0) Error::abortRun(*this,
			      "frames_per_second must be greater than zero.");
	    if (fps > 30.0) Error::abortRun(*this,
					   "maximum frames_per_second is 30");
	    deltaT = (1e6/fps);
	    next  = 0;
#endif // sun
	}

	method {
	    name { getTime }
	    access { protected }
	    type { double }
	    code {
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
		gettimeofday(&timePtr,0);
		return (1e6*(double)timePtr.tv_sec + (double)timePtr.tv_usec);
#else
	        return 0.0;
#endif // sun
	    }
	}

	go {
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
// Wait until it's time to grab a frame and set the next time to grab a frame
	    do{
		now = getTime();
	    }while( now < next);
	    next = now + deltaT;
	    grabber->capture();
// Create new images and fill them with data.
            LOG_NEW;
	    FloatMatrix& yColor = *(new FloatMatrix(height,width));
	    LOG_NEW;
	    FloatMatrix& uColor = *(new FloatMatrix(height,width));
	    LOG_NEW;
	    FloatMatrix& vColor = *(new FloatMatrix(height,width));
	    u_int i, j;
	    bufsize = height*width;
	    for(i = 0, j=0; i< bufsize; i++){
		yColor.entry(i) = buffer[j++];
		uColor.entry(i) = buffer[j++];
		vColor.entry(i) = buffer[j++];
	    }
// Write whole frame to output here...
	    output1%0 << yColor;
	    output2%0 << uColor;
	    output3%0 << vColor;
	    frameIdOut%0 << int(frameId);

	    frameId = int(frameId) + 1; //increment frame id
#else
	   Error::abortRun(*this, 
	        "Sorry, the SDFSunVideo star is only supported on Suns running Solaris2.x");
#endif // sun
	} // end go{}
	destructor{
#if defined(sun) && (defined(__svr4__) || defined(SYSV))
	    delete grabber;
#endif // sun
	}
    }
