This software goes with the ACS domain Annapolis Wildforce board

cd9805/	        Contents of the cd.

wf4/		The WF4 package

Diagnostics/	links from /var/tmp on andrews
sparc/


With the card installed, the software installation failed as follows:  
## Executing postinstall script.
At postinstall - BASE DIRECTORY is  /users/ptdesign/vendors/annapolis/wf4 
Installing the pwfdrv driver...
cp -p sparc/pwfdrv /platform/sun4u/kernel/drv/pwfdrv
-rw-rw-rw-   1 root     other     147892 May  1 07:24 /platform/sun4u/kernel/drv/pwfdrv
Driver (pwfdrv) not installed.
drvconfig: Driver (pwfdrv) failed to attach
Warning: Driver (pwfdrv) successfully added to system but failed to attach
Checking module info...
Adding links for installed boards...
/dev/pwone*: No such file or directory
/dev/pwfour*: No such file or directory
/dev/pw*: No such file or directory





To: Bill Hulbert <hulbert@annapmicro.com>
cc: James Lundblad <jamesl@eecs.berkeley.edu>, pauer@sanders.com,
    eal@carson.eecs.berkeley.edu
Subject: Re: WildForce VHDL? 
In-reply-to: Your message of Fri, 08 May 1998 15:00:38 -0400.
             <3.0.5.32.19980508150038.01ea77c0@annapmicro.com> 
Date: Fri, 08 May 1998 15:39:27 -0700


I picked up the Wildforce cdrom from my mailbox about an hour ago and
installed it locally.

The board is not installed yet, so I can really test anything.
I did go ahead and attempt to install the software, and the
installation mostly worked.

Below are some minor nits in the installation process.

James, locally, the software is installed in
~ptdesign/vendors/annapolis 
That directory contains the following subdirectories:

cd9805/	        Contents of the cd.

bin/		The WF4 package
driver/
include/
lib/
src/

Diagnostics/	links from /var/tmp on julian
sparc/

We could try installing the board sometime next week.


Here are those nits:

1. If you the first line of the install.sh file is
#!/bin/sh
and it is executable, then the user need not do
sh ./install.sh
they can just use
./install.sh

2. The user must have /usr/bin before /usr/ucb in their path, or they
will get the wrong echo, which does not understand \c
I got:
  Please enter the path to your CD-ROM.
  Default is [/CDROM]: \c

3. Most suns use vold, which mount the cd automatically.
I had to type /cdrom/wildforce for the directory.
Most (all?) Suns will have /cdrom as the directory, not /CDROM.


  Please enter the path to your CD-ROM.
  Default is [/CDROM]: /cdrom
  The directory you have specified: /cdrom
  is not the correct path to your CD-ROM.

  Please enter the path to your CD-ROM.
  Default is [/CDROM]: /cdrom/wildforce
  The diagnostic tests may be used to determine if your
  WILDFORCE(tm) board is in proper working order.  They may
  either be copied to the installation directory, or run from
  the CD-ROM.  Please note that they are rather large in size
  (about 50 Meg).  Would you like to have the diagnostic tests
  copied locally?
  
  Default(no) [yes/no]: 

Here's what my /cdrom directory looks like
  carson.eecs 10# ls /cdrom
  cdrom0@     wildforce/
  carson.eecs 11# 

4.  The Solaris package wants to write to /usr/lib, which
is a bit of a no-no.  

  Installing WildForce(tm) Software Development Tools as <WF4>

  ## Installing part 1 of 1.
  WARNING: /usr/lib/libWF4.so <not present on Read Only file system>

Usually /usr is not writable by the Sun clients, but the
installation must occur on the machine that has the Wildforce board,
which is usually going to be a client.  

To fix this, I did the following as root on the server:
brahe.eecs 85# cd /usr/lib
brahe.eecs 86# ln -s /users/ptdesign/vendors/annapolis/lib/libWF4.so .

5. The postinstall script failed for me, probably because I don't
have the board installed :-)

It would be nice if the drvconfig command was displayed so I could run
it by hand

  ## Executing postinstall script.
  At postinstall - BASE DIRECTORY is  /users/ptdesign/vendors/annapolis 
  Installing the pwfdrv driver...
  cp -p sparc/pwfdrv /platform/sun4u/kernel/drv/pwfdrv
  -rw-rw-rw-   1 root     other     147892 May  1 07:24 /platform/sun4u/kernel/drv/pwfdrv
  Driver (pwfdrv) not installed.
  drvconfig: Driver (pwfdrv) failed to attach
  Warning: Driver (pwfdrv) successfully added to system but failed to attach
  Checking module info...
  Adding links for installed boards...
  /dev/pwone*: No such file or directory
  /dev/pwfour*: No such file or directory
  /dev/pw*: No such file or directory

  Installation of <WF4> was successful.


6. The installation of the diagnostic tests failed:

  The following packages are available:
    1  WF4     WildForce(tm) Software Development Tools
               (sparc) Solaris 2.6 V1.0
  
  Select package(s) you wish to process (or 'all' to process
  all packages). (default: all) [?,??,q]: 

  Installing WILDFORCE(tm) diagnostic tests...
  cp: cannot access /var/tmp/wfcdrom/diagtsts/*
  Please follow the rest of the installation instructions specified in
  the WILDFORCE(tm) Installation Guide.

  The software package is in /var/tmp/wildforce/sparc/V1.0.
  If you did not elect to install the software, it can be
  installed later with the command:
 
	/usr/sbin/pkgadd -d /var/tmp/wildforce/sparc/V1.0
 
  If you did install the software, you may remove the package
  source files with the command:
	rm -r  /var/tmp/wildforce
 
  Installation complete.
  julian.eecs 20# 

It would be nice if the diagnosics software was put somewhere other
than /var/tmp.  Some machines will not have 36Mb free in /var.
This package could be installed in the same location as the other
Solaris package.

Here's what my /var/tmp looks like:
julian.eecs 22# ls /var/tmp
wildforce/           wsconAAAa000Pf:0.0   wsconAAAa0024X:0.0
wsconAAAa0004D:0.0   wsconAAAa000Vz:0.0   wsconAAAa002op:0.0
wsconAAAa0004b:0.0   wsconAAAa000Za:0.0   wsconAAAa003DG:0.0
wsconAAAa0004d:0.0   wsconAAAa000c1:0.0   wsconAAAa003DU:0.0
wsconAAAa0004v:0.0   wsconAAAa000tL:0.0   wsconAAAa003ZW:0.0
wsconAAAa00053:0.0   wsconAAAa001.p:0.0   wsconAAAa005WM:0.0
wsconAAAa0009Q:0.0   wsconAAAa001C5:0.0
wsconAAAa000IC:0.0   wsconAAAa001Ks:0.0
julian.eecs 23# ls /var/tmp/wildforce
Diagnostics/  sparc/
julian.eecs 24# cd /var/tmp
julian.eecs 25# /usr/ucb/df .
Filesystem            kbytes    used   avail capacity  Mounted on
/dev/dsk/c0t0d0s0      93151   36911   46930    45%    /

I copied the directories in /var/tmp over to the package directory and
created links for them.


7. It looks like the software package did not create a subdirectory
for itself, instead, it wrote all over the parent directory.

I typed in /users/ptdesign/vendors/annapolis, expecting that
the software would be installed in /users/ptdesign/vendors/annapolis/WF4

  Enter path to package base directory [?,q] /users/ptdesign/vendors/annapolis
  Using </users/ptdesign/vendors/annapolis> as the package base directory.

Instead it wrote in that directory. 
julian.eecs 43# ls /users/ptdesign/vendors/annapolis
Diagnostics/  cd9805/       include/      sparc/
bin/          driver/       lib/          src/

I think (but I'm not sure) that most packages install in a
subdirectory.  Usually, one specifies /opt, and the sofware gets
written into /opt/WF4




- -Christopher

- --------

    James-
    
    Is this the CD that just arrived?  If so it should contain the Solaris 2.6
    driver for your Ultra machine.
    
    Bill
    
    
    At 10:39 AM 5/8/98 -0700, you wrote:
    >I had a look at the HOST SOFTWARE CD that came with the WildForce card
    >this morning and could not find the VHDL models anywhere.
    >
    >-- 
    >James A. Lundblad			Visiting Scholar?
    >phone: 510-643-7612			558 Cory Hall
    >fax: 510-642-2739			EECS Department
    >email: jamesl@eecs.berkeley.edu		University of California
    >http://www.eecs.berkeley.edu/~jamesl	Berkeley CA 94720-1772
    >
    ____________________________________________________________
    Bill Hulbert
    Annapolis Micro Systems, Inc.
    190 Admiral Cochrane Drive, Suite 130
    Annapolis, MD  21401
    Phone:     410.841.2514    Fax:  410.841.2518
    email:     hulbert@annapmicro.com
    Web site:  http://www.annapmicro.com
    Home of WILDFIRE™ Reconfigurable Computing Engines
- --------

------- End of Blind-Carbon-Copy
