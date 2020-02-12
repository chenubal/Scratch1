#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

namespace geo
{
	template <class T = double>
	struct vector final
	{
		// C'tors
		vector(T x, T y) : x(x), y(y) {}
		template<class S = T>
		vector(vector<S> const& v) : vector(T(v.x), T(v.y)) {}

		// data
		const T x;
		const T y;

		// vector space arithmetic
		vector<T> operator*(double s) { return { x*T(s), y*T(s) }; }
		template <class S>
		auto operator+(vector<S> w)->vector<decltype(x + w.x)> { return { x + w.x, y + w.y }; }
	};

	// Scalar product <v|w>
	template <class T1, class T2>
	auto operator*(vector<T1> v, vector<T2> w)->decltype(v.x*w.x) { return { v.x*w.x + v.y*w.y }; }
	// 2-norm sqrt(<v|v>)
	template <class T>
	auto norm(vector<T> v)->decltype(v.x) { return std::sqrt(v*v); }
	// Left multiplication s*v
	template <class T>
	vector<T> operator*(double s, vector<T> v) { return v * s; }
	// Printing
	template<class T>
	std::ostream& operator<<(std::ostream& oss, vector<T> const& v)
	{
		oss << "<" << v.x << "," << v.y << ">";
		return oss;
	}

	template <class T = double>
	struct point final
	{
		// C'tors
		point(T x, T y) : x(x), y(y) {}
		template<class S = T>
		point(vector<S> const& p) : point(T(p.x), T(p.y)) {}
		// data
		const T x;
		const T y;
		// operation Q = P+v
		template <class S>
		auto operator+(vector<S> w)->point<decltype(x + w.x)> { return { x + w.x, y + w.y }; }
		// operation v = P-Q
		template <class S>
		auto operator-(point<S> p)->vector<decltype(x - p.x)> { return { x - p.x, y - p.y }; }
	};

	template<class T>
	std::ostream& operator<<(std::ostream& oss, point<T> const& p)
	{
		oss << "(" << p.x << "," << p.y << ")";
		return oss;
	}
}

int main()
{
	geo::vector<double> v(3.0, 4.0);
	auto w = v;
	geo::vector<float> w2(7.1f, 8.2f);
	std::cout << ((2.2*w + (v + w2) * 5)).x << "\n";
	std::cout << v * v << "\n";
	std::cout << norm(v) << "\n";
	std::cout << v << "\n";
	geo::point<> p(10.0, 20.0), q(30.0, 40.0);
	std::cout << p + (q - p) << "\n";
	std::cout << q + (p - q) << "\n";
	std::cout << (p + v) - p << "\n";
	std::cout << (p + v) << "\n";
	auto q2 = q;
	std::cout << q2 << "\n";
	return 0;
}