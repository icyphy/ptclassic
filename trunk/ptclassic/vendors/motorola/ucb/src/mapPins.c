/* $Id$ */ 
/* contains the translation between the DSP pins and the values received
 * over the socket. 
 *
 * contains the translation for constructing the corresponding pin map 
 * array for use by sim56000.  This is used to
 * tranlate between the order in which ThorDSP56000 wants to send and
 * receive pin values (all inputs, all outputs, all bipuuts), and the
 * numerical ordering of the pins in sim56000.
 */

#include <stdio.h>

#define NUM_PINS 74

mappins()		/* create pin number translation table */
{
	int i,t;
	extern int pin_map[NUM_PINS]; 

	for(i=0;i<=2;i++)	pin_map[i]=i;
	pin_map[3] = 8;
	for(i=4;i<=8;i++)	pin_map[i]=(i-1);
	pin_map[9] = 9;
	t=34;
	for(i=10;i<=25;i++)	pin_map[i]=t++;
	t=10;
	for(i=26;i<=49;i++)	pin_map[i]=t++;
	for(i=50;i<=73;i++)	pin_map[i]=i;

		
/*** DIAG 
	for (i = 0; i < NUM_PINS; i++) 
	fprintf(stdout, "table[%d]  ==  %d\n", i, pin_map[i]);
***/

} /* mappins() */

