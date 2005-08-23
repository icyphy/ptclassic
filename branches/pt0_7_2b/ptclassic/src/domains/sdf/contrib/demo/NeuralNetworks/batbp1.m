function [W1,W2,PI_vector,iter,Y2]=batbp1(NetDef,W1,W2,PHI,Y,trparms)
%Biao changed on 4/11/1997. Add Y2 to output the curve after learning
%
%  BATBP
%  -----
%           Batch version of the back-propagation algorithm.
%
%  Given a set of corresponding input-output pairs and an initial network
%  [W1,W2,critvec,iter]=batbp(NetDef,W1,W2,PHI,Y,trparms) trains the
%  network with backpropagation.
%
%  The activation functions must be either linear or tanh. The network
%  architecture is defined by the matrix 'NetDef' consisting of two
%  rows. The first row specifies the hidden layer while the second
%  specifies the output layer.
%
%  E.g.,    NetDef = ['LHHHH'
%                     'LL---']
%
%  (L = Linear, H = tanh)
% 
%  Notice that the bias is included as the last column in the weight
%  matrices.
%
%  See alternatively INCBP for an incremental version.
% 
%  INPUT:
%  NetDef: Network definition 
%  W1    : Input-to-hidden layer weights. The matrix dimension is
%          dim(W1) = [(# of hidden units) * (inputs + 1)] (the 1 is due to the bias)
%  W2    : hidden-to-output layer weights.
%           dim(W2) = [(outputs)  *  (# of hidden units + 1)]
%  PHI   : Input vector. dim(PHI) = [(inputs)  *  (# of data)]
%  Y     : Output data. dim(Y) = [(outputs)  * (# of data)]
%  trparms : Vector containing parameters associated with the training 
%             trparms = [max_iter eta alpha]
%             max_iter  : Max. number of iterations
%             stop_crit : Stop learning if criterion is below this value
%             eta       : Step size
%             alpha     : Momentum (default is 0 (=off) )
%
%  OUTPUT: 
%  W1, W2   : Weight matrices after training
%  critvec  : Vector containing the criterion evaluated at each iteration.
%  iter     : # of iterations
%  
%  Programmed by : Magnus Norgaard, IAU/IMM, DTU
%  LastEditDate  : July 16, 1996

 
%----------------------------------------------------------------------------------
%--------------             NETWORK INITIALIZATIONS                   -------------
%----------------------------------------------------------------------------------
max_iter = trparms(1);
stop_crit = trparms(2);
eta      = trparms(3);
if length(trparms)==4,
  alpha    = trparms(4);
else
  alpha = 0;
end
[outputs,N] = size(Y);                % # of outputs and # of data
[layers,dummy] = size(NetDef);        % Number of hidden layers
L_hidden = find(NetDef(1,:)=='L')';   % Location of linear hidden units
H_hidden = find(NetDef(1,:)=='H')';   % Location of tanh hidden units
L_output = find(NetDef(2,:)=='L')';   % Location of linear output units
H_output = find(NetDef(2,:)=='H')';   % Location of tanh output units
[hidden,net_inputs] = size(W1);       % # of hidden units 
inputs=inputs-1;                      % # of inputs
PI_vector  = zeros(max_iter,1);       % Vector containing the SSE for each iteration
y1 = zeros(hidden,N);                 % Hidden layer outputs
aug_y1=[y1;ones(1,N)];
delta1 = y1;
y2 = zeros(outputs,N);                % Network output
delta2 = y2;
Y_train = zeros(size(Y));
PHI = [PHI;ones(1,N)];                % Augment PHI with a row containg ones
dW1_old = 0*W1;                       % Weights from previous iteration
dW2_old = 0*W2;                       % Weights from previous iteration


%---------------------------------------------------------------------------------
%-------------                   TRAIN NETWORK                       -------------
%---------------------------------------------------------------------------------
clc;
c = fix(clock);
fprintf('Network training started at %2i.%2i.%2i\n\n',c(4),c(5),c(6));
for iter=1:max_iter,

% >>>>>>>>>>>>>>    Compute network output (Presentation phase)    <<<<<<<<<<<<<<< 
    h1 = W1*PHI;  
    y1(H_hidden,:) = pmntanh(h1(H_hidden,:));
    y1(L_hidden,:) = h1(L_hidden,:);
    
    aug_y1(1:hidden,1:N)=y1;
    h2 = W2*aug_y1;
    y2(H_output,:) = pmntanh(h2(H_output,:));
    y2(L_output,:) = h2(L_output,:);

    E            = Y - y2;                     % Training error
    SSE          = sum(sum(E.*E));             % Sum of squared errors (SSE)
    Y2 = y2;


% >>>>>>>>>>>>>>>>>>>>>    Backpropagation of the error     <<<<<<<<<<<<<<<<<<<<<< 
    % Delta for output layer
    delta2(H_output,:) = (1-y2(H_output,:).*y2(H_output,:)).*E(H_output,:);
    delta2(L_output,:) = E(L_output,:);
  
    % delta for hidden layer
    E1 = W2(:,1:hidden)'*delta2; 
    delta1(H_hidden,:) = (1-y1(H_hidden,:).*y1(H_hidden,:)).*E1(H_hidden,:);
    delta1(L_hidden,:) = E1(L_hidden,:);


    
% >>>>>>>>>>>>>>>>>>>>>>>>        Update weights         <<<<<<<<<<<<<<<<<<<<<<<<<
    % -- Update weights without momentum --
    if (alpha==0),
      W2 = W2 + eta*delta2*aug_y1'; % Update weights between hidden and ouput
      W1 = W1 + eta*delta1*PHI';    % Update weights between input and hidden
    else
    
    % -- Update weights with momentum --
      dW2 = alpha*dW2_old + (1-alpha)*eta*delta2*aug_y1';
      W2  = W2 + dW2;
      dW1 = alpha*dW1_old + (1-alpha)*eta*delta1*PHI';
      W1  = W1 + dW1;
      dW2_old = dW2;
      dW1_old = dW1;
    end

    
%>>>>>>>>>>>>>>>>>>>>        UPDATES FOR NEXT ITERATION        <<<<<<<<<<<<<<<<<<<
  PI = SSE/(2*N);
  PI_vector(iter)=PI;
  fprintf('iteration # %i   PI = %f\r',iter,PI)  % Print on-line inform
    if PI < stop_crit, break, end         % Check if stop condition is fulfilled
end
%---------------------------------------------------------------------------------
%-------------              END OF NETWORK TRAINING                  -------------
%---------------------------------------------------------------------------------
PI_vector = PI_vector(1:iter);
c = fix(clock);
fprintf('\n\nNetwork training ended at %2i.%2i.%2i\n',c(4),c(5),c(6));






