/*
 * Copyright (c) 1994 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Network Research
 *      Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
static const char rcsid[] =
    "@(#) $Header$ (LBL)";

#if defined(sun) && (defined(__svr4__) || defined(SYSV))

#include "rtvc.h"
RTVCGrabber::RTVCGrabber(int devno)
{
	fid_ = -1;
	cur_video_format_ = -1;
	basewidth_ = 1;
	baseheight_ = 1;
	decimate_ = 2;
	capbase_ = 0;
	capsize_ = 0;
	state_ = 0;
	running_ = 0;
	char device[30];
	sprintf(device, "/dev/rtvc%d", devno);
	fd_ = open(device, O_RDWR);
	if (fd_ < 0) {
		perror(device);
		status_ = -1;
		return;
	}
	sprintf(device, "/dev/rtvcctl%d", devno);
	int cd = open(device, O_RDONLY);
	if (cd < 0) {
		perror(device);
		close(fd_);
		status_ = -1;
		return;
	}
	int ver = ioctl(cd, RTVC_CMD_VERSION, 0);
	if (ver < 8) {
		fprintf(stderr,
			"-vic: warning: expected rtvc driver ver.14, have ver.%d\n",
			ver);
	}
	state_ = (rtvc_video_parameters_t*)
		    mmap(0, sizeof(rtvc_video_parameters_t),
			 PROT_READ, MAP_SHARED, cd,
			 RTVC_MMAP_VIDEO_OFFSET);
	if ((int)state_ == -1) {
		perror("mmap");
		close(fd_);
		close(cd);
		status_ = -1;
		return;
	}
	close(cd);
#if defined(__svr4__) || defined(SYSV)  
	pagesize_ = sysconf(_SC_PAGESIZE);
#else
	pagesize_ = NBPG;
#endif
	arpath_ = "xilSUNWrtvc_ucode.a";
	if (access(arpath_, R_OK) == -1)
		arpath_ =
	      "/opt/SUNWits/Graphics-sw/xil/lib/pipelines/xilSUNWrtvc_ucode.a";
	else
		printf("-vic: using rtvc ucode in current directory\n");
	findFirmware();

	fpb_.firmware_message.image_skip = 1;
	fpb_.firmware_message.flags = 0;
	fpb_.max_buffers = 1;
	setInputPort(RTVC_PORT_COMPOSITE_VIDEO_1);
}

RTVCGrabber::~RTVCGrabber()
{
	close(fd_);
	if (capbase_ != 0)
		free(capbase_);
	if (state_)
		(void)munmap((caddr_t)state_, sizeof(rtvc_video_parameters_t));
}

u_char *RTVCGrabber::returnBuffer(){
	return capbuf_;
}


void RTVCGrabber::returnGeometry(u_int &base, u_int &heigth){
	base = basewidth_;
	heigth = baseheight_;
}

void RTVCGrabber::findFirmware()
{
#ifdef stat
#undef stat
#endif
	struct stat fs;

	int arfd = open(arpath_, O_RDONLY);
	if (arfd < 0) {
		perror(arpath_);
		exit(1);
	}
	fstat(arfd, &fs);
	char* bp = mmap(0, fs.st_size, PROT_READ, MAP_SHARED, arfd, 0);
	close(arfd);
	if (bp != (char*) -1) {
		if (memcmp(bp, ARMAG, SARMAG) != 0) {
			printf("rtvc: bad magic number in %s\n", arpath_);
			exit(1);
		}
		char* ep = bp + fs.st_size;
		for (char* cp = bp + SARMAG; cp < ep; ) {
			ar_hdr* ar = (ar_hdr*)cp;
			cp += sizeof(*ar);
			if (!memcmp(ar->ar_name,      "scale_cif.i/    ", 16))
				ardir_[CAPTURE_FID] = cp - bp;
			else if (!memcmp(ar->ar_name, "scale_even.i/   ", 16))
				ardir_[SMALL_CAPTURE_FID] = cp - bp;
			else if (!memcmp(ar->ar_name, "scale_full.i/   ", 16))
				ardir_[LARGE_CAPTURE_FID] = cp - bp;
			else if (!memcmp(ar->ar_name, "jpeg.i/         ", 16))
				ardir_[JPEG_FID] = cp - bp;
			else if (!memcmp(ar->ar_name, "jpeg_pal.i/     ", 16))
				ardir_[JPEG_PAL_FID] = cp - bp;
			int s = atoi(ar->ar_size);
			if (s & 1)
				++s;
			cp += s;
		}
		(void)munmap((caddr_t)bp, fs.st_size);
	}
}

int RTVCGrabber::firmwareFID() const
{
	switch (decimate_) {
	case 1:
		return (LARGE_CAPTURE_FID);
	case 4:
		return (SMALL_CAPTURE_FID);
	}
	return (CAPTURE_FID);
}

void RTVCGrabber::sendMessage()
{
	if (ioctl(fd_, RTVC_CMD_SEND_MESSAGE, &fpb_) < 0) {
	        perror("RTVC_CMD_SEND_MESSAGE");
		exit(1);
	}
}

void RTVCGrabber::halt()
{
	u_int flags = fpb_.firmware_message.flags;
	fpb_.firmware_message.flags &=~ RTVC_FIRMWARE_FLAG_GO;
	sendMessage();
	running_ = 0;
	fpb_.firmware_message.flags = flags;
}

void RTVCGrabber::run()
{
	if (ioctl(fd_, RTVC_CMD_GO, 0) < 0) {
		perror("RTVC_CMD_GO");
		exit(1);
	} else running_ = 1;
}

void RTVCGrabber::updateParameters(int both_fields)
{
	rtvc_message_t& fpb = fpb_;
	rtvc_firmware_message_t& msg = fpb.firmware_message;

	msg.flags = RTVC_FIRMWARE_FLAG_FLUSH | RTVC_FIRMWARE_FLAG_GO;
	int fullframe = decimate_ & 1;
	if (fullframe)
		msg.flags |= RTVC_FIRMWARE_FLAG_FULL_FRAMES;
	msg.return_status = 0;
	msg.pixel_stride = decimate_;
	msg.width = basewidth_;
	msg.stride = basewidth_;
	msg.height = baseheight_ / decimate_;
	msg.blanking = msg.back_porch = 0;
	msg.video_dma_interrupts = fullframe?
					(msg.height + 1) >> 1 : msg.height;
	int slintr, firstline;
	switch (cur_video_format_) {
	case RTVC_FORMAT_PAL:
		msg.scan_width = (384 << 2);
		firstline = (PAL_FIRST_SCANLINE - 7) << 1;
		slintr = PAL_FIRST_SCANLINE - 7 + PAL_HEIGHT + 2;
		break;

	default:
	case RTVC_FORMAT_NTSC:
		msg.scan_width = (320 << 2);
		firstline = (NTSC_FIRST_SCANLINE - 10) << 1;
		slintr = NTSC_FIRST_SCANLINE - 10 + NTSC_HEIGHT + 2;
		break;
	}
	fpb.field1_scanline_interrupt = slintr;
	fpb.field2_scanline_interrupt = slintr;
	memset(fpb.scanline_mask, 0, sizeof(fpb.scanline_mask));
	int ystop = baseheight_ + firstline;
	int ystride = both_fields? 1 : decimate_;
	for (int line = firstline; line < ystop; line += ystride) {
		int i, field;
		if (line & 1)
			field = RTVC_NUM_VIDEO_SCANLINE_REGISTERS / 2;
		else
			field = 0;
		i = (line + 1) >> 1;
		fpb.scanline_mask[field + (i >> 5)] |= 1 << (i & 0x1f);
	}
}


void RTVCGrabber::setInputPort(int newport)
{
	if (running_)
		halt();

	rtvc_video_parameters_t param;
	memset(&param, -1, sizeof(param));
	param.video_port = newport;
	if (ioctl(fd_, RTVC_CMD_SET_VIDEO, &param) < 0) {
		perror("RTVC_CMD_SET_VIDEO");
		fprintf(stderr, "couldn't set port\n");
		exit(1);
	}
	int new_video_format = state_->video_format;

	if ((new_video_format < 0) || (new_video_format > RTVC_FORMAT_MAX))
		new_video_format = RTVC_FORMAT_UNKNOWN;

	if (state_->video_format == 1) {
		max_fps_ = 25;
		basewidth_ = PAL_WIDTH;
		baseheight_ = PAL_HEIGHT;
	} else {
		max_fps_ = 30;
		basewidth_ = NTSC_WIDTH;
		baseheight_ = NTSC_HEIGHT;
	}
	capsize_ = basewidth_ * baseheight_ * 3;
	if (capbase_ == 0)
		capbase_ = (u_char*)malloc(capsize_ + pagesize_);
	else
		capbase_ = (u_char*)realloc(capbase_, capsize_ + pagesize_);
	int off = (int)capbase_ & (pagesize_ - 1);
	capbuf_ = capbase_ + (pagesize_ - off);
	cur_video_format_ = new_video_format;
}

void RTVCGrabber::loadFirmware(int fid)
{
	rtvc_program_t program;
	struct stat fs;

	fid_ = -1;
	if (ioctl(fd_, RTVC_CMD_RESET, 0) < 0) {
		perror("RTVC_CMD_RESET");
		exit(1);
	}
	int arfd = open(arpath_, O_RDONLY);
	if (arfd < 0) {
		perror(arpath_);
		exit(1);
	}
	fstat(arfd, &fs);
	char* cp = mmap(0, fs.st_size, PROT_READ, MAP_SHARED, arfd, 0);
	close(arfd);
	if (cp != (char*) -1) {
		program.program = (u_int32_t*)(cp + ardir_[fid]);
		program.buffer = 0;
		program.error = 0;

		int rc = ioctl(fd_, RTVC_CMD_PROGRAM, &program);
		if (rc < 0) {
			perror("RTVC_CMD_PROGRAM");
			exit(1);
		}
		(void)munmap((caddr_t)cp, fs.st_size);
		fid_ = fid;
	}
}

struct scale_fpb {
	u_int num_lines;
	u_int scanline_dmas;
	u_int even_word_stride;
	u_int buffer_size;
};

void RTVCGrabber::set_size_nachos(int w, int h)
{
	int fid = firmwareFID();
	if (fid != fid_) {
		loadFirmware(fid);
		run();
	}
	updateParameters();
	u_int bytes_per_image = w * h * 3;
	fpb_.buffer_byte_size = (bytes_per_image + 3) &~ 3;
	scale_fpb* spb = (scale_fpb*)fpb_.firmware_message.data;
	spb->num_lines = h * 2;
	spb->scanline_dmas = (basewidth_ >> 7) - 1;
	spb->even_word_stride = (decimate_ >> 1) << 2;
	spb->buffer_size = bytes_per_image;
	sendMessage();
}



int RTVCGrabber::command(char * arg1, void * arg2)
{
        if (strcmp(arg1, "port") == 0) {
	         int newport;
	         if (strcasecmp((char*)arg2, "composite-1") == 0)
		        newport = RTVC_PORT_COMPOSITE_VIDEO_1;
	         else if (strcasecmp((char*)arg2, "composite-2") == 0)
	                newport = RTVC_PORT_COMPOSITE_VIDEO_2;
	         else if (strcasecmp((char*)arg2, "s-video") == 0)
		        newport = RTVC_PORT_S_VIDEO;
		 else return -1;
                 setInputPort(newport);
                 /* video format may have changed when port changed */
	        setsize();
	        return 0;
	        //return (TCL_OK);
	} else if (strcmp(arg1, "decimate") == 0) {
	       int d = *((int*)arg2);
	       if (d <= 0 ) {
		    //	Tcl& tcl = Tcl::instance();
		    //	tcl.resultf("%s: divide by zero", argv[0]);
		    //	return (TCL_ERROR);
		    return -1;
		}
		decimate_ = d;
	        setsize();
	        return 0;
		//return (TCL_OK);
	} 
	return -1;
//	return (Grabber::command(argc, argv));
}

void RTVCGrabber::fps(int f)
{
	if (f <= 0)
		f = 1;
	else if (u_int(f) > max_fps_)
		f = max_fps_;
	/* convert to skip count then back */
	int sc = max_fps_ / f;
	fpb_.firmware_message.image_skip = sc - 1;
	if (running_)
		sendMessage();
	//Grabber::fps(max_fps_ / sc);
}

void RTVCGrabber::setsize(){

	int w = basewidth_ / decimate_;
	int h = baseheight_ / decimate_;
	set_size_nachos(w, h);
}

int RTVCGrabber::capture()
{
	int cc = pread(fd_, (void*)capbuf_, capsize_,
		       RTVC_READ_CAPTURED_OFFSET);
	return (cc);
}

#ifdef NEEDED_FOR_SUNVIDEO_STAR

void RTVCGrabber::stop()
{
	unlink(); 
}

void RTVCGrabber::start()
{
	setsize();
	link(fd_, TK_READABLE);
	double now = gettimeofday();
	frameclock_ = now;
	nextframetime_ = now + tick(grab());
}

void RTVCGrabber::dispatch(int)
{
	double now = gettimeofday();
	if (nextframetime_ > now) {
		/*
		 * the frame is too early & we want to flush it.
		 * unfortunately, the sunvideo driver doesn't provide
		 * any mechanism for flushing frames.  So we do an inefficient
		 * 2MB read to get rid of the sucker.
		 */
		u_char buf[768*576*3 + 8192];
		pread(fd_, (void*)((u_int)(buf + pagesize_) &~ (pagesize_ - 1)),
		      768*576*3, RTVC_READ_CAPTURED_OFFSET);
	} else
		nextframetime_ = tick(grab()) + now;
}

/*
 * convert timestamp of most recent frame to 32 bit rtp timestamp format.
 * this must be called after 'capture' and before the next read.
 */
u_int32_t RTVCGrabber::media_ts()
{
	/*
	 * the conversion constant is 90000 / 10^9 but this is
	 * multiplied by 2^32 so we can multiply rather than
	 * divide to get the scaled ts.
	 */
	unsigned long long ts = state_->capture.timestamp * 386547;
	u_int32_t t = ts >> 32;
	return (t + offset_);
}

/*
 * return the media timestamp corresponding to the current unix time.
 * we use that fact that the media ts is an hrtime.
 */
u_int32_t RTVCGrabber::ref_ts()
{
	unsigned long long ts = (unsigned long long)gethrtime() * 386547;
	u_int32_t t = ts >> 32;
	return (t + offset_);
}

#endif /* NEEDED_FOR_SUNVIDEO_STAR */
#endif /* sun */

