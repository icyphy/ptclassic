// $Id$
#include <Random.h>
class ACG;
// This version substitutes for libg++ random-number classes.
// Returns a random number between "lower" and "upper".

class Uniform : public Random {
private:
	double minv;
	double scale;
public:
	Uniform(double lower,double upper,ACG*) :
		minv(lower), scale(upper-lower) {}
	double operator()() {
		return unif01()*scale + minv;
	}
};

