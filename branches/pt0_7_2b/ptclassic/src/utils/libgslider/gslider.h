/*
 * Gabriel slider package.
 *
 * $Id$
 */

/*
 * gsl_read makes a linked list of the following structure, one
 * entry per line of the "0code.sld" file generated by Gabriel.
 * The "user" field is initially NULL, and can be set to point
 * to whatever data the user wants (e.g., X widgets).
 */
struct gslider {
	char	*dsplabel;	/* DSP memory location label */
	char	*descr;		/* description */
	double	hostmin;	/* host minimum value */
	double	hostmax;	/* host maximum value */
	double	hostinit;	/* host initial value */
	double	dspmin;		/* dsp minimum value */
	double	dspmax;		/* dsp maximum value */
	double	dspinit;	/* dsp initial value */
	int	rep;		/* representation (db or linear) */
	char	*user;		/* user data */
	struct gslider *next;
};

#define	GSL_REP_LINEAR	0
#define	GSL_REP_DB	1

typedef struct gslider	Gslider;

extern	Gslider	*gsl_read();
