#ifndef _SOAP_RADIALBASIS_HPP
#define _SOAP_RADIALBASIS_HPP

#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include "base/exceptions.hpp"
#include "base/objectfactory.hpp"
#include "options.hpp"

namespace soap {

namespace ub = boost::numeric::ublas;

class RadialCoefficients : public ub::vector<double>
{
public:
	RadialCoefficients(int N) {
		this->resize(N);
		for (int i = 0; i < N; ++i) (*this)[i] = 0.0;
	}
	void set(int n, double c) {
		if (this->checkSize(n)) (*this)[n] = c;
		else throw soap::base::OutOfRange("RadialCoefficients::set");
	}
    double &get(int n) {
    	if (this->checkSize(n)) return (*this)[n];
    	else throw soap::base::OutOfRange("RadialCoefficients::get");
    }
    bool checkSize(int n) { return this->size() > n; }
};


class RadialBasis
{
public:
	typedef ub::matrix<double> radcoeff_t;
	typedef ub::zero_matrix<double> radcoeff_zero_t;

	std::string &identify() { return _type; }
	const int &N() { return _N; }
    virtual ~RadialBasis() {;}
    virtual void configure(Options &options);
    virtual RadialCoefficients computeCoefficients(double r);
    virtual void computeCoefficients(double r, double particle_sigma, radcoeff_t &save_here);
    virtual RadialCoefficients computeCoefficientsAllZero();

    template<class Archive>
    void serialize(Archive &arch, const unsigned int version) {
    	arch & _type;
    	arch & _N;
    	arch & _Rc;
    	arch & _integration_steps;
    	arch & _mode;
    }

protected:
   bool _is_ortho;
   std::string _type;
   int _N;
   double _Rc;
   int _integration_steps;
   std::string _mode; // <- 'equispaced' or 'adaptive'

   static constexpr double RADZERO = 1e-10;
};


struct RadialGaussian
{
	RadialGaussian(double r0, double sigma);
	RadialGaussian() {;}
	double at(double r);
	double _r0;
	double _sigma;
	double _alpha;
	// Integral S g^2 r^2 dr
	double _integral_r2_g2_dr;
	double _norm_r2_g2_dr;
	// Integral S g r^2 dr
	double _integral_r2_g_dr;
	double _norm_r2_g_dr;

    template<class Archive>
    void serialize(Archive &arch, const unsigned int version) {
    	arch & _r0;
    	arch & _sigma;
    	arch & _alpha;
    	arch & _norm_r2_g2_dr;
    	arch & _norm_r2_g_dr;
    }
};

class RadialBasisGaussian : public RadialBasis
{
public:
	typedef RadialGaussian basis_fct_t;
	typedef std::vector<RadialGaussian*> basis_t;
	typedef basis_t::iterator basis_it_t;

    RadialBasisGaussian() {
        _type = "gaussian";
        _is_ortho = false;
        _sigma = 0.5;
    }
   ~RadialBasisGaussian() {
        this->clear();
    }
    void clear() {
    	for (basis_it_t bit=_basis.begin(); bit!=_basis.end(); ++bit) {
			delete *bit;
		}
    	_basis.clear();
    }
    void configure(Options &options);
    RadialCoefficients computeCoefficients(double r);
    void computeCoefficients(double r, double particle_sigma, radcoeff_t &save_here);

    template<class Archive>
    void serialize(Archive &arch, const unsigned int version) {
    	arch & boost::serialization::base_object<RadialBasis>(*this);
    	arch & _sigma;
    	arch & _basis;
    	arch & _Sij;
    	arch & _Uij;
    	arch & _Tij;
    }

protected:
    double _sigma;
    basis_t _basis;
    // ORTHONORMALIZATION
    ub::matrix<double> _Sij; // Overlap
    ub::matrix<double> _Uij; // Cholesky factor
    ub::matrix<double> _Tij; // Transformation matrix

};








class RadialBasisLegendre : public RadialBasis
{
public:
	RadialBasisLegendre() {
		_type = "legendre";
		_is_ortho = true;
	}
};



class RadialBasisFactory 
    : public soap::base::ObjectFactory<std::string, RadialBasis>
{
private:
    RadialBasisFactory() {}
public:
    static void registerAll(void);
    RadialBasis *create(const std::string &key);
    friend RadialBasisFactory &RadialBasisOutlet();
};

inline RadialBasisFactory &RadialBasisOutlet() {
    static RadialBasisFactory _instance;
    return _instance;
}

inline RadialBasis *RadialBasisFactory::create(const std::string &key) {
    assoc_map::const_iterator it(getObjects().find(key));
    if (it != getObjects().end()) {
        RadialBasis *basis = (it->second)();
        return basis;
    } 
    else {
        throw std::runtime_error("Factory key " + key + " not found.");
    }
}

}

BOOST_CLASS_EXPORT_KEY(soap::RadialBasis);
BOOST_CLASS_EXPORT_KEY(soap::RadialBasisGaussian);

#endif
