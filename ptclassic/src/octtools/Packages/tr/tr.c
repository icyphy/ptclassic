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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "oct.h"
#include "tr.h"

#define LEFT_MULTIPLY(result,matrix,op)\
do_tmp = matrix[0][0];\
result[0][0] = do_tmp*op[0][0] + matrix[1][0]*op[0][1];\
result[1][0] = do_tmp*op[1][0] + matrix[1][0]*op[1][1];\
do_tmp = matrix[0][1];\
result[0][1] = do_tmp*op[0][0] + matrix[1][1]*op[0][1];\
result[1][1] = do_tmp*op[1][0] + matrix[1][1]*op[1][1];


#define RIGHT_MULTIPLY(result,matrix,op)\
do_tmp = matrix[0][0];\
result[0][0] = do_tmp*op[0][0] + matrix[0][1]*op[1][0];\
result[0][1] = do_tmp*op[0][1] + matrix[0][1]*op[1][1];\
do_tmp = matrix[1][0];\
result[1][0] = do_tmp*op[0][0] + matrix[1][1]*op[1][0];\
result[1][1] = do_tmp*op[0][1] + matrix[1][1]*op[1][1];

#define ROTATE_VECTOR(matrix, x, y)\
do_tmp = x;\
x = matrix[0][0]*do_tmp + matrix[0][1]*y;\
y = matrix[1][0]*do_tmp + matrix[1][1]*y;

static int simple_matrices[8][2][2] = {
    {{ 1, 0},{ 0, 1}},
    {{-1, 0},{ 0, 1}},
    {{ 1, 0},{ 0,-1}},
    {{ 0,-1},{ 1, 0}},
    {{-1, 0},{ 0,-1}},
    {{ 0, 1},{-1, 0}},
    {{ 0,-1},{-1, 0}},
    {{ 0, 1},{ 1, 0}}
};

static octTransformType op_table[2][8][8] = {
    {
	{OCT_NO_TRANSFORM,OCT_MIRROR_X,OCT_MIRROR_Y,OCT_ROT90,OCT_ROT180,OCT_ROT270,OCT_MX_ROT90,OCT_MY_ROT90,},
	{OCT_MIRROR_X,OCT_NO_TRANSFORM,OCT_ROT180,OCT_MY_ROT90,OCT_MIRROR_Y,OCT_MX_ROT90,OCT_ROT270,OCT_ROT90,},
	{OCT_MIRROR_Y,OCT_ROT180,OCT_NO_TRANSFORM,OCT_MX_ROT90,OCT_MIRROR_X,OCT_MY_ROT90,OCT_ROT90,OCT_ROT270,},
	{OCT_ROT90,OCT_MX_ROT90,OCT_MY_ROT90,OCT_ROT180,OCT_ROT270,OCT_NO_TRANSFORM,OCT_MIRROR_Y,OCT_MIRROR_X,},
	{OCT_ROT180,OCT_MIRROR_Y,OCT_MIRROR_X,OCT_ROT270,OCT_NO_TRANSFORM,OCT_ROT90,OCT_MY_ROT90,OCT_MX_ROT90,},
	{OCT_ROT270,OCT_MY_ROT90,OCT_MX_ROT90,OCT_NO_TRANSFORM,OCT_ROT90,OCT_ROT180,OCT_MIRROR_X,OCT_MIRROR_Y,},
	{OCT_MX_ROT90,OCT_ROT90,OCT_ROT270,OCT_MIRROR_X,OCT_MY_ROT90,OCT_MIRROR_Y,OCT_NO_TRANSFORM,OCT_ROT180,},
	{OCT_MY_ROT90,OCT_ROT270,OCT_ROT90,OCT_MIRROR_Y,OCT_MX_ROT90,OCT_MIRROR_X,OCT_ROT180,OCT_NO_TRANSFORM,},
    },
    {
	{OCT_NO_TRANSFORM,OCT_MIRROR_X,OCT_MIRROR_Y,OCT_ROT90,OCT_ROT180,OCT_ROT270,OCT_MX_ROT90,OCT_MY_ROT90,},
	{OCT_MIRROR_X,OCT_NO_TRANSFORM,OCT_ROT180,OCT_MX_ROT90,OCT_MIRROR_Y,OCT_MY_ROT90,OCT_ROT90,OCT_ROT270,},
	{OCT_MIRROR_Y,OCT_ROT180,OCT_NO_TRANSFORM,OCT_MY_ROT90,OCT_MIRROR_X,OCT_MX_ROT90,OCT_ROT270,OCT_ROT90,},
	{OCT_ROT90,OCT_MY_ROT90,OCT_MX_ROT90,OCT_ROT180,OCT_ROT270,OCT_NO_TRANSFORM,OCT_MIRROR_X,OCT_MIRROR_Y,},
	{OCT_ROT180,OCT_MIRROR_Y,OCT_MIRROR_X,OCT_ROT270,OCT_NO_TRANSFORM,OCT_ROT90,OCT_MY_ROT90,OCT_MX_ROT90,},
	{OCT_ROT270,OCT_MX_ROT90,OCT_MY_ROT90,OCT_NO_TRANSFORM,OCT_ROT90,OCT_ROT180,OCT_MIRROR_Y,OCT_MIRROR_X,},
	{OCT_MX_ROT90,OCT_ROT270,OCT_ROT90,OCT_MIRROR_Y,OCT_MY_ROT90,OCT_MIRROR_X,OCT_NO_TRANSFORM,OCT_ROT180,},
	{OCT_MY_ROT90,OCT_ROT90,OCT_ROT270,OCT_MIRROR_X,OCT_MX_ROT90,OCT_MIRROR_Y,OCT_ROT180,OCT_NO_TRANSFORM,},
    }
};

char *tr_op_name[9] = {
    "OCT_NO_TRANSFORM",
    "OCT_MIRROR_X",
    "OCT_MIRROR_Y",
    "OCT_ROT90",
    "OCT_ROT180",
    "OCT_ROT270",
    "OCT_MX_ROT90",
    "OCT_MY_ROT90",
    "OCT_FULL_TRANSFORM"
};


typedef double (*double_matrix_ptr)[2];
typedef int (*int_matrix_ptr)[2];
static double do_tmp;


tr_stack *
tr_create()
{
    tr_stack *stack;

    stack = ALLOC(tr_stack, 1);
    *stack = ALLOC(struct tr_link, 1);

    (*stack)->next = NIL(struct tr_link);
    tr_identity(stack);
    return stack;
}

void
tr_free(stack)
tr_stack *stack;
{
    struct tr_link *ptr,*tmp;

    if (stack == NIL(tr_stack)) {
	return;
    }
    
    ptr = *stack;
    while (ptr != NIL(struct tr_link)) {
	tmp = ptr;
	ptr = ptr->next;
	FREE(tmp);
    }
    
    *stack = NIL(struct tr_link);
    FREE(stack);
    return;
}

void
tr_push(stack)
tr_stack *stack;
{
    struct tr_link *ptr;
    
    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    ptr = ALLOC(struct tr_link, 1);

    *ptr = **stack;
    ptr->next = *stack;
    *stack = ptr;
    return;
}

void
tr_pop(stack)
tr_stack *stack;
{
    struct tr_link *tfm;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }
    
    tfm = *stack;

    if (tfm->next == NIL(struct tr_link)) {
	return;
    }

    tfm = tfm->next;
    FREE(*stack);
    *stack = tfm;
    return;
}

void
tr_identity(stack)
tr_stack *stack;
{
    struct tr_link *tfm;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    tfm->transform.transformType = OCT_NO_TRANSFORM;
    tfm->transform.translation.x = tfm->transform.translation.y = 0;
    return;
}

void
tr_do_op(stack, type, add_after)
tr_stack *stack;
int type;
int add_after;
{
    struct tr_link *tfm;
    int_matrix_ptr imatrix = simple_matrices[(int)type];

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;

    if (tfm->transform.transformType == OCT_FULL_TRANSFORM) {
	double_matrix_ptr dmatrix = tfm->transform.generalTransform;

	if (add_after) {
	    LEFT_MULTIPLY(dmatrix,dmatrix,imatrix);
	} else {
	    RIGHT_MULTIPLY(dmatrix,dmatrix,imatrix);
	}
    } else {
	tfm->transform.transformType =
	  op_table[add_after != 0][(int)tfm->transform.transformType]
	    [(int)type];
    }
    
    if (add_after) {
	ROTATE_VECTOR(imatrix, tfm->transform.translation.x,
		      tfm->transform.translation.y);
    }

    return;
}

#define DO_ROTATION(result, matrix, cos_th, sin_th, l_m)\
if (l_m) {\
    do_tmp = matrix[0][0];\
    result[0][0] = do_tmp*cos_th + -matrix[1][0]*sin_th;\
    result[1][0] = do_tmp*sin_th +  matrix[1][0]*cos_th;\
    do_tmp = matrix[0][1];\
    result[0][1] = do_tmp*cos_th + -matrix[1][1]*sin_th;\
    result[1][1] = do_tmp*sin_th +  matrix[1][1]*cos_th;\
} else {\
    do_tmp = matrix[0][0];\
    result[0][0] =  do_tmp*cos_th +  matrix[0][1]*sin_th;\
    result[0][1] = -do_tmp*sin_th +  matrix[0][1]*cos_th;\
    do_tmp = matrix[1][0];\
    result[1][0] =  do_tmp*cos_th +  matrix[1][1]*sin_th;\
    result[1][1] = -do_tmp*sin_th +  matrix[1][1]*cos_th;\
}

void
tr_rotate_dir(stack, x, y, add_after)
tr_stack *stack;
int32 x,y;
int add_after;
{
    struct tr_link *tfm;
    octTransformType op = OCT_FULL_TRANSFORM;
    double length;
    double sin_th, cos_th;
    double_matrix_ptr dmatrix;
    int_matrix_ptr imatrix;
    int32 temp;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    if (x == 0) {
	if (y < 0) {
	    op = OCT_ROT270;
	} else if (y > 0) {
	    op = OCT_ROT90;
	} else {
	    return;
	}
    } else if (y == 0) {
	if (x < 0) {
	    op = OCT_ROT180;
	} else {
	    return;
	}
    }

    if (op != OCT_FULL_TRANSFORM) {
	tr_do_op(stack, (int) op, add_after);
	return;
    }
    
    length = hypot((double)x,(double)y);
    sin_th = y/length;
    cos_th = x/length;
    dmatrix = tfm->transform.generalTransform;

    if (tfm->transform.transformType == OCT_FULL_TRANSFORM) {
	DO_ROTATION(dmatrix, dmatrix, cos_th, sin_th, add_after);
    } else {
	imatrix = simple_matrices[(int)tfm->transform.transformType];
	DO_ROTATION(dmatrix, imatrix, cos_th, sin_th, add_after);
    }

    if (add_after) {
	temp = tfm->transform.translation.x;
	tfm->transform.translation.x = cos_th*temp -
	  sin_th*tfm->transform.translation.y;
	tfm->transform.translation.y = sin_th*temp +
	  cos_th*tfm->transform.translation.y;
    }
	  
    tfm->transform.transformType = OCT_FULL_TRANSFORM;
    return;
}

static double radians_per_degree = .01745329251994329576;

void
tr_rotate_angle(stack, angle, add_after)
tr_stack *stack;
double angle;
int add_after;
{
    double sin_th, cos_th;
    struct tr_link *tfm;
    double_matrix_ptr dmatrix;
    int_matrix_ptr imatrix;
    int32 temp;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    dmatrix = tfm->transform.generalTransform;
    angle *= radians_per_degree;
    cos_th = cos(angle);
    sin_th = sin(angle);
    
    if (tfm->transform.transformType == OCT_FULL_TRANSFORM) {
	DO_ROTATION(dmatrix, dmatrix, cos_th, sin_th, add_after);
    } else {
	imatrix = simple_matrices[(int)tfm->transform.transformType];
	DO_ROTATION(dmatrix, imatrix, cos_th, sin_th, add_after);
    }

    if (add_after) {
	temp = tfm->transform.translation.x;
	tfm->transform.translation.x = cos_th*temp -
	  sin_th*tfm->transform.translation.y;
	tfm->transform.translation.y = sin_th*temp +
	  cos_th*tfm->transform.translation.y;
    }
	  
    tfm->transform.transformType = OCT_FULL_TRANSFORM;
    return;
}

void
tr_scale(stack, scale, add_after)
tr_stack *stack;
double scale;
int add_after;
{
    double mat[2][2];
    
    mat[0][0] = mat[1][1] = scale;
    mat[0][1] = mat[1][0] = 0;
    tr_rotate_matrix(stack, mat, add_after);
    return;
}

void 
tr_rotate_matrix(stack, matrix, add_after)
tr_stack *stack;
double matrix[2][2];
int add_after;
{
    double_matrix_ptr dmatrix;
    int_matrix_ptr imatrix;
    struct tr_link *tfm;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    dmatrix = tfm->transform.generalTransform;
    
    if (tfm->transform.transformType == OCT_FULL_TRANSFORM) {
	if (add_after) {
	    LEFT_MULTIPLY(dmatrix, dmatrix, matrix);
	} else {
	    RIGHT_MULTIPLY(dmatrix, dmatrix, matrix);
	}
    } else {
	imatrix = simple_matrices[(int)tfm->transform.transformType];
	if (add_after) {
	    LEFT_MULTIPLY(dmatrix, imatrix, matrix);
	} else {
	    RIGHT_MULTIPLY(dmatrix, imatrix, matrix);
	}
	tfm->transform.transformType = OCT_FULL_TRANSFORM;
    }

    if (add_after) {
	ROTATE_VECTOR(matrix, tfm->transform.translation.x,
		      tfm->transform.translation.y);
    }
    
    return;
}

void
tr_add_transform(stack, transform, add_after)
tr_stack *stack;
struct octTransform *transform;
int add_after;
{
    
    if (!add_after) {
	tr_translate(stack, transform->translation.x,
			    transform->translation.y, add_after);
    }
    if (transform->transformType == OCT_FULL_TRANSFORM) {
	tr_rotate_matrix(stack, transform->generalTransform, add_after);
    } else {
	tr_do_op(stack, (int) transform->transformType, add_after);
    }
    if (add_after) {
	tr_translate(stack, transform->translation.x,
			    transform->translation.y, add_after);
    }
    return;
}

void
tr_translate(stack, x, y, add_after)
tr_stack *stack;
int32 x, y;
int add_after;
{
    struct tr_link *tfm;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    if (add_after) {
	tfm->transform.translation.x += x;
	tfm->transform.translation.y += y;
    } else {
	tr_transform(stack, &x, &y);
	tfm->transform.translation.x = x;
	tfm->transform.translation.y = y;
    }
    
    return;
}

void
tr_oct_scaler_transform(transform, scaler)
struct octTransform *transform;
int32 *scaler;
{
    if (transform->transformType == TR_FULL_TRANSFORM) {
	double scale = hypot(transform->generalTransform[0][0],
			     transform->generalTransform[0][1]);
	*scaler *= scale;
    }
    return;
}

void
tr_scaler_transform(stack, scaler)
tr_stack *stack;
int32 *scaler;
{
    register struct octTransform *transform;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    transform = &(*stack)->transform;
    if (transform->transformType == TR_FULL_TRANSFORM) {
	double scale = hypot(transform->generalTransform[0][0],
			     transform->generalTransform[0][1]);
	*scaler *= scale;
    }
    return;
}

#define DO_OCT_ITRANSFORM(transform, x, y)\
{\
    register int32 _tmp;\
\
    switch (transform->transformType) {\
    case TR_NO_TRANSFORM:\
	 break;\
    case TR_MIRROR_X:\
	x = -x; y = y; break;\
    case TR_MIRROR_Y:\
	x = x; y = -y; break;\
    case TR_ROT90:\
	_tmp = x; x = -y; y = _tmp; break;\
    case TR_ROT180:\
	x = -x; y = -y; break;\
    case TR_ROT270:\
	_tmp = x; x = y; y = -_tmp; break;\
    case TR_MX_ROT90:\
	_tmp = x; x = -y; y = -_tmp; break;\
    case TR_MY_ROT90:\
	_tmp = x; x = y; y = _tmp; break;\
    case TR_FULL_TRANSFORM:\
	{\
	    double_matrix_ptr dmatrix = transform->generalTransform;\
	    \
	    _tmp = x;\
	    x = dmatrix[0][0]*_tmp + dmatrix[0][1]*y;\
	    y = dmatrix[1][0]*_tmp + dmatrix[1][1]*y;\
	}\
    }\
}

#define DO_OCT_TRANSFORM(transform, x, y)\
    DO_OCT_ITRANSFORM(transform, x, y);\
    x += transform->translation.x;\
    y += transform->translation.y;


void
tr_oct_itransform(transform, x_ptr, y_ptr)
struct octTransform *transform;
int32 *x_ptr, *y_ptr;
{
    register int32 x = *x_ptr, y = *y_ptr;
    DO_OCT_ITRANSFORM(transform, x, y);
    *x_ptr = x;
    *y_ptr = y;
    return;
}

void
tr_itransform(stack, x_ptr, y_ptr)
tr_stack *stack;
int32 *x_ptr, *y_ptr;
{
    register struct octTransform *transform;
    register int32 x = *x_ptr, y = *y_ptr;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    transform = &(*stack)->transform;
    DO_OCT_ITRANSFORM(transform, x, y);
    *x_ptr = x;
    *y_ptr = y;
    return;
}

void
tr_oct_transform(transform, x_ptr, y_ptr)
struct octTransform *transform;
int32 *x_ptr, *y_ptr;
{
    register int32 x = *x_ptr, y = *y_ptr;
    DO_OCT_TRANSFORM(transform, x, y);
    *x_ptr = x;
    *y_ptr = y;
    return;
}

void
tr_transform(stack, x_ptr, y_ptr)
tr_stack *stack;
int32 *x_ptr, *y_ptr;
{
    register struct octTransform *transform;
    register int32 x = *x_ptr, y = *y_ptr;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    transform = &(*stack)->transform;
    DO_OCT_TRANSFORM(transform, x, y);
    *x_ptr = x;
    *y_ptr = y;
    return;
}

void
tr_inverse_transform(stack, x_ptr, y_ptr)
tr_stack *stack;
int32 *x_ptr, *y_ptr;
{
    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tr_oct_inverse_transform(&(*stack)->transform, x_ptr, y_ptr);
    return;
}


#define DO_OCT_IS_MANHATTAN(transform)\
    if (transform->transformType != TR_FULL_TRANSFORM) {\
	return 1;\
    } else {\
	double_matrix_ptr dmatrix = transform->generalTransform;\
	if (dmatrix[0][1] == 0 && dmatrix[1][0] == 0 &&\
	    ((dmatrix[0][0] ==  dmatrix[1][1]) ||\
	     (dmatrix[0][0] == -dmatrix[1][1]))) {\
	    return 1;\
	}\
	if (dmatrix[0][0] == 0 && dmatrix[1][1] == 0 &&\
	    ((dmatrix[1][0] ==  dmatrix[0][1]) ||\
	     (dmatrix[1][0] == -dmatrix[0][1]))) {\
	    return 1;\
	}\
	return 0;\
    }


int
tr_oct_is_manhattan(transform)
struct octTransform *transform;
{
    DO_OCT_IS_MANHATTAN(transform);
} 

int
tr_is_manhattan(stack)
tr_stack *stack;
{
    register struct octTransform *transform;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return 0;
    }

    transform = &(*stack)->transform;
    DO_OCT_IS_MANHATTAN(transform);
}

#define DO_OCT_IS_MIRRORED(transform)\
    switch (transform->transformType) {\
    case TR_NO_TRANSFORM:\
    case TR_ROT90:\
    case TR_ROT180:\
    case TR_ROT270:\
	return 0;\
    case TR_MX_ROT90:\
    case TR_MY_ROT90:\
    case TR_MIRROR_X:\
    case TR_MIRROR_Y:\
	return 1;\
    case TR_FULL_TRANSFORM:\
	{\
	    double_matrix_ptr dm = transform->generalTransform;\
	    if (dm[0][0]*dm[1][1] - dm[0][1]*dm[1][0] < 0 ) {\
		return 1;\
	    } else {\
		return 0;\
	    }\
	}\
    default :\
	return 0;\
    }

int
tr_oct_is_mirrored(transform)
struct octTransform *transform;
{
    DO_OCT_IS_MIRRORED(transform);
}

int
tr_is_mirrored(stack)
tr_stack *stack;
{
    register struct octTransform *transform;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return 0;
    }

    transform = &(*stack)->transform;
    DO_OCT_IS_MIRRORED(transform);
}    


#define DO_OCT_GET_ANGLE(transform, theta_ptr, mirrored, scale)\
    *scale = 1;\
    switch (transform->transformType) {\
    case TR_NO_TRANSFORM:\
        *mirrored = 0;\
	*theta_ptr = 0;\
	break;\
    case TR_MIRROR_X:\
	*mirrored = 1;\
        *theta_ptr = 180;\
	break;\
    case TR_MIRROR_Y:\
	*mirrored = 1;\
	*theta_ptr = 0;\
	break;\
    case TR_ROT90:\
        *mirrored = 0;\
        *theta_ptr = 90;\
	break;\
    case TR_ROT180:\
        *mirrored = 0;\
        *theta_ptr = 180;\
	break;\
    case TR_ROT270:\
        *mirrored = 0;\
        *theta_ptr = 270;\
	break;\
    case TR_MX_ROT90:\
	*mirrored = 1;\
	*theta_ptr = 270;\
	break;\
    case TR_MY_ROT90:\
	*mirrored = 1;\
	*theta_ptr = 90;\
	break;\
    case TR_FULL_TRANSFORM:\
	{\
	    double diag = transform->generalTransform[0][0];\
	    double off_diag = transform->generalTransform[1][0];\
	    *mirrored = tr_oct_is_mirrored(transform);\
	    *theta_ptr = atan2(off_diag, diag)/radians_per_degree;\
	    if (*theta_ptr < 0) *theta_ptr += 360;\
	    *scale = hypot(diag, off_diag);\
	    break;\
	}\
    }


void
tr_oct_get_angle(transform, angle_ptr, mirrored, scale)
struct octTransform *transform;
double *angle_ptr;
int *mirrored;
double *scale;
{
    DO_OCT_GET_ANGLE(transform, angle_ptr, mirrored, scale);
    return;
}

void
tr_get_angle(stack, angle_ptr, mirrored, scale)
tr_stack *stack;
double *angle_ptr;
int *mirrored;
double *scale;
{
    register struct octTransform *transform;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    transform = &(*stack)->transform;
    DO_OCT_GET_ANGLE(transform, angle_ptr, mirrored, scale);
    
    return;
}

#define DO_OCT_ANGLE_TRANSFORM(transform, angle_ptr)\
    {\
	int _is_mirrored;\
	double _rot_angle, _scale;\
	DO_OCT_GET_ANGLE(transform, &_rot_angle, &_is_mirrored, &_scale);\
	if (_is_mirrored) {\
	    *angle_ptr = -*angle_ptr;\
	}\
	*angle_ptr += _rot_angle;\
    }


void
tr_oct_angle_transform(transform, angle_ptr)
struct octTransform *transform;
double *angle_ptr;
{
    DO_OCT_ANGLE_TRANSFORM(transform, angle_ptr);
    return;
}

void
tr_angle_transform(stack, angle_ptr)
tr_stack *stack;
double *angle_ptr;
{
    register struct octTransform *transform;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    transform = &(*stack)->transform;
    DO_OCT_ANGLE_TRANSFORM(transform, angle_ptr);
    return;
}

/* THIS ONLY WORKS FOR ORTHOGONAL MATRICES */

#define DO_INVERSE_TRANSFORM(xp, yp, matrix, x, y)\
    *xp = matrix[0][0]*x + matrix[1][0]*y;\
    *yp = matrix[0][1]*x + matrix[1][1]*y;

void
tr_oct_inverse_transform(transform, x_ptr, y_ptr)
struct octTransform *transform;
int32 *x_ptr, *y_ptr;
{
    int32 x = *x_ptr, y = *y_ptr;

    x -= transform->translation.x;
    y -= transform->translation.y;
    
    if (transform->transformType == OCT_FULL_TRANSFORM) {
	double_matrix_ptr dmatrix = transform->generalTransform;
	double norm = dmatrix[0][0]*dmatrix[0][0] + dmatrix[0][1]*dmatrix[0][1];
	if (norm == 0) return;
	DO_INVERSE_TRANSFORM(x_ptr, y_ptr, dmatrix, x, y);
	*x_ptr /= norm; *y_ptr /= norm;
    } else {
	int_matrix_ptr imatrix = simple_matrices[(int)transform->transformType];
	DO_INVERSE_TRANSFORM(x_ptr, y_ptr, imatrix, x, y);
    }

    return;
}

/* Backwards compatibility */

void
tr_inverse_oct_transform(transform, x_ptr, y_ptr)
struct octTransform *transform;
int32 *x_ptr, *y_ptr;
{
    tr_oct_inverse_transform(transform, x_ptr, y_ptr);
    return;
}

void
tr_get_oct_transform(stack, transform)
tr_stack *stack;
struct octTransform *transform;
{
    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    *transform = (*stack)->transform;
    return;
}

#define COPY_MATRIX(copy, orig)\
    copy[0][0] = orig[0][0]; copy[0][1] = orig[0][1];\
    copy[1][0] = orig[1][0]; copy[1][1] = orig[1][1];
    
void
tr_get_transform(stack, matrix, t_x, t_y, type_ptr)
tr_stack *stack;
double matrix[2][2];
int32 *t_x, *t_y;
int *type_ptr;
{
    double_matrix_ptr dmatrix;
    int_matrix_ptr imatrix;
    struct tr_link *tfm;

    if (stack == NIL(tr_stack) || *stack == NIL(struct tr_link)) {
	return;
    }

    tfm = *stack;
    
    *type_ptr = (int) tfm->transform.transformType;
    *t_x = tfm->transform.translation.x;
    *t_y = tfm->transform.translation.y;

    if (tfm->transform.transformType == OCT_FULL_TRANSFORM) {
	dmatrix = tfm->transform.generalTransform;
	COPY_MATRIX(matrix, dmatrix);
    } else {
	imatrix = simple_matrices[(int)tfm->transform.transformType];
	COPY_MATRIX(matrix, imatrix);
    }

    return;
}

/* 
 * the following two routines supplied by Chuck Kring (kring@ic.berkeley.edu)
 */

/*
 * tr_get_inverse_transform
 *
 * sets inv to the inverse of tr so that
 *
 * inv( tr ) = ( tr )inv = unit transform
 */
void
tr_get_inverse_transform( tr, inv )
struct octTransform *tr;
struct octTransform *inv;
{
    double det;

    inv->translation.x = inv->translation.y = 0;
    tr_oct_inverse_transform( tr, &inv->translation.x, &inv->translation.y );

    switch( tr->transformType ){
	case OCT_NO_TRANSFORM:
	    inv->transformType = OCT_NO_TRANSFORM;
	    break;
	case OCT_MIRROR_X:
	    inv->transformType = OCT_MIRROR_X;
	    break;
	case OCT_MIRROR_Y:
	    inv->transformType = OCT_MIRROR_Y;
	    break;
	case OCT_ROT90:
	    inv->transformType = OCT_ROT270;
	    break;
	case OCT_ROT180:
	    inv->transformType = OCT_ROT180;
	    break;
	case OCT_ROT270:
	    inv->transformType = OCT_ROT90;
	    break;
	case OCT_MX_ROT90:
	    inv->transformType = OCT_MY_ROT90;
		break;
	case OCT_MY_ROT90:
	    inv->transformType = OCT_MX_ROT90;
	    break;
	case OCT_FULL_TRANSFORM:
	    inv->transformType = OCT_FULL_TRANSFORM;
	    det =  tr->generalTransform[ 0 ][ 0 ] *
		   tr->generalTransform[ 1 ][ 1 ] -
		   tr->generalTransform[ 1 ][ 0 ] *
		   tr->generalTransform[ 0 ][ 1 ];
	    inv->generalTransform[ 0 ][ 0 ] = 
		tr->generalTransform[ 1 ][ 1 ] / det;
	    inv->generalTransform[ 0 ][ 1 ] = 
		tr->generalTransform[ 0 ][ 1 ] / -det;
	    inv->generalTransform[ 1 ][ 0 ] = 
		tr->generalTransform[ 1 ][ 0 ] / -det;
	    inv->generalTransform[ 1 ][ 1 ] = 
		tr->generalTransform[ 0 ][ 0 ] / det;
	    break;
	}
}

/*
 * tr_delta_transfrom
 *
 * sets tr to the transform needed to transform oldTr to newTr
 *
 * tr = ( inv( oldTr ))( newTr )
 */
void
tr_delta_transform( oldTr, newTr, tr )
struct octTransform *oldTr;
struct octTransform *newTr;
struct octTransform *tr;
{
    struct octTransform inv;
    tr_stack *stack;

    tr_get_inverse_transform( oldTr, &inv );
    stack = tr_create();
    tr_add_transform( stack, &inv, 1 );
    tr_add_transform( stack, newTr, 1 );
    tr_get_oct_transform( stack, tr );
    tr_free( stack );
}


#ifdef TBL_GEN
main()
{
    int lm,i,j;
    int result[2][2];
    int result_type;
    int_matrix_ptr a1,a2;

    printf("static int op_table[2][8][8] = {\n    {\n");

    for (i = 0; i < 8; i++) {
	a1 = simple_matrices[i];
	printf("\t{");
	for (j = 0; j < 8; j++) {
	    a2 = simple_matrices[j];
	    RIGHT_MULTIPLY(result,a1,a2);
	    result_type = look_up_matrix(result);
	    if (result_type == OCT_FULL_TRANSFORM) {
		fprintf(stderr, "Cant find the result of %s x %s\n",
			tr_op_name[i], tr_op_name[j]);
		exit(-1);
	    }
	    printf("%s,", tr_op_name[result_type]);
	}
	printf("},\n");
    }
    
    printf("    },\n    {\n");

    for(i = 0; i < 8; i++) {
	a1 = simple_matrices[i];
	printf("\t{");
	for (j = 0; j < 8; j++) {
	    a2 = simple_matrices[j];
	    LEFT_MULTIPLY(result,a1,a2);
	    result_type = look_up_matrix(result);
	    if (result_type == OCT_FULL_TRANSFORM) {
		fprintf(stderr, "Cant find the result of %s x %s\n",
			tr_op_name[j], tr_op_name[i]);
		exit(-1);
	    }
	    printf("%s,", tr_op_name[result_type]);
	}
	printf("},\n");
    }

    printf("    }\n};\n");
}

look_up_matrix(result)
int result[2][2];
{
    int op, i;
    int_matrix_ptr mat;

    for(op=0; op<8; op++) {
	mat = simple_matrices[op];
	if (mat[0][0] == result[0][0] &&
	        mat[0][1] == result[0][1] &&
	        mat[1][0] == result[1][0] &&
	        mat[1][1] == result[1][1]) {
	    return op;
	}
    }

    return OCT_FULL_TRANSFORM;
}
#endif
