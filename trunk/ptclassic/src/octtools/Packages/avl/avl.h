/* Version Identification:
 * @(#)avl.h	1.1	12/15/93
 */
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
#ifndef AVL_H
#define AVL_H

typedef struct avl_node_struct avl_node;
struct avl_node_struct {
    avl_node *left, *right;
    char *key;
    char *value;
    int height;
};


typedef struct avl_tree_struct avl_tree;
struct avl_tree_struct {
    avl_node *root;
    int (*compar)();
    int num_entries;
    int modified;
};


typedef struct avl_generator_struct avl_generator;
struct avl_generator_struct {
    avl_tree *tree;
    avl_node **nodelist;
    int count;
};


#define AVL_FORWARD 	0
#define AVL_BACKWARD 	1

#include "ansi.h"

extern avl_tree *avl_init_table();
	/* ARGS((int (*compar)())); */

extern int avl_lookup
	ARGS((avl_tree *tree, char *key, char **value_p));

extern int avl_first
	ARGS((avl_tree *tree, char **key_p, char **value_p));

extern int avl_last
	ARGS((avl_tree *tree, char **key_p, char **value_p));

extern int avl_insert
	ARGS((avl_tree *tree, char *key, char *value));

extern int avl_find_or_add
	ARGS((avl_tree *tree, char *key, char ***slot_p));

extern int avl_delete
	ARGS((avl_tree *tree, char **key_p, char **value_p));

extern avl_generator *avl_init_gen
	ARGS((avl_tree *tree, int dir));

extern int avl_gen
	ARGS((avl_generator *gen, char **key_p, char **value_p));

extern void avl_free_gen
	ARGS((avl_generator *gen));

extern void avl_foreach();
	/* ARGS((avl_tree *tree, void (*func)(), int direction)); */

extern void avl_free_table();
	/* ARGS((avl_tree *tree, void (*key_free)(), void (*value_free)())); */

extern int avl_count
	ARGS((avl_tree *tree));

extern int avl_numcmp();

extern int avl_check_tree
	ARGS((avl_tree *tree));

#define avl_is_member(tree, key)	avl_lookup(tree, key, (char **) 0)

#define avl_foreach_item(table, gen, dir, key_p, value_p) 	\
    for(gen = avl_init_gen(table, dir); 			\
	    avl_gen(gen, key_p, value_p) || (avl_free_gen(gen),0);)

#endif /* AVL_H */
