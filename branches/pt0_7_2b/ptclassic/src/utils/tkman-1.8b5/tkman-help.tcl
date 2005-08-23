   $t insert end {A bird?  A plane?  TkMan!  (TkPerson?)

 by Tom Phelps 

implemented in John Ousterhout's Tcl 7.5/Tk 4.1, icon drawn by Rei Shinozuka
 Compatible with Sun Solaris, SunOS, Hewlett-Packard HP-UX, OSF/1, DEC Ultrix, AT&T System V, SGI IRIX, Linux, SCO, IBM AIX, FreeBSD, BSDI -- each of which, believe you me, is in some way different from all the others 

Copyright (c) 1993-1996  Thomas A. Phelps
 All Rights Reserved.
 University of California, Berkeley
 Department of Electrical Engineering and Computer Science
 Computer Science Division 

The latest version of TkMan is always available by anonymous FTP at ftp.cs.Berkeley.EDU in the /ucb/people/phelps/tcltk directory. 

PERMISSION IS GRANTED TO DISTRIBUTE THIS SOFTWARE FREELY, WITH THE EXCEPTION THAT ONE MAY NOT CHARGE FOR IT OR INCLUDE IT WITH SOFTWARE WHICH IS SOLD. Walnut Creek, O'Reilly -- this means you too! 

If you send me bug reports and/or suggestions for new features, include your MANPATH, the versions of TkMan, Tcl, Tk, X, and UNIX, your machine and X window manager names, the edited Makefile, a copy of your ~/.tkman file,  and the first few lines of the tkman executable.  Otherwise don't waste my time. 


Introduction

A graphical manual page browser, TkMan offers two major advantages over man(1) and xman: hypertext links to other man pages (click on a word in the text which corresponds to a man page, and you jump there), and better navigation within long man pages with searches (both incremental and regular expression) and jumps to section headers.  TkMan also offers some convenience features, like a user-configurable list of commonly used man pages, a one-click printout, and integration of whatis and apropos. Further, one may highlight, as if with a yellow marker, arbitrary passages of text in man pages and subsequently jump directly to these passages by selecting an identifying excerpt from a pulldown menu.  Finally, TkMan gives one control over the directory-to-menu volume mapping of man pages with a capability similar to but superior to xman's mandesc in that rather than forcing all who share a man directory to follow a single organization, TkMan gives control to the individual.  For instance, one may decide he has no use for a large set of man pages--say for instance the programmer routines in volumes 2, 3, 4, 8--and eliminate them from his personal database, or a Tcl/Tk programmer may decide to group Tcl/Tk manual pages in their own volume listing. 

Other features include:
 * full text search of manual pages (with Glimpse; optional)
 * when multiple pages match the search name, a pulldown list of all matches
 * regular expression searches for manual page names
 * a list of recently added or changed manual pages
 * a "history" list of the most recently visited pages
 * a preferences panel to control fonts, colors, and other system settings
 * compatibility with compressed pages (both as source and formatted)
 * diagnostics on your manual page installation
 * elision of those unsightly page headers and footers
 * and, when attempting to print a page available only in formatted form, reverse compilation into [tn]roff source, which can then be reformatted as good-looking PostScript. 

This help page is shipped with the distribution in HTML form for easy printing. 


Using TkMan

Locating a man page

There are several ways to specify the manual page you desire.  You can type its name into the entry box at the top of the screen and press Return or click the man button.  The name may be just the name of the command or may end with .n or (n), where n specifies in which section to look.  Man pages are matched using regular expressions, so you can use . to match any single character, * to match any (zero or more) of the previous regular expression, [ .. ] to match any single character in the enclosed class; see egrep for more information.  For instance, .*mail.*.1 searches section 1 (user commands) for commands with "mail" anywhere in their names. Likewise, one can collect all the various manual pages relating to Perl 5 with perl.*, or see a list of all X window managers with .*wm. If you're running TkMan from a shell and giving it an initial man page name to load up as an argument, use this syntax (adequately quoted for protection from the shell), as opposed to the syntax of the standard man command (which is man section name--that is, the section number comes first, whereas in TkMan it is part of the name. 

Usually TkMan searches the directories in your MANPATH environment variable for the man page, but you may instead provide a path name for the man page by beginning it with  `~', `/', `.' or `..'; this is the way to access a man page which isn't installed in a MANPATH man directory.  Further, other Tcl interpreters may display a man page in TkMan by sending a message to the function manShowMan with the name of the desired man page, e.g. send tkman manShowMan tcl.n.  If multiple man page names match the specification, the first match (as searched for in MANPATH order) is shown and a pulldown menu appears which contains a list of the other matches.  Return from reading help or a volume listing to the last man page seen with C-m when the focus is in the main text display area. 

apropos information is available by typing the name and clicking apropos or hitting Shift-Return.  The output of apropos is piped through sort and uniq to remove duplicates.  To pass the matches through additional filters, simply give the pipe as in a shell, e.g., `search | grep ^g' (each space character is significant) returns all the search-related commands which begin with the letter g. The results of the last apropos query are available under the Volumes menu. 

You may also see a button for glimpse, a full text search program that requires only small index files ("typically 2-5% the size of the original text" but larger percentages for smaller amounts of text),  written by Udi Manber, Sun Wu, and Burra Gopal of the University of Arizona's Department of Computer Science.  In their performance measurements, "a search for Schwarzkopf allowing two misspelling errors in 5600 files occupying 77MB took 7 seconds on a SUN IPC".  For example, one may search for the string `WWW' anywhere in any manual page by typing in `WWW' in the entry line at the top of the screen and clicking on the glimpse button or typing Meta-Return (for meta-information, of course). Escape and C-g can interrupt a search after the current directory is done. To employ glimpse's command line options, simply place them before the search pattern in the entry box, or add them to the default options by editing the man(glimpse) variable in your ~/.tkman startup file (see Customizing TkMan, below). For instance, to search for "perl" as a word and not part of another word (as in "properly"), glimpse for -w perl. Glimpse supports an AND operation denoted by the symbol `;' and an OR operation denoted by the symbol `,'. Refer to the glimpse manual page for more information. The regular expression used by glimpse automatically sets the intrapage search  expression.  For this reason, the case sensitivity of the glimpsing is set to the same intrapage. A complete set of matches from the last full text search is available under the Volumes menu. 

The Paths pulldown gives you complete control over which directory hierarchies of your MANPATH are searched for man pages and apropos information.  You can call up a listing of all man pages in a volume through the Volumes pulldown menu and then select one to view by double-clicking on its name. New `pseudo-volumes' can be added, and arbitrary directories may be added to or deleted from a volume listing using tkmandesc commands, described below. In a volume listing, typing a letter jumps to the line in the listing starting with that letter (capital and lower case letters are distinct).  Return to the last volume seen with C-d when the focus is in the main text display area. 

Whenever you have a man page name in the text display box, whether from apropos, a volume listing or a reference within another man page, you can double-click on it to hypertext-jump to it.  Pressing shift while double-clicking opens up a new viewer box to display the page. 

The last few man pages you looked at can be accessed directly through the History pulldown menu.  The list is sorted top to bottom in order of increasing time since that page was last visited. Shortcuts lists your personal favorites and is used just like History, with the additional options of adding the current man page (by clicking +) or removing it (-) from the list.  A shift-click on - removes all shortcuts. 

(Man pages specified as above are processed through an nroff filter. TkMan can also read raw text from a file or from a command pipeline, which can then be read, searched and highlighted same as a man page.  To read from a file, make the first character in the name a <, as in <~/foo.txt.  To open a pipe, make the first character a | (vertical bar), as in `|gzcat foo.txt.gz' or `|cat ../foo.txt | grep bar' (that's no space after the first |, a space before and after any subsequent ones). After reading a file in this way, the current directory is set to its directory. Commands are not processed by a shell, but the metacharacters ., .., ~ and $ (for environment variables), are expanded nonetheless.  Typing is eased further by file name completion, bound to Escape.  Lone files (i.e., not part of a pipe) are automatically uncompressed--no need to read compressed files through a zcat pipe.  It is not expected that reading raw text will be done much; it is included so the occasional non-man page documentation, say, a FAQ or RFC, may be read from the same environment.) 


Working within a man page

The full pathname of the current manual page is shown at the top of the screen.  Via the Preferences dialog, this can be changed to the whatis information for the page. (whatis information is build by catman.) 

To the extent it follows conventional formatting, a manual page is parsed to yield its section and subsection titles (which are directly available from the Sections pulldown) and references to other man pages from throughout the page including its SEE ALSO section (Links pulldown).   One may jump directly to a section within a man page or a referenced man page, respectively, by selecting the corresponding entry from one of these pulldowns. 

Within a man page or raw text file or pipe, you may add ad hoc highlighting, as though with a yellow marker (underlining on monochrome monitors).  Highlighted regions may then be scrolled to directly through the Highlights pulldown menu.  To highlight a region, select the desired text by clicking button 1, dragging to the far extent of the desired region and releasing the button, then click on the + next to Highlights.  To remove any highlights or portions thereof in a region, select it as before but then click on -.  A shift-click on the menu title tours through all the highlights on the page.  A shift-click on - removes all highlights on the page.  A complete set of pages with highlighting is available under the Volumes menu. 

Highlighting information is robust against changes to and reformatting of the page.  Here's how highlight reattachment works. When you highlight a region, the starting and ending positions are saved along with some the content of the highlighted region and context.   When that page is viewed again, if those positions still match the context, the highlight is attached there (this is an exact match).  If not, the context is searched forward and backward for a match, with the closer match chosen if there are matches in both directions (a repositioned match).   If no match is found with the full context, gradually less and less of it is tried,  reasoning that perhaps the content of the context has been changed (repositioned, but with less confidence, triggering a warning dialog). If still no match is found (an orphan), the highlight is reported at the bottom of the page, where it must be reattached manually before leaving the page or it will be forgotten.  (Before TkMan v1.8b4, highlights were attached by positions only, and when the page modification date changed, the user had the choice of applying  highlights at those same positions regardless of the text there now or throwing out the highlights wholesale.  Old style highlights are automatically updated to the new style that can be automatically and robustly repositioned.  The next time an old style page is viewed, the old style highlights are applied as before, and from those postions new style highlights are composed.)  The annotation reattachment mechanism is inspried by Stanford's ComMentor system. 

You can move about the man page by using the scrollbar or typing a number of key combinations familiar to Emacs aficionados.  Space and C-v page down, and delete and M-v page up.  C-n and C-p scroll up and down, respectively, by a single line (vi fans will be happy to hear that C-f and C-b also page down and page up, respectively). M-< goes to the head and M-> to the tail of the text.  One may "scan" the page, which is to say scroll it up and down with the mouse but without the use of the scrollbar, by dragging on the text display with the middle mouse button pressed.  Like Emacs, C-space will mark one's current location, which can be returned to later with C-x, which exchanges the then-current position with the saved mark; a second C-x swaps back. 

C-s initiates an incremental search.  Subsequently typing a few letters attempts to find a line with that string, starting its search at the current match, if any, or otherwise the topmost visible line.   A second C-s finds the next match of the string typed so far.  (If the current search string is empty, a second C-s retrieves the previous search pattern.) C-r is similar to C-s but searches backwards. Escape or C-g cancels searching. Incremental search can be used to quickly locate a particular command-line option or a particular command in a group (as in csh's long list of internal commands). At the bottom of the screen, type in a regular expression to search for (see Tcl's regexp command), and hit return or click Search to begin a search.  Hit C-s, click the down arrow or keep hitting return to search for the next occurance; likewise C-r or the up arrow for previous occurances. 

The Tab key moves the focus from the man page type-in line to the text view of the man page to the search line and back around.  Shift-Tab jumps about in the opposite direction. 


Other commands

The Occasionals menu holds commands and options which you probably won't frequently.  Help returns to this information screen.  Although virtually made obsolete by TkMan, Kill Trees makes a printout of the current man page on dead, cut, bleached trees, helping to starve the planet of life-giving oxygen.  A list of printers appears in the cascade menu; this list may be edited in Preferences/Misc.  (Even if only one printer is available, it placed in the cascade menu, rather than being directly available.  This is a feature.) (If the [tn]roff source is not available, TkMan asks if it should try to reverse compile the man page.  If successful, this produces much more appealing output than an ASCII dump.)  By default, incremental searching is not case sensitive, but regular expression searching is; these settings can be toggled with the next two menus.  iff upper means that searching is case sensitive if and only if their is at least one uppercase letter in the search expression--that is, all lowercase searches are not case sensitive; this idea is taken from Emacs. With proportional fonts giving a ragged right margin, any change bars in the right margin will form an uneven line; by opting for Changebars on left, they will form a straight line at the left margin. 

As with xman one may instantiate multiple viewers.  When there is more than one viewer you may choose man pages in one viewer and have their contents shown in another.  Use the Output pulldown (which appears and disappears as relevant) to direct one viewer's output destination to another.  With this feature one may easily compare two similar man pages for differences, keep one man page always visible, or examine several man pages from a particular volume listing or a SEE ALSO section.  Output only affects the display destination of man pages. TkMan uses a database of all manual page names in searching for a match for a particular name.  This database is constructed automatically if it doesn't exist (this includes the first time TkMan is run for a particular user) and whenever it is out of date due to pages being added or changed, or changes in one's MANPATH or tkmandesc commands.  (If you want to add paths to your MANPATH, or edit ~/.tkman, you will have to restart to see any changes take effect, however.)  If you install new manual pages, invoking Rebuild Database will permit them to show up in the next search or volume listing without the bother of quitting and re-executing TkMan.  Rebuild Glimpse Database creates and then maintains the index that is used for full text searches.  The Glimpse database is not maintained automatically. When exited via the Quit button TkMan saves its state.  One may guard against losing highlighting, shortcuts and other what-should-be persistent information without quitting by by invoking Checkpoint state to .tkman; Quit, don't update performs the opposite operation. 

At the bottom right corner of the screen, Mono toggles between the proportionally-spaced font and a monospaced one, for use in those man pages that rely on a fixed-width font to align columns.  Quit exits TkMan, of course, after saving some state information (see below).  To exit without saving status information, select the Quit option from the Occasionals pulldown. 


Preferences

The Preferences... choice in the Occasionals pulldown menu brings up a graphical user interface to setting various attributes of TkMan, including fonts, colors, and icons.  Click on a checkbutton at the top of the window to bring up the corresponding group of choices.  After making a set of choices, the Apply button reconfigures the running application to show these changes, OK sets the changes for use now and in the future, Cancel quits the dialog and sets all choices to their settings as of the time Preferences was called up, and Defaults resets the settings in the current group to those set by TkMan out of the box. 

The first line in the Fonts group specifies the font to use for the general user interface, which amounts to the labels on buttons and the text in menus.  The first menu in the line labeled Interface sets the font family, the next menu sets the font size, and the last the font styling (normal, bold, italics, bold-italics). Text display makes these settings for the text box in which the manual pages contents are displayed.  For listings of all man pages in a particiular volume (as chosen with the Volumes menu), you may wish to use a smaller font so that more names fit on the screen at once. Screen DPI specifies the right set of fonts to use for your monitor. 

Colors sets the foreground and background colors to use for the manual page text display box, the general user interface, and the buttons of the user interface.  In addition it sets the color (or font) in which to show various classes of text in the text box, including manual page references, incremental search hits, regular expression search hits, and highlighted regions. 

The See group specifies what information to display. Usually manual page headers and footers are uninteresting and therefore are stripped out, but a canonical header and footer (along the date the page was installed in the man/mann directory or formatted to the man/catn directory) to be shown at the bottom of every page can be requested.  Solaris and IRIX systems come with many "subvolumes"--that is volumes with names like "3x" and "4dm" that form subgroupings under the main volumes "3" and "4", respectively--and you make use tkmandesc commands to add your own subvolumes.  You can reduce the length of the main Volumes menu by placing all volumes in such groups as cascaded menus. When a highlighted passage is jumped to via the Highlights menu, some number of lines of back context are included; the exact number of lines is configurable.   The information bar at the top of the window can display either the short, one-line description from a manual page's NAME section or the pathname of the page.   Around the man page display area runs a buffer region, the exact width of  which is configurable. Tk deviates from Motif behavior slightly, as for instance in highlighting buttons when they're under the cursor, but you can observe strict Motif behavior. 

The Icon group sets all the options relating to iconification. The pathnames of the icon bitmap and icon mask should be the full pathnames (beginning with a `/'). 

If a man page has not been formatted by nroff, TkMan must first pipe the source text through nroff.  By turing on Cache formatted (nroff'ed) pages in the Misc(ellaneous) group, the nroff-formatted text is saved to disk (if possible), thereby eliminating this time-consuming step the next time the man page is read.  The on & compress setting will compress the page, which saves on disk space (often substantially as much of a formatted page is whitespace), but will make it unavailable to other manual pagers that don't handle compression.   By default man page links are activated by double clicking.  The first click puts the name in the entry box so that it can be used as the apropos or glimpse pattern as well.  This click once to select, twice to launch follows the Macintosh convention.  Since TkMan was written, some hypertext browsers have popularized a click-once-to-launch practice.  The Mouse click to activate hyperlink switch chooses between the two. TkMan can extract section headers from all manual pages, but only some manual page macros format subsection headers in a way that can be distinguished from ordinary text; if your macros do, turn this option on to add subsections to the Sections menu. The History pulldown must balance depth of the list against ease of finding an entry; set your own inflection point with this menu.  Volumes' (recent) choice will show all manual pages that have been added or changed n days, where n is set with this next menu.  Glimpse works best when searching for relatively uncommon words; guard against getting too many hits by setting the maximum number reported. By default Glimpse indexes are placed at the root of the corresponding man hierarchy, where they can be shared.  For the case when an individual may not have write permission there, a single, unified index can be created and stored locally (though you lose control of it from the Paths settings). Unified indexes are much faster that distributed. In this latter case and also for "stray cats" (i.e., directories not part of a set of man hierarchy directories), you should specify an auxiliary directory to hold the index. Proportional spacing wrecks the spacing used to set tables in columns, hence the Mono(space) button on the bottom line of the main screen. 


Customizing TkMan

There are four levels of configuration to TkMan. 

(1) Transparent.  Simply use TkMan and it will remember your window size and placement, short cuts, and highlights (if you quit out of TkMan via the Quit button). 

(2) Preferences editor (see Preferences above). 

(3) Configuration file.  Most interesting settings, like the command(s) used to print the man page and some key bindings, can be changed by editing one's own ~/.tkman.  Thus, a single copy of TkMan (i.e., the executable tkman) can be shared, but each user can have his own customized setup.  (The file ~/.tkman is created/rewritten every time one quits TkMan via the Quit button in the lower right corner.  Therefore, to get a ~/.tkman to edit, first run and quit TkMan.  Do not create one from scratch as it will not have the proper format used for saving other persistent information, and your work will be overwritten, which is to say lost.)  Be careful not to edit a ~/.tkman file only to have it overwritten when a currently-running TkMan quits. 

Options that match the defaults are commented out (i.e., preceded by a #).  This is so that any changes in the defaults will propagate nicely, while the file still lists all interesting variables. To override the default settings for these options, first comment in the line. 

The ~/.tkman save file is the place to add or delete colors to the default set, which will subsequently become menu choices in Preferences, by editing in place the variable man(colors).  One may also edit the order of Shortcuts in the man(shortcuts) variable. Other interesting variables include man(highlight), which can be edited to change the background in place of the foreground, or both the foreground and background, or a color and the font as with the following setting:
set man(highlight) {bold-italics -background #ffd8ffffb332} 

Arbitrary Tcl commands, including tkmandesc commands (described below), can be appended to ~/.tkman (after the ### your additions go below line). 

To set absolutely the volume names for which all directories should be searched, edit the parallel arrays on these existing lines:
set man(manList) ...
set man(manTitleList) ...
 Changing the order volumes in these lists (make sure to keep the two lists in parallel correspondence) changes the precedence of matches when two or more pages have the same name: the page found in the earlier volume in this list is show first. 

Additional useful commands include wm(n), which deals with the window manager; bind(n), which changes keyboard and mouse bindings not related to the text display window; options, which sets the X defaults; and text(n), which describes the text widget. 

(4) Source code.  Of course, but if you make generally useful changes or have suggestions for some, please report them back to me so I may share the wealth with the next release. 


Command line options

 The environment variable named TKMAN, if it exists, is used to set command line options.  Any options specified explicitly (as from a shell or in a script) override the settings in TKMAN. Any settings made with command-line options apply for the current execution only. Many of these options can be set persistently via the Preferences dialog (under the Occasionals menu). 

-title title 
      Place title in the window's title bar. 

-geometry geometry 
      Specify the geometry for this invocation only.  To assign a persistent geometry, start up TkMan, size and place the window as desired, then (this is important) quit via the Quit button in the lower right corner. 

-iconify and -!iconify 
      Start up iconified or uniconified (the default), respectively. 

-iconname name 
      Use name in place of the uniconified window's title for the icon name. 

-iconbitmap bitmap-path and -iconmask bitmap-path 
      Specify the icon bitmap and its mask. 

-iconposition (+|-)x(+|-)y 
      Place the icon at the given position; -iconposition "" "" cancels any such hints to the window manager. 

-debug or -!debug 
      Generate (or not) debugging information. 

-startup filename 
      Use filename in place of ~/.tkman as the startup file; "" indictates no startup file. 

-database filename 
      Use filename in place of ~/.tkmandatabase as the name of the file in which to create the database of man page names.  This can point to a shared file to save disk space or share a custom design, or to an OS-specific file for systems with multiple machine architectures that share home directories. 

-rebuildandquit 
      Simply rebuild the database and quit.  This option may be useful in a cron script. 

-quit save and -quit nosave 
      Specify that the startup file (usually ~/.tkman) should be updated (save) or not (nosave) when quitting by the Quit button. 

-v 
      Show the current version of TkMan and exit immediately thereafter. 

-M path-list
 or -M+ path-list
 or -+M path-list 
      As with man, change the search path for manual pages to the given colon-separated list of directory subtrees.  -M+ appends and -+M prepends these directories to the current list. 

-now 
      Start up TkMan without checking to see if the database is out of date. 

--help Display a list of options. 
       


Key bindings

Key bindings related to the text display box are kept in the sb array in ~/.tkman (for more information on Tcl's arrays, refer to the array(n) man page.  In editing the sb(key,...) keyboard bindings, modifiers MUST be listed in the following order: M (for meta), C (control), A (alt).  DO NOT USE SHIFT.  It is not a general modifier: Some keyboards require shift for different characters, resulting in incompatibilities in bindings.  For this reason, the status of the shift key is supressed in matching for bindings.  For instance, set sb(key,M-less) pagestart is a valid binding on keyboards worldwide, whereas set sb(key,MS-less) is not.  To make a binding without a modifier key, precede the character by `-', as in set sb(key,-space) pagedown. 


tkmandesc

Like xman, TkMan gives you directory-by-directory control over named volume contents.  Unlike and superior to xman, however, each individual user controls directory-to-volume placement, rather than facing a single specification for each directory tree that must be observed by all. 

By default a matrix is created by taking the product of directories in the MANPATH crossed with volume names, with the yield of each volume containing all the corresponding subdirectories in the MANPATH.  By adding Tcl commands to your ~/.tkman (see above), you may add new volume names and add, move, copy and delete directories to/from/among directories. 

The interface to this functionality takes the form of Tcl commands, so you may need to learn Tcl--particularly the commands that deal with Tcl lists  (including lappend(n), linsert(n), lrange(n), lreplace(n)) and string matching (string(n), match subcommand)--to use this facility to its fullest.  tkmandesc commands are used to handle the nonstandard format of SGI's manual page directories, and the sgi_bindings.tcl in the contrib directory is a good source of examples in the use of tkmandesc commands. 

Directory titles and abbrevations are kept in lists.  Abbreviations MUST be unique (capital letters are distinct from lower case), but need not correspond to actual directories.  In fact, volume letters specified here supercede the defaults in identifying a volume in man page searches. 


COMMANDS

The following commands are appended to the file ~/.tkman (see Customizing TkMan, above). 

To recreate a cross product of current section lists:
manDescDefaults
 This cross product is made implicitly before other tkmandesc commands. Almost always this is what one expects.  If it is not, one may supress the cross product by setting the variable manx(defaults) to a non-null, non-zero value before other tkmandesc commands are invoked. 

To add "pseudo" sections to the current volume name list, at various positions including at end of the list, in alphabetical order, or before or after a specific volume:
manDescAddSects list of (letter, title pairs)
 or manDescAddSects list of (letter, title) pairs sort
 or manDescAddSects list of (letter, title) pairs before sect-letter
 or manDescAddSects list of (letter, title) pairs after sect-letter
 In manual page searches that produce multiple matches, the page found in the earlier volume is the one shown by default. 

To move/copy/delete/add directories:
manDescMove from-list to-list dir-patterns-list
manDescCopy from-list to-list dir-patterns-list
manDescDelete from-list dir-patterns-list
manDescAdd to-list dir-list 

The dir-patterns-list uses the same meta characters as man page searching (see above).  It is matched against MANPATH directories with volume subdirectory appended, as in /usr/man/man3, where /usr/man is a component of the MANPATH and man3 is a volume subdirectory. from-list and to-list are Tcl lists of the unique volume abbreviations (like 1 or 3X); * is an abbreviation for all volumes. 

Adding directories with manDescAdd also makes them available to Glimpse for its indexing. 

Warning: Moving directories from their natural home slightly impairs searching speed when following a reference within a man page.  For instance, say you've moved man pages for X Windows subroutines from their natural home in volume 3 to their own volume called `X'.  Following a reference in XButtonEvent to XAnyEvent(3X11) first searches volume 3; not finding it, TkMan searches all volumes and finally finds it in volume X.  With no hint to look in volume 3 (as given by the 3X11 suffix), the full volume search would have begun straight away.  (Had you double-clicked in the volume listing for volume X or specified the man page as XButtonEvent.X, volume X would have been searched first, successfully.) 

To help debug tkmandesc scripts, invoke manDescShow to dump to stdout the current correspondence of directories to volumes names. 


EXAMPLES

(1) To collect together all man pages in default volumes 2 and 3 in all directories into a volume called "Programmer Subroutines", add these lines to the tail of ~/.tkman:
manDescAddSects {{p "Programmer Subroutines"}}
manDescMove {2 3} p * 

To place the new section at the same position in the volume pulldown list as volumes 2 and 3:
manDescAddSects {{p "Programmer Subroutines"}} after 2
manDescMove {2 3} p * 

To move only a selected set of directories:
manDescAddSects {{p "Programmer Subroutines"}}
manDescMove * p {/usr/man/man2 /usr/local/man/man3} 

(2) To have a separate volume with all of your and a friend's personal man pages, keeping a duplicate in their default locations:
manDescAddSects {{t "Man Pages de Tom"} {b "Betty Page(s)"}}
manDescCopy *phelps* t *
manDescCopy *page* t * 

(3) To collect the X windows man pages into two sections of their own, one for programmer subroutines and another for the others:
manDescAddSects {{x "X Windows"}} after 1
manDescAddSects {{X "X Subroutines"}} after 3
manDescMove * x *X11*
manDescMove x X *3 

(4) If you never use the programmer subroutines, why not save time and memory by not reading them into the database?
manDescDelete * {*[2348]} (braces prevent Tcl from trying to execute [2348] as a command) 

Alternatively but not equivalently:
manDescDelete {2 3 4 8} * 


tkmandesc vs. xman and SGI

TkMan's tkmandesc capability is patterned after xman's mandesc files.  By placing a mandesc file at the root of a man page directory tree, one may create pseudo volumes and move and copy subdirectories into them.  Silicon Graphics has modified xman so that simply by creating a subdirectory in a regular man subdirectory one creates a new volume.  This is evil.  It violates the individual user's rights to arrange the directory-volume mapping as he pleases, as the mandesc file or subdirectory that spontaneously creates a volume is set a single place and must be observed by all who read that directory.  By contrast, TkMan places the directory-to-volume mapping control in an individual's own ~/.tkman file. This gives the individual complete control and inflicts no pogrom on others who share man page directories.  Therefore, mandesc files are not supported in any way by TkMan. 

One may still share custom setups, however, by sharing the relevant lines of ~/.tkman.  In fact, a tkmandesc version of the standard SGI man page directory setup is included in the contrib directory of the TkMan distribution.  For assistance with SGI-specific directory manipulation, contact Paul Raines (raines@bohr.physics.upenn.edu). 


RosettaMan

TkMan uses RosettaMan to translate and reformat man pages (see man(5)). RosettaMan (called rman in its executable form)  takes formatted man pages from most of the popular flavors of UNIX and transforms them into any of a number of text source formats.  RosettaMan accepts formatted man pages from Hewlett-Packard HP-UX, AT&T System V, SunOS, Sun Solaris, OSF/1, DEC Ultrix, SGI IRIX, Linux, FreeBSD, SCO.  It can produce ASCII-only, section headers-only, TkMan, [tn]roff (source), Ensemble, SGML, HTML, MIME, LaTeX, LaTeX2e, RTF, Perl 5 POD.   A modular architecture permits easy addition of additional output formats.  The latest version of RosettaMan is available from  ftp://ftp.cs.berkeley.edu/ucb/people/phelps/tcltk/rman.tar.Z. 


Author

Tom Phelps
 University of California, Berkeley
 Computer Science Division
 387 Soda Hall #1776
 Berkeley, CA  94720-1776
 USA 

phelps@cs.Berkeley.EDU 


More Information

 My article "TkMan: A Man Born Again" appears in The X Resource, issue 10, pages 33--46.  Here are the section titles: Introduction, Availability, The User Interface, Navigating among Man Pages, Inspecting Individual Man Pages, Customization, Logical Volumes with tkmandesc, Persistency, The RosettaMan Filter, Extensions, Problems, Future Work, Acknowledgements, Bibliography. 

A World Wide Web page that lists other Tcl/Tk software and a related Berkeley Computer Science Division technical report, CSD-94-802, can be found at http://http.cs.berkeley.edu/~phelps/. 


Disclaimer

Permission to use, copy, modify, and distribute this software and its documentation for educational, research and non-profit purposes,  without fee, and without a written agreement is hereby granted,  provided that the above copyright notice, this and the following two paragraphs appear in all copies.   

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 

Help page last revised on $Date$
}
foreach qq {{h1 1.0 1.38} {sc 14.61 14.64} {tt 14.68 14.87} {tt 14.95 14.119} {sc 16.0 16.150} {tt 18.208 18.216} {tt 18.255 18.260} {h1 21.0 21.12} {manref 23.72 23.78} {manref 23.83 23.87} {manref 23.482 23.488} {manref 23.493 23.500} {tt 23.839 23.843} {h1 40.0 40.11} {h2 42.0 42.19} {tt 44.139 44.145} {tt 44.159 44.162} {tt 44.233 44.234} {i 44.234 44.235} {tt 44.239 44.240} {i 44.240 44.241} {tt 44.241 44.242} {i 44.250 44.251} {tt 44.353 44.354} {tt 44.386 44.387} {tt 44.452 44.453} {tt 44.457 44.458} {manref 44.516 44.521} {manref 44.559 44.569} {manref 44.734 44.740} {manref 44.786 44.790} {tt 44.1003 44.1006} {tt 44.1025 44.1028} {i 44.1029 44.1036} {i 44.1037 44.1041} {sc 46.47 46.54} {tt 46.173 46.176} {tt 46.178 46.181} {tt 46.183 46.186} {tt 46.190 46.194} {sc 46.260 46.267} {manref 46.299 46.302} {tt 46.351 46.355} {tt 46.385 46.395} {tt 46.440 46.467} {sc 46.558 46.565} {tt 46.731 46.734} {tt 48.0 48.7} {tt 48.65 48.72} {tt 48.113 48.120} {tt 48.138 48.142} {tt 48.147 48.151} {tt 48.266 48.282} {tt 48.390 48.391} {tt 48.455 48.462} {tt 50.30 50.37} {tt 50.628 50.635} {tt 50.653 50.664} {tt 50.700 50.706} {tt 50.711 50.714} {tt 50.785 50.792} {tt 50.929 50.941} {tt 50.959 50.967} {tt 50.1119 50.1126} {tt 50.1148 50.1151} {tt 50.1184 50.1187} {tt 50.1195 50.1197} {tt 50.1230 50.1233} {manref 50.1248 50.1255} {tt 50.1321 50.1328} {tt 50.1548 50.1555} {tt 52.4 52.9} {sc 52.87 52.94} {tt 52.215 52.222} {tt 52.630 52.633} {tt 56.74 56.81} {tt 56.193 56.202} {tt 56.255 56.262} {tt 56.336 56.337} {tt 56.355 56.356} {tt 56.391 56.392} {tt 58.55 58.60} {tt 58.268 58.269} {tt 58.277 58.287} {tt 58.333 58.334} {tt 58.358 58.375} {tt 58.381 58.407} {tt 58.442 58.443} {tt 58.635 58.636} {tt 58.638 58.640} {tt 58.642 58.643} {tt 58.648 58.649} {tt 58.764 58.770} {tt 58.886 58.890} {h2 61.0 61.25} {manref 63.136 63.142} {manref 63.201 63.207} {tt 65.156 65.164} {sc 65.248 65.256} {tt 65.266 65.271} {tt 67.212 67.222} {tt 67.401 67.402} {tt 67.411 67.421} {tt 67.520 67.521} {tt 67.620 67.621} {tt 67.724 67.731} {tt 71.136 71.139} {tt 71.166 71.169} {tt 71.180 71.183} {tt 71.188 71.191} {tt 71.244 71.246} {tt 71.279 71.282} {tt 71.287 71.290} {tt 71.334 71.337} {tt 71.359 71.362} {tt 71.588 71.595} {tt 71.666 71.669} {tt 71.743 71.746} {tt 73.0 73.3} {tt 73.214 73.217} {tt 73.317 73.320} {tt 73.361 73.364} {tt 73.379 73.382} {tt 73.407 73.413} {tt 73.417 73.420} {tt 73.564 73.567} {manref 73.686 73.692} {tt 73.727 73.733} {tt 73.758 73.761} {tt 73.850 73.853} {tt 75.4 75.7} {tt 75.129 75.138} {h2 78.0 78.14} {tt 80.4 80.15} {tt 80.86 80.90} {tt 80.171 80.181} {tt 80.538 80.546} {sc 80.698 80.703} {tt 80.862 80.871} {tt 80.1208 80.1226} {tt 82.8 82.12} {tt 82.177 82.183} {sc 82.472 82.480} {tt 82.491 82.497} {sc 82.862 82.869} {sc 82.928 82.935} {tt 82.945 82.953} {tt 82.1065 82.1081} {tt 82.1203 82.1227} {tt 82.1377 82.1381} {tt 82.1546 82.1572} {tt 82.1574 82.1592} {tt 84.42 84.46} {tt 84.194 84.198} {tt 84.327 84.331} {tt 84.348 84.359} {h2 87.0 87.11} {tt 89.4 89.18} {tt 89.33 89.44} {tt 89.305 89.310} {tt 89.378 89.380} {tt 89.429 89.435} {tt 89.538 89.546} {tt 91.190 91.199} {tt 91.325 91.337} {tt 91.501 91.508} {tt 91.597 91.607} {tt 95.223 95.230} {i 95.230 95.231} {tt 95.262 95.269} {i 95.269 95.270} {tt 95.618 95.625} {tt 95.736 95.746} {tt 99.40 99.45} {tt 99.93 99.98} {tt 99.114 99.146} {tt 99.181 99.186} {tt 99.320 99.333} {tt 99.899 99.932} {i 99.1062 99.1065} {tt 99.1201 99.1209} {tt 99.1220 99.1227} {tt 99.1349 99.1357} {tt 99.1358 99.1366} {i 99.1433 99.1434} {i 99.1447 99.1448} {tt 99.2222 99.2226} {h1 102.0 102.17} {tt 106.149 106.153} {tt 110.158 110.166} {tt 110.220 110.225} {tt 110.302 110.310} {tt 110.367 110.371} {tt 110.427 110.435} {tt 112.71 112.72} {b 114.143 114.159} {tt 114.173 114.184} {tt 114.235 114.249} {tt 114.296 114.310} {i 114.408 114.411} {i 114.435 114.438} {tt 115.0 115.59} {tt 117.91 117.99} {tt 117.111 117.138} {b 119.81 119.85} {b 119.115 119.123} {tt 120.0 120.20} {tt 121.0 121.25} {manref 124.35 124.40} {manref 124.79 124.86} {manref 124.170 124.177} {manref 124.210 124.217} {h2 129.0 129.20} {sc 131.32 131.37} {sc 131.182 131.187} {tt 131.355 131.366} {tt 133.0 133.7} {i 133.7 133.12} {tt 134.12 134.12} {i 134.12 134.17} {tt 136.0 136.10} {i 136.10 136.18} {tt 137.179 137.183} {tt 139.0 139.8} {tt 139.13 139.22} {tt 142.0 142.10} {i 142.10 142.14} {tt 143.10 143.10} {i 143.10 143.14} {tt 145.0 145.12} {i 145.12 145.23} {tt 145.28 145.38} {i 145.38 145.49} {tt 148.0 148.26} {tt 149.44 149.63} {tt 151.0 151.6} {tt 151.10 151.17} {tt 154.0 154.9} {i 154.9 154.17} {tt 155.10 155.10} {i 155.10 155.18} {tt 155.31 155.39} {tt 157.0 157.10} {i 157.10 157.18} {tt 158.10 158.10} {i 158.10 158.18} {tt 158.31 158.47} {tt 160.0 160.15} {tt 163.0 163.10} {tt 163.15 163.27} {tt 164.45 164.53} {tt 164.74 164.78} {tt 164.88 164.94} {tt 164.117 164.121} {tt 166.0 166.2} {tt 169.0 169.3} {i 169.3 169.12} {tt 170.4 170.8} {i 170.8 170.17} {tt 171.4 171.8} {i 171.8 171.17} {tt 172.14 172.17} {tt 172.117 172.120} {tt 172.133 172.136} {tt 174.0 174.4} {tt 177.0 177.6} {h2 181.0 181.12} {tt 183.61 183.63} {tt 183.73 183.81} {manref 183.134 183.142} {tt 183.169 183.180} {tt 183.249 183.250} {tt 183.263 183.264} {tt 183.276 183.277} {tt 183.534 183.562} {tt 183.614 183.633} {tt 183.721 183.748} {h2 186.0 186.9} {tt 188.5 188.9} {tt 188.110 188.114} {sc 190.75 190.82} {sc 190.195 190.202} {tt 190.236 190.244} {manref 192.161 192.171} {manref 192.173 192.183} {manref 192.185 192.194} {manref 192.196 192.207} {manref 192.230 192.239} {tt 192.241 192.246} {tt 192.401 192.417} {tt 192.425 192.432} {h3 197.0 197.8} {b 199.27 199.35} {tt 199.48 199.56} {tt 202.0 202.15} {tt 203.185 203.199} {tt 206.0 206.16} {i 206.16 206.45} {tt 207.4 207.20} {i 207.20 207.49} {tt 208.4 208.20} {i 208.20 208.49} {i 208.57 208.68} {tt 209.4 209.20} {i 209.20 209.49} {i 209.56 209.67} {tt 213.0 213.12} {i 213.12 213.21} {i 213.22 213.29} {i 213.30 213.47} {tt 214.0 214.12} {i 214.12 214.21} {i 214.22 214.29} {i 214.30 214.47} {tt 215.0 215.14} {i 215.14 215.23} {i 215.24 215.41} {tt 216.0 216.11} {i 216.11 216.18} {i 216.19 216.27} {i 218.4 218.21} {sc 218.110 218.117} {tt 218.171 218.184} {tt 218.192 218.200} {sc 218.223 218.230} {tt 218.235 218.239} {i 218.266 218.275} {i 218.280 218.287} {tt 218.343 218.344} {tt 218.348 218.350} {tt 218.353 218.354} {tt 220.24 220.34} {tt 222.293 222.305} {tt 222.309 222.324} {tt 222.478 222.482} {tt 222.636 222.650} {tt 224.40 224.51} {h3 227.0 227.8} {tt 229.162 229.170} {tt 230.0 230.46} {tt 231.0 231.21} {tt 234.0 234.54} {tt 235.0 235.21} {tt 238.0 238.46} {tt 239.0 239.51} {tt 242.0 242.60} {tt 243.0 243.24} {tt 244.0 244.22} {tt 247.0 247.41} {tt 248.0 248.45} {tt 249.0 249.21} {tt 250.0 250.18} {tt 253.0 253.25} {tt 256.0 256.25} {h3 259.0 259.26} {tt 261.48 261.52} {tt 261.244 261.248} {tt 261.696 261.704} {tt 263.77 263.85} {tt 263.181 263.188} {tt 263.305 263.334} {h1 266.0 266.10} {i 268.11 268.21} {manref 268.63 268.69} {i 268.72 268.82} {manref 268.91 268.95} {i 268.254 268.264} {tt 268.673 268.733} {h1 271.0 271.6} {tt 280.0 280.22} {h1 283.0 283.16} {i 285.49 285.63} {tt 287.150 287.186} {h1 290.0 290.10} {sc 294.0 294.289} {sc 296.0 296.359} } {
	eval $t tag add $qq
}

foreach qq {{introduction1 21.0} {using1 40.0} {locating2 42.0} {working2 61.0} {other2 78.0} {preferences2 87.0} {customizing1 102.0} {command2 129.0} {key2 181.0} {tkmandesc2 186.0} {rosettaman1 266.0} {author1 271.0} {more1 283.0} {disclaimer1 290.0} } {
	eval $t mark set $qq
}

