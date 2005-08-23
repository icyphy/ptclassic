{configure -canvasheight 358}
{configure -canvaswidth 480}
{configure -scrolling auto}
{centeredText Title title {} black {helvetica 24 bolditalic}}
{centeredText subtitle subtitle title blue {times 16 bold}}
{centeredText Author author {subtitle title} red {times 16 bold}}
{centeredText {version: @(#)sample.fst	1.3 03/25/98} version {author subtitle title} black {times 16 bold}}
{centeredText {Copyright (c) 1996} copyright {version author subtitle title} black {times 16 bold}}
{add {Simple node} {} {}}
{add Color {color red} {{Simple node}}}
{add Font {font {Times 24 bold i}} {{Simple node}}}
{add {Color and Font} {color forestgreen font {Times 18 bolditalic}} Font}
{add {HTML formatted message} {html {<h3>HTML Field</h3><p>
     This message is contained in the "html"
     field of the contents of the tree node.
     Arbitrary HTML can be included here,
     even with hyperlinks, like this one to
     the <a href="$TYCHO/doc/introduction.html>introduction to Tycho</a>}
     } {{Simple node}}}
{add Hyperlink {link {$TYCHO/doc/introduction.html {}}} {{Simple node}}}
{add {Hyperlink and formatted message} {link {$TYCHO/doc/introduction.html {}}
    html {<h3>HTML Field</h3><p>
    This message is contained in the "html"
    field of the contents of the tree node.
    If both a "link" and an "html" field
    are included, then both will respond to
    a double click.}} Hyperlink}
{add {The second tree} {} {}}
{add {A child} {} {{The second tree}}}
