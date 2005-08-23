defstar {
        name { Sigmoid }
        domain { CGC }
        desc {
Compute the Sigmoid function, defined as 1/(1 + exp(-r*input)), where
r is the learning rate.  The Sigmoid function is commonly used in
neural networks.
        }
        author { Biao Lu }
        acknowledge { Brian L. Evans }
        location { CGC contrib library }
        version { @(#)CGCSigmoid.pl	1.2	03/24/98 }
        copyright {
Copyright (c) 1997 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        htmldoc {
A common model of an artificial neuron is a weighted sum of inputs
followed by an activation function.  The activation function, which
is often nonlinear, transforms the net input to a neuron into its
activation (level of activity) [1].  The Sigmoid function is one of
the most commonly used activation functions in artificial neural
networks.
<p>
The input to the Sigmoid function can be any number, positive or negative,
and outputs a number from 0 and 1.  Hence, the Sigmoid function does not
suffer from numerical overflow.
<p>
REFERENCES
<ol>
<li>L. Fausett, <em>Fundamentals of Neural Networks</em>, Prentice Hall,
Englewood Cliffs, NJ, ISBN 0-13-334186-0, 1994.
</ol>
        }
        input {
                name { input }
                type { float }
        }
        output {
                name { output }
                type { float }
        }
        state {
            name { learning_rate }
            type { float }
            default { 0.5 }
            desc {
The learning rate must be positive.  Once the network is well-trained,
the learning rate is fixed.  If you scale the weights of the neurons
in a trained neural network, then you multiply the learning rate by the
same factor.  Scaling is important, for example, to guarantee that all
of the intermediate computations remain in the range [-1,1).
            }
        }

        constructor {
                noInternalState();
        }
        initCode {
                addInclude("<math.h>");
        }
        go {
                addCode(exp);
        }
   codeblock(exp) {
        double rate = $val(learning_rate);
        $ref(output) = 1.0/(1.0 + exp(- rate * $ref(input)));
   }
        exectime {
                return 33;  /* use the same value as exp */
        }
}
