#if !defined(lint) && !defined(SABER)
static	char	sccsid[] = "$Id$";
static	char	copyright[] =
"Copyright 1990 Regents of the University of California.  All rights reserved.";
#endif /* not defined lint and not defined SABER */

#include <stdio.h>
#include "gslider.h"

extern	char	*malloc();
extern	char	*index();
extern	double	atof();

static	char	*strdup();
static	Gslider	*gsl_parse();

/*
 * Read in the a slider configuration file.  Format:
 *
 * label hostmin hostmax hostinit dspmin dspmax dspinit rep descriptor
 */

Gslider *
gsl_read(file)
	char	*file;
{
	FILE	*fp;
	char	*cp;
	char	buf[256];
	Gslider	*sp;
	Gslider	*head = NULL;

	fp = fopen(file, "r");
	if (fp == NULL) {
		fprintf(stderr, "gsl_read: fopen ");
		perror(file);
		return (NULL);
	}

	while (fgets(buf, sizeof (buf), fp) != NULL) {
		cp = index(buf, '\n');
		if (cp)
			*cp = '\0';
		if (buf[0] == '\0')
			continue;
		sp = gsl_parse(buf);
		if (sp == NULL)
			continue;
		if (head)
			sp->next = head;
		head = sp;
	}

	return (head);
}


#define ERROR(error) \
	fprintf(stderr, "%s\n", error); \
	free(sp); \
	return (NULL);

#define	STRING	0
#define	DOUBLE	1

static Gslider *
gsl_parse(line)
	char	*line;
{
	Gslider	*sp;
	int	ret;
	char	*next;
	char	*rep;

	sp = (Gslider *)malloc(sizeof (*sp));
	if (sp == NULL) {
		perror("gsl_parse: malloc");
		return (NULL);
	}

	ret = gsl_field(line, &next, &sp->dsplabel, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing DSP label\n");
	}
	ret = gsl_field(next, &next, &sp->hostmin, DOUBLE);
	if (ret < 0) {
		ERROR("gsl_parse: bad host minimum value\n");
	}
	ret = gsl_field(next, &next, &sp->hostmax, DOUBLE);
	if (ret < 0) {
		ERROR("gsl_parse: bad host maximum value\n");
	}
	ret = gsl_field(next, &next, &sp->hostinit, DOUBLE);
	if (ret < 0) {
		ERROR("gsl_parse: bad host initial value\n");
	}
	ret = gsl_field(next, &next, &sp->dspmin, DOUBLE);
	if (ret < 0) {
		ERROR("gsl_parse: bad dsp minimum value\n");
	}
	ret = gsl_field(next, &next, &sp->dspmax, DOUBLE);
	if (ret < 0) {
		ERROR("gsl_parse: bad dsp maximum value\n");
	}
	ret = gsl_field(next, &next, &rep, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing representation field\n");
	}
	if (strcmp(rep, "linear") == 0)
		sp->rep = GSL_REP_LINEAR;
	else if (strcmp(rep, "db") == 0)
		sp->rep = GSL_REP_DB;
	else {
		ERROR("gsl_parse: unknown representation field\n");
	}
	free(rep);

	sp->descr = strdup(next);
	sp->user = NULL;

	return (sp);
}

#define	SKIP_WS(start, cp) \
	for (cp = start; *cp == ' ' || *cp == '\t'; cp++) \
		;

#define	SKIP_NWS(line, cp) \
	for (cp = start; *cp && *cp != ' ' && *cp != '\t'; cp++) \
		;

/*
 * Parse a field into buf pointed to by value pointer.
 * Return 1 on success, 0 on success but end of string, -1
 * on premature end of string.
 */
static int
gsl_field(start, next, vp, type)
	char	*start;
	char	**next;
	char	*vp;
	int	type;
{
	char	*s, *e;
	int	eol = 0;

	SKIP_WS(start, s);
	if (*s == '\0')
		return (-1);
	SKIP_NWS(s, e);
	if (*e == '\0')
		eol++;
	*e++ = '\0';
	if (type == STRING) {
		*(char **)vp = strdup(s);
	} else {
		*(double *)vp = atof(s);
	}
	*next = e;
	return ((eol == 0) ? 1 : 0);
}

static char *
strdup(s)
	char	*s;
{
	char	*cp;

	cp = malloc(strlen(s) + 1);
	if (cp == NULL)
		return (NULL);
	(void) strcpy(cp, s);
	return cp;
}
