#include <iostream>
#include <vector>

namespace
{
	using vect_t = std::vector<double>;

	template<class I>
	double horner(I begin, I end, double x)
	{
		if (begin == end)
			return 0.0;
		else
			return *begin + x * horner(++begin, end, x);
	}
	template<class C>
	double horner(C const c, double x)
	{
		return horner(c.rbegin(), c.rend(), x);
	}

	// Polynomial function with component vector a={a0,a1,a2,a3, ..} => P(a;x) = a0+a1*x+a2*x²+a3*x³+...
	// Evaluation with Horner method
	struct Polynomial
	{
		Polynomial(vect_t const& c = {}) : coeffs(c) {}
		Polynomial(vect_t && c) : coeffs(std::move(c)) {}
		double operator()(double x) { return horner(coeffs, x); }
		size_t degree() const { auto n = coeffs.size();  return (n > 0 ? n - 1 : 0); }
	private:
		vect_t coeffs;
	};
}

int main()
{
	{
		Polynomial p; // P(x)=0
		std::cout << p(1) << std::endl;
	}
	{
		Polynomial p({ 5 }); // P(x)=5
		std::cout << p(1) << std::endl;
	}
	{
		Polynomial p({ 4,5 }); // P(x) = 4x+5
		std::cout << p(1) << std::endl; 
	}
	{
		Polynomial p({3, 4,5 }); // P(x) = 3x²+4x+5
		std::cout << p(1) << std::endl;
		std::cout << p(2) << std::endl;
	}
	{
		Polynomial p({ 0,0,3, 4,5 }); // P(x) = 3x²+4x+5
		std::cout << p(1) << std::endl;
		std::cout << p(2) << std::endl;
	}
	{
		Polynomial p({3,4,5,0,0 }); // P(x) = 3(x^4)+4x³+5x²
		std::cout << p(1) << std::endl;
		auto q = p;
		std::cout << q(2) << " deg(Q)=" << q.degree() <<std::endl;
	}
}
