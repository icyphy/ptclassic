#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "pq.h"

#include "ansi.h"

int getline
	ARGS((FILE *f, char *s));
int
main()
{
    pq_t *queue, *queue1;
    pq_gen_t gen;
    int i;
    char buf[1024], *s, *key;
    FILE *infile;

    infile = fopen("input", "r");

    queue = pq_init_queue(pq_numcmp);

    while((fscanf(infile, "%d", &i) != EOF) && getline(infile, buf)) {
	key = (char *) i;
	pq_insert(queue, key, util_strsav(buf));
    }

    queue1 = pq_copy(queue);

    printf("Dumping\n");

    pq_foreach(queue, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }

    printf("Popping\n");
    

    while(pq_maximal_key(queue, &key) && pq_pop(queue, &s)) {
	i = (int) key;
	printf("%d %s\n",i,  s);
    }

    printf("Redumping\n");
    
    
    pq_foreach(queue1, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }

    printf(" Testing robust foreach\n");
    
    pq_foreach1(queue1, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }

    printf(" Testing pq_break\n");
    
    pq_foreach1(queue1, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
	pq_break(gen);
    }

    printf(" Testing pq_break on a normal generator\n");

    
    pq_foreach(queue1, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
	pq_break(gen);
    }

    printf("Reusing queue\n");

    pq_foreach(queue1, gen, key, s)
	pq_insert(queue, key, s);

    pq_foreach(queue, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }

    printf(" Testing pq_free\n");

    pq_free_queue(queue1);
    
    pq_foreach(queue, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }
    
    queue1 = pq_init_queue(pq_numcmp);

    printf(" Testing delete and pq_insert_fast\n");

    pq_foreach1(queue, gen, key, s) {
	pq_delete(queue, key, s);
	pq_insert_fast(queue1, key, s);
    }

    /* Dumping Empty queue */

    printf("Dumping Empty queue\n");
    
    pq_foreach(queue, gen, key, s) {
	i = (int) key;
	printf("%d %s\n", i, s);
    }

    pq_free_queue(queue);

    printf(" Dumping fast-built queue\n");
    

    while(pq_maximal_key(queue1, &key) && pq_pop(queue1, &s)) {
	i = (int) key;
	printf("%d %s\n",i,  s);
    }

    pq_free_queue(queue1);

    return 0;
}

int
getline(f, s)
FILE *f;
char *s;
{
    while((*s = getc(f)) != '\n') ++s;
    *s = '\0';
    
    return 1;
}

