/* Version Identification:
 * $Id$
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
#ifndef TR_INCLUDED
#define TR_INCLUDED
#include "oct.h"

struct tr_link {
    struct octTransform transform;
    struct tr_link *next;
};

typedef struct tr_link *tr_stack;

#define tr_get_transform_type(stack)\
((stack == (tr_stack *) 0 || *stack == (struct tr_link *) 0) ?\
     TR_NO_TRANSFORM : (*stack)->transform.transformType)

#define TR_NO_TRANSFORM OCT_NO_TRANSFORM
#define TR_MIRROR_X OCT_MIRROR_X
#define TR_MIRROR_Y OCT_MIRROR_Y
#define TR_ROT90 OCT_ROT90
#define TR_ROT180 OCT_ROT180
#define TR_ROT270 OCT_ROT270
#define TR_MX_ROT90 OCT_MX_ROT90
#define TR_MY_ROT90 OCT_MY_ROT90
#define TR_FULL_TRANSFORM OCT_FULL_TRANSFORM

extern char *tr_op_name[];

#include "ansi.h"

EXTERN tr_stack *tr_create
	NULLARGS;

EXTERN void tr_free
	ARGS((tr_stack *stack));

EXTERN void tr_push
	ARGS((tr_stack *stack));

EXTERN void tr_pop
	ARGS((tr_stack *stack));

EXTERN void tr_identity
	ARGS((tr_stack *stack));

EXTERN void tr_do_op
	ARGS((tr_stack *stack, int type, int add_after));

EXTERN void tr_rotate_dir
	ARGS((tr_stack *stack, int32 x, int32 y, int add_after));

EXTERN void tr_rotate_angle
	ARGS((tr_stack *stack, double angle, int add_after));

EXTERN void tr_rotate_matrix
	ARGS((tr_stack *stack, double matrix[2][2], int add_after));

EXTERN void tr_scale
	ARGS((tr_stack *stack, double scale, int add_after));

EXTERN void tr_add_transform
	ARGS((tr_stack *stack, struct octTransform *transform, int add_after));

EXTERN void tr_translate
	ARGS((tr_stack *stack, int32 x, int32 y, int add_after));
	
EXTERN void tr_transform
	ARGS((tr_stack *stack, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_inverse_transform
	ARGS((tr_stack *stack, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_itransform
	ARGS((tr_stack *stack, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_scaler_transform
	ARGS((tr_stack *stack, int32 *scaler));

EXTERN void tr_angle_transform
	ARGS((tr_stack *stack, double *angle_ptr));

EXTERN int tr_is_manhattan
	ARGS((tr_stack *stack));

EXTERN int tr_is_mirrored
	ARGS((tr_stack *stack));

EXTERN void tr_get_transform
	ARGS((tr_stack *stack, double matrix[2][2], int32 *trans_x, int32 *trans_y, int *type_ptr));

EXTERN void tr_get_oct_transform
	ARGS((tr_stack *stack, struct octTransform *transform));

EXTERN void tr_get_angle
	ARGS((tr_stack *stack, double *angle_ptr, int *mirrored, double *scale));

EXTERN void tr_oct_transform
	ARGS((struct octTransform *transform, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_oct_inverse_transform
	ARGS((struct octTransform *transform, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_oct_itransform
	ARGS((struct octTransform *transform, int32 *x_ptr, int32 *y_ptr));

EXTERN void tr_oct_scaler_transform
	ARGS((struct octTransform *transform, int32 *scaler));

EXTERN void tr_oct_angle_transform
	ARGS((struct octTransform *transform, double *angle_ptr));

EXTERN int tr_oct_is_manhattan
	ARGS((struct octTransform *transform));

EXTERN int tr_oct_is_mirrored
	ARGS((struct octTransform *transform));

EXTERN void tr_oct_get_angle
	ARGS((struct octTransform *transform, double *angle_ptr, int *mirrored, double *scale));

EXTERN void tr_get_inverse_transform
	ARGS((struct octTransform *tr, struct octTransform *inv));

EXTERN void tr_delta_transform
	ARGS((struct octTransform *oldTr, struct octTransform *newTr, struct octTransform *tr));

#endif /* TR_INCLUDED */
