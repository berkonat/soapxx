#ifndef _SOAP_ANGULARBASIS_HPP
#define _SOAP_ANGULARBASIS_HPP

#include <string>
#include <math.h>
#include <vector>
#include "base/exceptions.hpp"

#include "options.hpp"

namespace soap {

namespace ub = boost::numeric::ublas;

class AngularCoefficients : public ub::vector< std::complex<double> >
{
public:
	AngularCoefficients(int L) : _L(L) {
        this->resize((L+1)*(L+1));
        for (int i=0; i!=size(); ++i) {
        	(*this)[i] = std::complex<double>(0,0);
        }
    }
	void set(int l, int m, std::complex<double> c) {
		if (this->checkSize(l, m)) (*this)[l*l+l+m] = c;
		else throw soap::base::OutOfRange("AngularCoefficients::set");
	}
	std::complex<double> &get(int l, int m) {
		if (this->checkSize(l, m)) return (*this)[l*l+l+m];
		else throw soap::base::OutOfRange("AngularCoefficients::get");
	}
	bool checkSize(int l, int m) { return (std::abs(m) <= l && l <= _L); }
	void conjugate() {
		for (int i = 0; i != size(); ++i) {
			(*this)[i] = std::conj((*this)[i]);
		}
	}
protected:
	int _L;
};


class AngularBasis
{
public:
	std::string &identify() { return _type; }
	const int &L() { return _L; }
    AngularBasis() : _type("spherical-harmonic"), _L(0) {;}
    virtual ~AngularBasis() {;}
    virtual void configure(Options &options);
    virtual AngularCoefficients computeCoefficients(vec d, double r);
    virtual AngularCoefficients computeCoefficientsAllZero();

protected:

    std::string _type;
    int _L;
};




}

#endif
