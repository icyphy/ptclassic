#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
 * 
 */
#include "copyright.h"
#include "port.h"
#include "avl.h"

char *words[] = {
    "ricks", "davidh", "moore", "rudell", "ellen", "wbaker", "pds", "wiggin", "tom",
    "quarles", "brianl", "fabio", "casotto", "jlb", "sakurai", "singh", "kring",
    "clm", "beorn", "kjpires", "hendry", "hastings", "kiernan"
};

int f, b;

/*ARGSUSED*/
static int
incf(key, value)
char *key, *value;
{
    f++;
    return 0;
}


/*ARGSUSED*/
static int
incb(key, value)
char *key, *value;
{
    b++;
    return 0;
}


int
main()
{
    avl_tree *tree;
    int i;
    char *key, *value;
    
    tree = avl_init_table(strcmp);

    for (i = 0; i < sizeof(words)/sizeof(*words); i++) {
	if (avl_insert(tree, words[i], (char *) i)) {
	    exit(1);
	}
    }

    if (!avl_lookup(tree, "ellen", (char **) &i) || i != 4) {
	exit(-1);
    }

    f = 0;
    avl_foreach(tree, incf, AVL_FORWARD);

    b = 0;
    avl_foreach(tree, incb, AVL_BACKWARD);

    if (f != b) {
	exit(-2);
    }

    key = "kring";
    
    if (!avl_delete(tree, &key, (char **) &i)) {
	exit(-3);
    }

    f = 0;
    avl_foreach(tree, incf, AVL_FORWARD);

    if (f != b - 1) {
	exit(-4);
    }

    if (avl_count(tree) != sizeof(words)/sizeof(*words) - 1) {
	exit(-5);
    }

    if (!avl_first(tree, &key, &value) || (strcmp(key, "beorn") != 0)) {
	exit(-6);
    }

    if (!avl_last(tree, &key, &value) || (strcmp(key, "wiggin") != 0)) {
	exit(-6);
    }

    return 0;
}
