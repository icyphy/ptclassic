/* paramStructs.h  edg
Version identification:
$Id$
*/

#ifndef __cplusplus
#ifndef const
#define const /* nothing */
#endif
#endif

/* Data Structures */
struct ParamStruct {
    const char *name;
    const char *type;
    const char *value;
};
typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length; /* length of array */
    ParamType *array; /* points to first element */
};
typedef struct ParamListStruct ParamListType;

extern boolean PStrToPList();
extern char * PListToPStr();
