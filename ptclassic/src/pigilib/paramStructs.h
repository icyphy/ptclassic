/* paramStructs.h  edg
Version identification:
$Id$
*/

/* Data Structures */
struct ParamStruct {
    char *name;
    char *value;
};
typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length; /* length of array */
    ParamType *array; /* points to first element */
};
typedef struct ParamListStruct ParamListType;

extern boolean PStrToPList();
extern char * PListToPStr();
