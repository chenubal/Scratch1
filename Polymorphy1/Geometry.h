#pragma once
#include <cmath>

namespace geo
{
	template<class T> struct vector2_t
	{
		vector2_t(T x = 0, T y = 0) : x(x), y(y) {}
		template<class S = T>
		explicit vector2_t(vector2_t<S> const& u) : vector2_t(T(u.x), T(u.y)) {}

		vector2_t<T>& operator*=(double s) { x *= T(s); y *= T(s); return *this; }

		template<class S=T>
		vector2_t<T>& operator+=(vector2_t<S> const& u) { x += T(u.x); y += T(u.y); return *this; }
	
		template<class S=T>
		double operator|(vector2_t<S> const& u) const { return double(x*u.x + y*u.y); }

		operator double() const { return std::hypot(x, y); }
		T x, y;
	};
	
	template<class T=double>
	using Vector = vector2_t<T>;

	// Operators for vector
	template<class T, class S = T>
	bool operator==(Vector<T> const& u, Vector<S> const& v) { return u.x == v.x&& u.y == v.y; }
	template<class T, class S = T>
	bool operator!=(Vector<T> const& u, Vector<S> const& v) { return !(u==v); }
	template<class T>
	Vector<T> operator-(Vector<T> const& u) { return{ -u.x, -u.y}; }
	template<class T, class S=T>
	Vector<T> operator+(Vector<T> const& u, Vector<S> const& v) { auto t(u);  return t += v; }
	template<class T, class S=T>
	Vector<T> operator-(Vector<T> const& u, Vector<S> const& v) { auto t(u);  return t += -v; }
	template<class T>
	Vector<T> operator*(Vector<T> const& u, double s) { auto t(u);  return t *= s; }
	template<class T>
	Vector<T> operator*(double s, Vector<T> const& u) { return u*s; }
	template<class T>
	Vector<T> operator+(Vector<T> const& u, double s) { auto t(u);  return t += s; }
	template<class T>
	Vector<T> operator+(double s, Vector<T> const& u) { return u+s; }
	template<class T>
	std::ostream& operator<<(std::ostream& os, const Vector<T>& v) { os << "(" << v.x << "," << v.y << ")";	return os; }
	template<class T>
	double norm2(Vector<T> const& u) { return double(u); }

	//-------------------------------------------------------------------------------------------------------------------------
	template<class T = double> struct point2_t
	{
		point2_t(T x=0, T y=0) : x(x), y(y) {}
		template<class S = T>
		explicit point2_t(point2_t<S> const& P) : point2_t(T(P.x), T(P.y)) {}
		const T x, y;

		template<class S = T>
		double distance(point2_t<S> const& P = { 0,0 }) const {return std::hypot(x - T(P.x), y - T(P.y));	}
 	};

	template<class T = double>
	using Point = point2_t<T>;

	template<class T, class S = T>
	bool operator==(Point<T> const& P, Point<S> const& Q) { return P.x == Q.x&& P.y == Q.y; }
	template<class T, class S = T>
	bool operator!=(Point<T> const& P, Point<S> const& Q) { return !(P == Q); }

	template<class T, class S = T>
	Vector<T> operator-(Point<T> const& P, Point<S> const& Q) { return{ T(Q.x) - P.x, T(Q.y) - P.y }; }
	template<class T, class S = T>
	Point<T> operator+(Point<T> const& P, Vector<S> const& u){	return{ P.x + T(u.x),P.y + T(u.y) };}
 	template<class T>
	std::ostream& operator<<(std::ostream& os, Point<T> const& P) {	os << "(" << P.x << "," << P.y << ")"; return os;}

	//-------------------------------------------------------------------------------------------------------------------------
	struct line2_t
	{
		line2_t() = delete;
		template<class T = double>
		line2_t(Point<T> const& P, Vector<T> const& u) : P(P), u(u) { if (norm2(u) == 0) throw; }

		template<class T = double>
		line2_t(Point<T> const& P, Point<T> const& Q) : line2_t(P,P-Q) {}

		explicit line2_t(line2_t const&) = default;

		Point<double> operator() (double t) const { return P +(t * u); }

		template<class T = double>
		Point<double> projection(Point<T> const& X) const {return operator()((u | P - X) / (u | u));}
		template<class T = double>
		double distance(Point<T> const& X) const { return double(X - projection(X)); }

		Point<double> P;
		Vector<double> u;
	};

	using Line = line2_t;

	template<class T>
	Point<T> projection(Point<T> const& P, Vector<T> const& u, Point<T> const& X) {	return Line(P, u).projection(X);}

	template<class T>
	double distance(Point<T> const& P, Vector<T> const& u, Point<T> const& X) {return Line(P, u).distance(X); }


	template<class T>
	struct PointOpt 
	{ 
		PointOpt(T const& value = {}, bool ok=false) : value(value), ok(ok) {}
		T value; 
		bool ok = false; 
		operator bool() const { return ok; } 
		T operator*() const { return value; } 
	};

	PointOpt<Point<double>> intersect(Line const& L, Line const& K)
	{
		if (L.P == K.P)	return { L.P,true };
		auto sp = L.u | K.u;
		if (sp == 0.0)	return { L.projection(K.P),true };
		auto l = double(L.u), k = double(K.u);
		if (std::abs(sp) == (l*k))	return { L.projection(K.P),true };
		if (L.projection(K.P) == K.P)  return { L.projection(K.P),true };
		return{};
	}

}

