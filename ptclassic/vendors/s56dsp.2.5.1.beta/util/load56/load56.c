#ifndef lint
static	char	sccsid[] = "$Id$";
#endif

/* load56.c
	loads programs to the S-56 or S-56X.  Program may or may not be boot-
loadable.  If the program is boot-loadable, it will be loaded as is.  If
the program is not boot-loadable, then either the "-q" or "-d" switch to
indicate which monitor to load it through.  If loaded through the QckMon
monitor and a -x file is specified, qckXilinx is called to load the Xilinx.

    options:
	-q	forces program to be loaded through the qckMon monitor.
		If the program is boot-loadable, an error results since
		the program would overwrite the monitor's host command
		vectors.

	-d	forces the program to be loaded through the degmon monitor.
		If the program is boot-loadable, an error results since
		the program would overwrite the monitor's host command
		vectors.

	-e	forces the program to start execution after loading.
		Included for Gabriel.

	-x	loads a Xilinx .bit file, but only after QckMon is loaded.

*/

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/file.h>
#include "s56dspUser.h"
#include "qckMon.h"

main(argc, argv)
int argc;
char *argv[];
{
    extern int	optind;
    extern char	*optarg;
    char	*filename;
    int	c;
    int	monitor = 0;		/* monitor choice flag */
    int	program[512];		/* boot-loadable program code - internal
    					program memory is 512 words */
    int	proglen;		/* program length */
    int	bootable;		/* boot-loadable flag */
    char *start = NULL;
    int	fd;
    QckLod *lodprog;		/* program parser structure */
    HostIntr dsp;			/* ioctl() structure */
    char *xfile = NULL;
    QckMon	*dspmon;		/* qckMon structure */
    DspType	type;

    /*
     * check options
     */
    while ((c = getopt(argc, argv, "dqe:x:")) != -1) {
	switch (c) {
	case 'd':
	case 'q':
		monitor = c;
		break;
	case 'e':
		start = optarg;
		break;
	case 'x':
		xfile = optarg;
		break;
	}
    }

    if (monitor == -1 || optind >= argc) {
    	usage(argv[0]);
    	exit(1);
    }

    /*
     * parse the program file
     */
    filename = argv[optind];
    lodprog = qckLodNew(filename);	/* open file and parse it */
    if (lodprog == NULL) {
	fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
	exit(1);
    }

    /*
     * determine if file is boot-loadable
     */
    bootable = 1;	/* assume it *is* boot-loadable */
    if (qckLodBoot(lodprog, program, &proglen) == -1)
    	bootable = 0;	/* nope, it's not */

    /*
     * The "bootable" flag just tells whether or not the code is less than
     * than 512 words long and lives in P space.  It *could* overwrite the
     * monitor address, but then again, it might not.  We let the user get
     * by with a warning here.
     */
    if (bootable && monitor) {
    	fprintf(stderr, 
    	    "%s: warning: program is boot-loadable, might overwrite monitor\n",
		argv[0]);
    }

    if (!bootable && !monitor) {  /* oops, can't load a non-bootable program
    					without a monitor! */
    	fprintf(stderr, 
    	    "load56: can not load a non-bootable program without a monitor\n");
    	exit(1);
    }

    /*
     * load the monitor (if needed) and program
     */
    if (bootable && !monitor) {
    	printf("boot loading program %s\n", filename);
    	dsp.reset = 1;		/* reset puts board into boot-load mode */
    	dsp.interrupt = 0;
    	dsp.putWords = (unsigned char *) program;
    	dsp.putCount = proglen;
    	dsp.putMode = DspLow32;
    	dsp.getCount = 0;
    	dsp.getWords = 0;
    	dsp.getMode = DspLow32;
    	errno = 0;
    	fd = open("/dev/s56dsp", O_RDWR);
    	if (ioctl(fd, DspHostIntr, &dsp) == -1) {
    	    perror(
	"load56: ioctl(DspHostIntr) error loading boot-loadable program");
    	    exit(1);
    	}
    }
    else {
    	dspmon = qckAttach("/dev/s56dsp", NULL, 0);
	if (dspmon == NULL) {
		fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
		exit(1);
	}
    	if (monitor == 'd')
    	    c = qckBoot(dspmon, "degmon.lod");
    	else
    	    c = qckBoot(dspmon, "qckMon.lod");
	if (c == -1) {
		fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
		exit(1);
	}

	/* load Xilinx file */
	if (xfile) {
		if (monitor != 'q') {
			fprintf(stderr,
		"%s: can only load Xilinx files when running under QckMon\n",
				argv[0]);
			exit(1);
		}
		if (ioctl(dspmon->fd, DspGetType, &type) == -1) {
			fprintf(stderr, "%s: ", argv[0]);
			perror("ioctl DspGetType");
			exit(1);
		}
fprintf(stderr, "xilinx type %d\n", type.lca);
		c = qckXilinx(dspmon, xfile, type.lca);
		if (c == -1) {
			fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
			exit(1);
		}
	}

	/* load application */
    	if (qckLoad(dspmon, filename) == -1) {
		fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
		exit(1);
	}

	/* start */
	if (start) {
		if (qckJsr(dspmon, start) == -1)  {
			fprintf(stderr, "%s: %s\n", argv[0], qckErrString);
			exit(1);
		}
	}
    }
    exit(0);
}

usage(prog)
	char	*prog;
{
    fprintf(stderr, "Usage:  %s [-d | -q] [-e addr] [-x xfile] <loadfile>\n",
prog);
    fprintf(stderr, "        -d       for degmon monitor\n");
    fprintf(stderr, "        -q       for qckMon monitor\n");
    fprintf(stderr, "        -e addr  to start execution at \"addr\"\n");
    fprintf(stderr, "        -x xfile to load a Xilinx file\n");
}

