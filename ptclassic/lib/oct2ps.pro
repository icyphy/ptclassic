%
% PostScript prologue file for oct2ps
%
%   Rick L. Spickelmier
%

%
% tile a box
%
% llx lly urx ury tb
%
/tb {
/ury exch def
/urx exch def
/lly exch def
/llx exch def
% if the layer is all 1's, don't tile it, fill it
bm 256 eq
{
  0 setgray
  newpath
  llx lly moveto
  urx lly lineto
  urx ury lineto
  llx ury lineto
  closepath fill
}
{
bm 0 ne
{
gsave
/tilex { bmw llx bmw div cvi mul bmw sub } def
/tiley { bmh lly bmh div cvi mul bmh sub } def
/xcount { 1 urx tilex sub bmw div cvi add } def
/ycount { 1 ury tiley sub bmh div cvi add } def
xcount 0 lt { /xcount 1 def } if
ycount 0 lt { /ycount 1 def } if
%
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
closepath clip

/strng xcount string def

0 1 xcount 1 sub { strng exch bm put } for

ycount { tilex tiley moveto strng show 0 bmh translate } repeat

grestore
} if
} ifelse
} def
%
% tile a bitmap with out clipping (for polygons)
%
% llx lly urx ury tp
%
/tp {
/ury exch def
/urx exch def
/lly exch def
/llx exch def
% if the layer is all 1's, don't tile it, fill it
bm 256 eq
{
  0 setgray
  newpath
  llx lly moveto
  urx lly lineto
  urx ury lineto
  llx ury lineto
  closepath fill
}
{
bm 0 ne
{
gsave
/tilex { bmw llx bmw div cvi mul bmw sub } def
/tiley { bmh lly bmh div cvi mul bmh sub } def
/xcount { 1 urx tilex sub bmw div cvi add } def
/ycount { 1 ury tiley sub bmh div cvi add } def
xcount 0 lt { /xcount 1 def } if
ycount 0 lt { /ycount 1 def } if
%
/strng xcount string def

0 1 xcount 1 sub { strng exch bm put } for

ycount { tilex tiley moveto strng show 0 bmh translate } repeat

grestore
} if
} ifelse
} def
%
% formal terminal
%
% llx lly urx ury string ft
%
/ft {
/str exch def
/ury exch def
/urx exch def
/lly exch def
/llx exch def
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
closepath stroke
llx lly moveto
str show
} def
%
% actual terminal
%
% llx lly urx ury string at
%
/at {
/str exch def
/ury exch def
/urx exch def
/lly exch def
/llx exch def
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
closepath stroke
urx str stringwidth pop sub ury fscale sub moveto
str show
} def
%
% label
%
% x y string lb
%
/lb {
/str exch def
/y exch def
/x exch def
x y moveto
str show
} def
%
% outline with text
%
% llx lly urx ury string ot
%
/ot {
/str exch def
/ury exch def
/urx exch def
/lly exch def
/llx exch def
/cx llx urx add 2 div def
/cy lly ury add 2 div def
/halfstrwidth str stringwidth pop 2 div def
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
closepath 0.8 setgray fill
0 setgray newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
closepath stroke
cx halfstrwidth sub cy moveto
str show
} def
%
% box outline 
%
% llx lly urx ury bx
%
/bx {
/ury exch def
/urx exch def
/lly exch def
/llx exch def
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
llx lly lineto
closepath stroke
} def
%
% abstract connector
%
% llx lly urx ury con
%
/con {
/ury exch def
/urx exch def
/lly exch def
/llx exch def
newpath
llx lly moveto
urx lly lineto
urx ury lineto
llx ury lineto
llx lly lineto
urx ury lineto
llx ury moveto
urx lly lineto
closepath stroke
} def
%
% draw a line
%
% x1 y1 x2 y2 ln
%
/ln {
/y2 exch def
/x2 exch def
/y1 exch def
/x1 exch def
newpath
x1 y1 moveto
x2 y2 lineto
stroke
} def

/putbm {
   /arr exch def
   /str arr length string def
   0 1 arr length 1 sub { str exch dup arr exch get put } for
   str
} def
