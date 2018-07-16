#include <iostream>

template<class T = double>
struct vector_t
{
	vector_t(T x = 0, T y = 0) : x(x), y(y) {}
	vector_t(vector_t<T> const& u) : vector_t(u.x, u.y) {}
	T x, y;

	template<class S>
	vector_t operator+=(vector_t<S> const& u) { x += T(u.x); y += T(u.y); return *this; }
	template<class S>
	vector_t operator*=(S s) { x *= T(s); y *= T(s); return *this; }

	template<class S>
	bool operator==(vector_t<S> const& v) const { return x == T(v.x) && y == T(v.y); }
	template<class S>
	bool operator!=(vector_t<S> const& v) const { return !(*this == v); }

	template<class S> // operation U+V
	vector_t<T> operator+(vector_t<S> const& v) const { vector_t<T> w(*this); return w += v; }
	template<class S> // operation U*s
	vector_t<T> operator*(S s) const { vector_t<T> w(*this); return w *= s; }
	template< class S> // operationU*V
	double operator*(vector_t<S> const& v) const { return double(x)*double(v.x) + double(y)*double(v.y); }
};

// Syntactic sugar
template<class T, class S> // U*s
vector_t<T> operator*(S s, vector_t<T> const& v) { return v*s; }
template<class T> // -U
vector_t<T> operator-(vector_t<T> const& v) { return v*T(-1); }
template<class T, class S> // U-V
vector_t<T> operator-(vector_t<T> const& u, vector_t<S> const& v) { return u + (-v); }
template<class T> // U-V
double norm2(vector_t<T> const& v) { return std::sqrt(v*v); }

//---------------------------------------------------------------------------------------------------------------------------
template<class T = double>
struct point_t
{
	point_t(T x = 0, T y = 0) : x(x), y(y) {}
	point_t(point_t<T> const& p) : point_t(p.x, p.y) {}
	const T x, y;

	template<class S>
	vector_t<T> operator-(point_t<S> const& p) { return vector_t<T>(x - T(p.x), y - T(p.y)); }
	template<class S> // U-V
	point_t<T> operator+(vector_t<S> const& v) { return point_t<T>(x + T(v.x), y + T(v.y)); }
	template<class S>
	bool operator==(point_t<S> const& p) { return x == T(p.x) && y == T(p.y); }
	template<class S>
	bool operator!=(point_t<S> const& p) { return !(p == *this); }
};

//---------------------------------------------------------------------------------------------------------------------------

template<typename T>
std::ostream& operator<<(std::ostream& os, const point_t<T>& p) { os << "(" << p.x << ',' << p.y << ")"; return os; }

template<class T> // <<
std::ostream& operator<<(std::ostream& os, vector_t<T> const& v) { os << "<" << v.x << ',' << v.y << ">"; return os; }

int main()
{
	vector_t<> v1(3, 4);
	vector_t<> v2(v1); v2 += vector_t<float>{1.0, 1.0};
	std::cout << "V2-V1=" << (v2 - v1) << std::endl;
	point_t<> p(27, 28);
	std::cout << "P=" << (p + v1) << std::endl;
	std::cout << "V1=" << norm2(p + v1 - p) << std::endl;
}
