{configure -canvasheight 358}
{configure -canvaswidth 480}
{configure -scrolling auto}
{centeredText Title title {} black {{helvetica 24 bold i} {times 24 bold i}}}
{centeredText subtitle subtitle title blue {{times 16 bold r} {helvetica 16 bold r}}}
{centeredText Author author {subtitle title} red {{times 16 bold r} {helvetica 16 bold r}}}
{centeredText {version: $Id$} version {author subtitle title} black {{times 16 bold r} {helvetica 16 bold r}}}
{centeredText {Copyright (c) %Q%} copyright {version author subtitle title} black {{times 16 bold r} {helvetica 16 bold r}}}
{add {Simple node} {} {}}
{add Color {{color red}} {{Simple node}}}
{add Font {{font {Times 24 bold i}}} {{Simple node}}}
{add {Color and Font} {{color forestgreen}
     {font {Times 18 bold i}}} Font}
{add {HTML formatted message} {{html {<h3>HTML Field</h3><p>
     This message is contained in the "html"
     field of the contents of the tree node.
     Arbitrary HTML can be included here,
     even with hyperlinks, like this one to
     the <a href="$TYCHO/doc/introduction.html>introduction to Tycho</a>}
     }} {{Simple node}}}
{add Hyperlink {{link {$TYCHO/doc/introduction.html {}}}} {{Simple node}}}
{add {Hyperlink and formatted message} {{link {$TYCHO/doc/introduction.html {}}}
    {html {<h3>HTML Field</h3><p>
    This message is contained in the "html"
    field of the contents of the tree node.
    If both a "link" and an "html" field
    are included, then both will respond to
    a double click.}
    }
    } Hyperlink}
{add {The second tree} {} {}}
{add {A child} {} {{The second tree}}}
