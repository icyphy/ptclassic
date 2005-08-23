function PlotPats(P,D)
% PLOTPATS   Plots the training patterns defined by Patterns and Desired.
%
%            P - NxM matrix with N patterns of length M.
%		 The first two values in each pattern are used 
%		 as the coordinates of the point to be plotted.
%
%            D - NxQ matrix with N desired binary output patterns
%		 of length Q.  The first 2 bits of the output pattern
%		 determine the class of the point: o, +, *, or x.

if nargin ~=2
  error('Wrong number of arguments.');
  end

[N,M] = size(P);
Q = size(D,2);

if Q<2, D=[D,zeros(N,1)];,end

clf reset
hold on

% Calculate the bounds for the plot and cause axes to be drawn.
xmin = min(P(:,1)); xmax = max(P(:,1)); xb = (xmax-xmin)*0.2;
ymin = min(P(:,2)); ymax = max(P(:,2)); yb = (ymax-ymin)*0.2;
axis([xmin-xb, xmax+xb, ymin-yb, ymax+yb]);

title('Input Classification');
xlabel('x1'); ylabel('x2');

class = 1 + 3*D(:,1) + 6*D(:,2);
colors = [1 0 1; 1 1 0; 0 1 1; 1 0 0];
symbols = 'o+*x';

for i=1:N
  c = class(i);
  plot(P(i,1),P(i,2),symbols(c),'Color',colors(c,:));
  end
