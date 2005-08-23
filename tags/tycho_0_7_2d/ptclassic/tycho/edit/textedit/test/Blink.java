/* 
Authors: Sun Microsystems

Version: @(#)Blink.java	1.1 10/21/97

Copyright (c) 1996 The Regents of the University of California.
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


/*
 * Copyright (c) 1994 Sun Microsystems, Inc. All Rights Reserved.
 */

import java.awt.*;
import java.util.StringTokenizer;

/**
 * I love blinking things.
 *
 * @author Arthur van Hoff
 */
public class Blink extends java.applet.Applet implements Runnable {
    Thread blinker;
    String lbl;
    Font font;
    int speed;
    boolean offset;

    public void init() {
	font = new java.awt.Font("TimesRoman", Font.PLAIN, 24);
	String att = getParameter("speed");
	speed = (att == null) ? 400 : (1000 / Integer.valueOf(att).intValue());
	att = getParameter("lbl");
	lbl = (att == null) ? "Blink" : att;
    	offset=false;
     }
    
    public void paint(Graphics g) {
	int x = 0, y = font.getSize(), space;
	int red = (int)(Math.random() * 50);
	int green = (int)(Math.random() * 50);
	int blue = (int)(Math.random() * 256);
	Dimension d = size();

	g.setColor(Color.black);
	g.setFont(font);
	FontMetrics fm = g.getFontMetrics();
	space = fm.stringWidth(" ");
	for (StringTokenizer t = new StringTokenizer(lbl) ; 
            t.hasMoreTokens() ; ) {

	    String word = t.nextToken();
	    int w = fm.stringWidth(word) + space;
	    if (x + w > d.width) {
		x = 0;
		y += font.getSize();
	    }
	    if (Math.random() < 0.5) {
		g.setColor(new java.awt.Color(
                    (red + y * 30) % 256, (green + x / 3) % 256, blue));
	    } else {
		g.setColor(Color.lightGray);
	    }
	     if (offset)
		y = y*2;
	    g.drawString(word, x, y);
	    x += w;
		if (offset) {
			offset = false;
			y = y /2;
		}else {
			offset = true;
		}
	}
    }

    public void start() {
	blinker = new Thread(this);
	blinker.start();
    }
    public void stop() {
	blinker.stop();
    }
    public void run() {
	while (true) {
	    try {
		Thread.currentThread().sleep(speed);
	    } catch (InterruptedException e) {
	    }
	repaint();
	}
    }
}
