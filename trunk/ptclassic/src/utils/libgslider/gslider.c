/* 
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static	char	SccsId[] = "$Id$";
#endif /* not lint and not saber */

#include <stdio.h>
#include <malloc.h>
#include "compat.h"
#include "gslider.h"

extern	char	*malloc();
extern	char	*index();
extern	double	atof();

static	char	*strdup();
static	Gslider	*gsl_parse();
static  int	gsl_field();
/*
 * Read in the async i/o configuration file.  Format:
 *
 * label name slider hostmin hostmax hostinit dspmin dspmax dspinit rep descriptor
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
	char	*type;

	sp = (Gslider *)malloc(sizeof (*sp));
	if (sp == NULL) {
		perror("gsl_parse: malloc");
		return (NULL);
	}

	ret = gsl_field(line, &next, &type, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing type\n");
	}
	/* ignore ones that aren't sliders */
	if (strcmp(type, "slider") != 0) {
		(void) free(type);
		(void) free(sp);
		return (NULL);
	}
	ret = gsl_field(next, &next, &sp->dsplabel, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing DSP label\n");
	}
	ret = gsl_field(next, &next, &sp->name, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing name\n");
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
	ret = gsl_field(next, &next, &sp->descr, STRING);
	if (ret < 0) {
		ERROR("gsl_parse: missing description field\n");
	}
	sp->user = NULL;

	return (sp);
}

#define	SKIP_WS(start, cp) \
	for (cp = start; *cp == ' ' || *cp == '\t'; cp++) \
		;

#define	SKIP_NWS(line, cp) \
	for (cp = line; *cp && *cp != ' ' && *cp != '\t'; cp++) \
		;

#define	SKIP_TOQUOTE(line, cp) \
	for (cp = line; *cp && *cp != '"'; cp++) \
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
	if (*s == '"')  {
		s++;
		SKIP_TOQUOTE(s, e);
	} else {
		SKIP_NWS(s, e);
	}
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
