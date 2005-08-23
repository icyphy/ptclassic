#ifndef RTVC_H_
#define RTVC_H_

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ar.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

//#include "module.h"
//#include "Tcl.h"

//#include <tk.h>
#include <sys/param.h>
#include <sys/types.h>
//#include "grabber.h"
//#include "crdef.h"
//#include "iohandler.h"
//#include "device-input.h"
//#include "transmitter.h"

typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef int int32_t;

/*
 * rtvc driver ioctls & data structures.
 */
#define	RTVC_IOC		('r' << 8)
#define	RTVC_CMD_PROGRAM	(RTVC_IOC | 0)	/* download firmware */
#define	RTVC_CMD_VERSION	(RTVC_IOC | 1)	/* return driver version */
#define	RTVC_CMD_SET_VIDEO	(RTVC_IOC | 2)	/* set video attributes */
#define	RTVC_CMD_GET_VIDEO	(RTVC_IOC | 3)	/* get video attributes */
#define	RTVC_CMD_SEND_MESSAGE	(RTVC_IOC | 4)	/* talk to firmware */
#define	RTVC_CMD_RESET		(RTVC_IOC | 5)	/* reset board */
#define	RTVC_CMD_HALT		(RTVC_IOC | 6)	/* halt cl4000 */
#define	RTVC_CMD_GO		(RTVC_IOC | 7)	/* resume cl4000 */

struct rtvc_capture_t {
	u_int32_t byte_count;		/* buffer size on entry,
					 * image size on return */
	u_int32_t frame_number;		/* (relative to start of capture) */
	hrtime_t timestamp;		/* when image was captured */
};

struct rtvc_video_parameters_t {
	int32_t video_port;
	int32_t video_format;
	int32_t video_device;		/* 812 = Brooktree; 7191 = Philips */
	int32_t video_status;		/* video device hardware status */
	int32_t video_buffers;		/* number of video buffers */
	int32_t video_images;		/* video images available */
	int32_t buffer_alignment;	/* capture buffer alignment */
	int32_t padding;
	rtvc_capture_t capture;		/* last capture state */
	int32_t video_registers[29];
};

#define RTVC_PORT_S_VIDEO              0
#define RTVC_PORT_COMPOSITE_VIDEO_1    1
#define RTVC_PORT_COMPOSITE_VIDEO_2    2

#define RTVC_FORMAT_UNKNOWN	0
#define RTVC_FORMAT_PAL		1
#define RTVC_FORMAT_NTSC	2
#define RTVC_FORMAT_MAX		2

struct rtvc_firmware_message_t {
	u_int32_t flags;
	u_int32_t pixel_stride;
	u_int32_t return_status;
	int32_t image_skip;
	u_int32_t video_dma_interrupts;
	u_int32_t image_buffers;      
	u_int32_t dmem_image_info_end;
	u_int32_t width;
	u_int32_t height;
	u_int32_t scan_width;
	u_int32_t stride;
	u_int32_t back_porch;
	u_int32_t blanking;
	u_int32_t unused[3];
	u_int8_t data[1024];
};

#define RTVC_FIRMWARE_FLAG_GO		0x0001
#define RTVC_FIRMWARE_FLAG_FLUSH	0x0002
#define RTVC_FIRMWARE_FLAG_LOGO		0x0004
#define RTVC_FIRMWARE_FLAG_FULL_FRAMES	0x0008
#define RTVC_FIRMWARE_FLAG_DVMA		0x0010

#define	RTVC_NUM_VIDEO_SCANLINE_REGISTERS	20

struct rtvc_message_t {
	u_int32_t scanline_mask[RTVC_NUM_VIDEO_SCANLINE_REGISTERS];
	u_int32_t field1_scanline_interrupt;
	u_int32_t field2_scanline_interrupt;
	u_int32_t buffer_byte_size;
	u_int32_t max_buffers;
	rtvc_firmware_message_t firmware_message;
};

struct rtvc_program_t {
	u_int32_t *program;
	u_int32_t *buffer;
	u_int32_t *error;
	int32_t command;
};

#define	RTVC_MMAP_VIDEO_OFFSET		((off_t)0x00800000)
#define	RTVC_READ_CAPTURED_OFFSET	((off_t)0x01000000)


class RTVCGrabber {
    public:
	RTVCGrabber(int devno);
	virtual ~RTVCGrabber();
  	void halt();
        void run();
  	virtual int command(char * arg1, void* arg2);
	virtual void fps(int);
	inline int is_pal() const { return (max_fps_ == 25); }
	u_char* returnBuffer();
	void	returnGeometry(u_int &base, u_int &heigth);
	virtual int capture();
    protected:
	void set_size_nachos(int w, int h);
	virtual void setsize();// = 0;
	void setInputPort(int newport);
	void updateParameters(int both_fields = 0);
	void loadFirmware(int fid);
	void sendMessage();
	void findFirmware();
	virtual int firmwareFID() const;

#ifdef NEEDED_FOR_SUNVIDEO_STAR
  	u_int32_t media_ts();
  	u_int32_t ref_ts();
        void dispatch(int mask);
	virtual void start();
	virtual void stop();
#endif

        int status_;
	int running_;

	double nextframetime_;
	int fd_;		/* rtvc data fd */
	u_int decimate_;
	u_int basewidth_;
	u_int baseheight_;
	u_int max_fps_;		/* 25 (PAL) or 30 (NTSC) */
	int pagesize_;

	volatile rtvc_video_parameters_t* state_;
	rtvc_message_t fpb_;
	int cur_video_format_;
	int capsize_;
	u_char* capbuf_;	/* rtvc capture buffer */
	u_char* capbase_;	/* buffer base address (not page aligned) */
	const char* arpath_;
	int fid_;		/* current loaded capture firmware id */
	int ardir_[8];		/* offsets to firmware files in archive */

#define PAL_WIDTH 768
#define PAL_HEIGHT 576
#define PAL_FIRST_SCANLINE  23
#define NTSC_WIDTH 640
#define NTSC_HEIGHT 480
#define NTSC_FIRST_SCANLINE 23

#define CAPTURE_FID 0
#define SMALL_CAPTURE_FID 1
#define LARGE_CAPTURE_FID 2
#define JPEG_FID 3
#define JPEG_PAL_FID 4
};

#endif
