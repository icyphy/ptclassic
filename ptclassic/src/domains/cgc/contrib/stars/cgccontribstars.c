/* This file contains a table of the official CGC_CONTRIB stars.  Its purpose is
 * to force the linker to retrieve the corresponding stars from their
 * library.
 *
 * DO NOT EDIT THIS FILE!  It is generated automatically from the
 * make.template file's definition of PL_SRCS.
 */

#define STR extern char *

 STR star_nm_CGCNeuronBase;
 STR star_nm_CGCMPNeuron;
 STR star_nm_CGCNeuron;
 STR star_nm_CGCSigmoid;
static char** nm_table[] = {
 &star_nm_CGCNeuronBase,
 &star_nm_CGCMPNeuron,
 &star_nm_CGCNeuron,
 &star_nm_CGCSigmoid,
};
