function PtCloseStarFigures(handle)
%PtCloseStarFigures
%	PtCloseStarFigures(handle) closes all figures (images, plots,
%	etc.) associated with handle.  Ptolemy calls this function to
%	close all figures associated with a particular star.
%
%	See also PtSetStarFigures.

%
%	$Id$
%

%	Author: B. L. Evans
%	(c) Copyright 1994 by the Regents of the University of California

h = get(0, 'children');
len = length(h);
for i = 1:len
  if strcmp( get(h(i), 'UserData'), handle )
    delete(h(i));
  end
end
end
