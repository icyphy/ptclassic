/* $Id$ */

#include "flowgraph.h"

#define FLOWVIEW "flowgraph"

#define TBLSIZE 31
#define BUFSIZE 2048
#define STRSIZE 50
#define MAXINDEXLEVEL 20
#define EOS '\0'
#define IN 0
#define OUT 1
#define EXACT "exact"
#define EDGETYPE 0
#define CONTYPE 1
#define NODETYPE 2
#define PARALLEL 0
#define SERIAL 1
#define CLEAR 0
#define SET 1
#define LOWER 0
#define UPPER 1

#define PLUS 	"add"
#define MINUS 	"minus"
#define MULT 	"multiply"
#define DIV 	"divide"
#define LSH	"shift_left"
#define RSH	"shift_right"
#define GT	"cond_g"
#define LT	"cond_l"
#define GTE	"cond_ge"
#define LTE	"cond_le"
#define EQ	"cond_e"
#define NTE	"cond_ne"
#define OR   	"or"
#define EXOR   	"exor"
#define AND	"and"
#define NOT	"inv"
#define MUX	"mux"
#define CONST   "constant"
#define ASSIGN  "equal"
#define DELAY   "delay"
#define COS     "cos"
#define SIN     "sin"
#define SQRT    "sqrt"
#define _MIN     "min"
#define _MAX     "max"
#define LPDELAY "lpdelay"
#define CAST   	"cast"
#define NEG	"negate"
#define NOP	"nop"
#define INPUT   "input"
#define OUTPUT  "output"
#define READ	"read"
#define WRITE	"write"
#define UPSAMPLE "upsample"
#define DOWNSAMPLE "downsample"
#define INTERPOLATE "interpolate"
#define DECIMATE "decimate"
#define TIMEMUX  "timemux"
#define TIMEDEMUX "timedemux"

/* Jan's extra nodes, no libraries yet */
#define INC	"inc"
#define DEC	"dec"
#define BIT     "bit"
#define BITSELECT "bitselect"
#define BITMERGE   "bitmerge"
#define EXIT    "exit"

/* no libraries for these */
#define FUNC    "func"
#define INIT	"init"
#define ITER    "iteration"
#define IF	"if"
#define DO      "do"
#define DOBODY  "dobody"
#define LOOP	"loop"

/* macro's */

#define GP(a) ((GraphPointer) a->Entry)
#define NP(a) ((NodePointer) a->Entry)
#define EP(a) ((EdgePointer) a->Entry)

extern char *malloc();

typedef struct ENTRY *EntryPointer;

typedef struct ENTRY {
    char *Name;
    pointer Entry;
    EntryPointer Next;
} ENTRY;

typedef struct ITEREXP *IterExp;

typedef enum {Inttype, IDtype, OPtype} Itertype;
typedef struct ITEREXP {
    Itertype type;
    int value;
} ITEREXP;
