defstar {
	name{ Fading }
	domain{ SDF }
	desc {
This function simulates multipath effects on a Rayleigh fading channel.
	}
	author { Srikanth Gummadi }
	acknowledge { Biao Lu and Brian L. Evans }
	location { SDF DSP library }
	version { @(#)SDFFading.pl	1.1	03/17/98 }
	copyright {
Copyright (c) 1997-1998 Regents of The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        htmldoc {
A common problem faced in wireless communications systems is
due to fading caused by movement of vehicles.  We represent the
channel using a Rayleigh fading model.  This is a model which is
very accurate in the 2 MHz - 2 GHz frequency range for urban
environments which contain many obstacles.
<h3> References </h3>
<p>[1]
W. C. Jakes, <i>Microwave mobile communications</i>,
John Wiley and Sons, New York, 1996.
	}
        location { SDF main library }
        output{
            name { amp }
            type { complex }
        }
        defstate {
            name { numberOfMultipaths }
            type { int }
            default { 14 }
            desc { The number of multipaths assumed }
        }
        defstate {
            name {wavelength}
            type {float}
            default{0.15}
            desc{The wavelength of transmitted wave}
        }            
        defstate {
            name { velocity }
            type { float }
            default { 10 }
            desc { The speed of the mobile in meters/second }
        }
        defstate {
            name { samplingFrequency }
            type { float }
            default { 5000 }
            desc { The sampling frequency }
        }
        state {
            name { timeIncrement }
            type { int }
            default { 1 }
            desc { The increment when every time the star is executed }
            attributes{A_NONCONSTANT | A_NONSETTABLE}
        }
        state {
            name{ multipathAngles }
            type{ floatarray }
            default { "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0" }
            attributes { A_NONCONSTANT | A_NONSETTABLE}
        }
        hinclude{<Normal.h>}
        hinclude{<math.h>}
        ccinclude{<ACG.h>}
        protected{
            Normal *random;
        }
        code {
            extern ACG* gen;
        }
        constructor{
            random = 0;
        }
        destructor {
            LOG_DEL; delete random;
        }
        setup {
            if ( double(wavelength) <= 0.0 ) {
                StringList msg = "The wavelength should be positive";
		msg << " and not " << double(wavelength);
                Error::abortRun(*this, msg);
                return;
	    }
            if ( double(velocity) <= 0.0 ) {
                StringList msg = "The velocity should be positive";
		msg << " and not " << double(velocity);
                Error::abortRun(*this, msg);
                return;
	    }
            if ( double(samplingFrequency) <= 0.0 ) {
                StringList msg = "The samplingFrequency should be positive";
		msg << " and not " << double(samplingFrequency);
                Error::abortRun(*this, msg);
                return;
	    }
            if (int(numberOfMultipaths) <= 0 ) {
                StringList msg = "The numberOfMultipaths should be positive";
                msg << " and not " << int(numberOfMultipaths);
                Error::abortRun(*this, msg);
                return;
            }
	    multipathAngles.resize(int(numberOfMultipaths));
            LOG_DEL; delete random;
            LOG_NEW; random = new Normal(0.0,1.0,gen);
            for (int i = 0; i < int(numberOfMultipaths); i++)
                multipathAngles[i] = (*random)();
        }
        go {
            double factor = 2.0 * M_PI * double(velocity) /
	    		    (double(samplingFrequency) * double(wavelength));
            double phase = 2.0 * M_PI / double(numberOfMultipaths);
            Complex out=0.0;
            for (int i = 0; i < int(numberOfMultipaths); i++) {
                double temp = factor * double(timeIncrement)*cos(phase*i);
                out += Complex( cos(temp+multipathAngles[i]),
				sin(temp+multipathAngles[i]) );
            }
            out=out/sqrt(double(numberOfMultipaths));
            timeIncrement = int(timeIncrement) + 1;
            amp%0 << out;
        }
}
