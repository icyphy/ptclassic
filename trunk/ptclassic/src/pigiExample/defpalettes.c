/* defpalettes.c for pigiExample
Version identification:
$Id$
*/

/* This function returns the list of default palettes.  The list must
   "survive" after being returned (be static storage or on the heap).
 */

char* defaultPalettes() {
	return "~ptolemy/src/domains/sdf/icons/main.pal:~ptolemy/src/domains/sdf/icons/dsp.pal:~ptolemy/src/domains/ddf/icons/ddf.pal:~ptolemy/src/domains/de/icons/de.pal:./user.pal:./init.pal";
}
