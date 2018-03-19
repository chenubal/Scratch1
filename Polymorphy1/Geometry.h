#pragma once
#include <cmath>

namespace geo
{
	template<class T = double> struct Vector
	{
		Vector(T x=0, T y=0) : x(x), y(y) {}
		Vector(Vector<T> const&) = default;

		Vector<T>& operator*=(double s) { x *= T(s); y *= T(s); return *this; }
		Vector<T>& operator+=(double s) { x *= T(s); y += T(s); return *this; }

		template<class S=T>
		Vector<T>& operator+=(Vector<S> const& o) { x += T(o.x); y += T(o.y); return *this; }
		template<class S=T>
		Vector<T>& operator*=(Vector<S> const& o) { x *= T(o.x); y *= T(o.y); return *this; }
	
		T x, y;
	};

	template<class T>
	Vector<T> operator-(Vector<T> const& l) { return{ -l.x, -l.y}; }
	template<class T, class S=T>
	Vector<T> operator+(Vector<T> const& l, Vector<S> const& r) { auto t(l);  return t += r; }
	template<class T, class S=T>
	Vector<T> operator-(Vector<T> const& l, Vector<S> const& r) { auto t(l);  return t += -r; }
	template<class T>
	Vector<T> operator*(Vector<T> const& l, double s) { auto t(l);  return t *= s; }
	template<class T>
	Vector<T> operator*(double s, Vector<T> const& l) { return l*s; }
	template<class T>
	Vector<T> operator+(Vector<T> const& l, double s) { auto t(l);  return t += s; }
	template<class T>
	Vector<T> operator+(double s, Vector<T> const& l) { return l+s; }

	template<class T, class S = T>
	double operator|(Vector<T> const& l, Vector<S> const& r) { return double(l.x*r.x + l.y*r.y); }
	template<class T>
	double norm2(Vector<T> const& l) { return std::sqrt(l|l); }

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Vector<T>& v)
	{
		os << "(" << v.x << "," << v.y << ")";
		return os;
	}

	template<class T = double> struct Point
	{
		Point() = delete;
		Point(T x, T y) : x(x), y(y) {}
		Point(Point const&) = default;
		const T x, y;

		template<class S = T>
		double distance(Point<S> const& P = { 0,0 }) const
		{
			return double(std::hypot(x - T(P.x), y - T(P.y)));
		}
		Point<T> shift( Vector<T> const& u)
		{
			return{ x + u.x,y + u.y };
		}
	};

		
	template<class T>
	std::ostream& operator<<(std::ostream& os, Point<T> const& P)
	{
		os << "(" << P.x << "," << P.y << ")";
		return os;
	}


}

