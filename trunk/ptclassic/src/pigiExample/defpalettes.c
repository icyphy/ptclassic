/* defpalettes.c for pigiExample
Version identification:
$Id$
*/

/* This function returns the list of default palettes.  The list must
   "survive" after being returned (be static storage or on the heap).
 */

char* defaultPalettes() {
	return "$PTOLEMY/src/domains/sdf/icons/main.pal:$PTOLEMY/src/domains/sdf/icons/dsp.pal:$PTOLEMY/src/domains/ddf/icons/ddf.pal:$PTOLEMY/src/domains/de/icons/de.pal:./user.pal:./init.pal";
}

/* This defines the default domain for pigiRpc */

char DEFAULT_DOMAIN[] = "SDF";
