.\" $Id$
.NA wireless
.SD
Demonstrate shared media communication without graphical connectivity,
using
.c EtherSend
and
.c EtherRec
stars.
Two clusters on the left transmit to two clusters on the right over
two distinct media, radio and infrared.
The communication is implemented using shared data structures between
the stars.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee and T. Parks"
.LD
The
.c EtherSend
and
.c EtherRec
stars are derived from a common base class,
.c Ether.
The base class implements a shared data structure that maintains
a directory to all stars derived from the base class, indexed by the
medium that they share.
In this demonstration, two media are used by each of two
.c EtherSend
stars, called "radio900MHz" and "infrared".
A total of six receivers, three for each medium, are also implemented.
.SA
VirtClock.
.ES
