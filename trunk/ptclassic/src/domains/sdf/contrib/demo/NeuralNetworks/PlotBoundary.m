function PlotBoundary(W,iter,style)
% PlotBoundary   Plot classification boundary based on weight matrix W.

NUNITS = size(W,1);

colors = get(gca,'ColorOrder');
ncolors = size(colors,1);
c1 = [1 0 0; 1 1 0; 0 1 1; 1 0 1; 0 1 0; 0 0 1];
c2 = c1([2:size(c1,1),1],:);
colorsteps = 3;
cb = floor((iter-1)/colorsteps);
ci = 1+rem(cb,size(c1,1));
cj = 1+rem(cb+1,size(c1,1));
ck = rem(iter-1,colorsteps)/colorsteps;


temp = axis;
xrange = temp(1:2);

for i = 1:NUNITS
 if NUNITS > 1
     color = colors(1+rem(i-1,ncolors),:);
   elseif size(style)==1
     color = [1 1 1];
   else
    color = (1-ck)*c1(ci,:) + ck*c2(ci,:);
   end
 plot(xrange,(-W(i,2)*xrange-W(i,1))/W(i,3),'LineStyle',style,'Color',color);
 end
