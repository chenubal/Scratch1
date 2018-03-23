#pragma once
#include <cmath>

namespace geometry
{
	namespace D3
	{
		template<class T> struct Vector
		{
			Vector(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
			template<class S = T>
			explicit Vector(Vector<S> const& u) : Vector(T(u.x), T(u.y), T(u.z)) {};

			Vector<T>& operator*=(double s) { x *= T(s); y *= T(s); z *= T(s); return *this; }

			template<class S = T>
			Vector<T>& operator+=(Vector<S> const& u) { x += T(u.x); y += T(u.y); z += T(u.z); return *this; }

			template<class S = T>
			bool operator==( Vector<S> const& u) { return x == u.x && y == u.z && y == u.z; }

			template<class S = T>
			double operator|(Vector<S> const& u) const { return double(x*u.x + y*u.y + z*u.z); }

			template<class T>
			Vector<T> operator-() { return{ -x, -y, -z }; }

			operator double() const { return std::sqrt(x*x + y*y + z*z); }
			T x, y, z;
		};

		// Operators for vector
		template<class T, class S = T>
		bool operator!=(Vector<T> const& u, Vector<S> const& v) { return !(u == v); }
		template<class T, class S = T>
		Vector<T> operator+(Vector<T> const& u, Vector<S> const& v) { auto t(u);  return t += v; }
		template<class T, class S = T>
		Vector<T> operator-(Vector<T> const& u, Vector<S> const& v) { auto t(u);  return t += -v; }
		template<class T>
		Vector<T> operator*(Vector<T> const& u, double s) { auto t(u);  return t *= s; }
		template<class T>
		Vector<T> operator*(double s, Vector<T> const& u) { return u*s; }
		template<class T>
		Vector<T> operator+(Vector<T> const& u, double s) { auto t(u);  return t += s; }
		template<class T>
		Vector<T> operator+(double s, Vector<T> const& u) { return u + s; }
		template<class T>
		std::ostream& operator<<(std::ostream& os, const Vector<T>& v) { os << "(" << v.x << "," << v.y << "," << v.z << ")"; return os; }
		template<class T>
		double norm2(Vector<T> const& u) { return double(u); }

		//-------------------------------------------------------------------------------------------------------------------------
		template<class T = double> struct Point
		{
			Point(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
			template<class S = T>
			explicit Point(Point<S> const& P) : Point(T(P.x), T(P.y)) {}
			const T x, y, z;
			template<class S = T>
			bool operator==(Point<T> const& P) { return P.x == x. x && P.y == y && P.z==z; }
		};

		template<class T, class S = T>
		bool operator!=(Point<T> const& P, Point<S> const& Q) { return !(P == Q); }
		template<class T, class S = T>
		Vector<T> operator-(Point<T> const& P, Point<S> const& Q) { return{ T(Q.x) - P.x, T(Q.y) - P.y, T(Q.z) - P.z }; }
		template<class T, class S = T>
		Point<T> operator+(Point<T> const& P, Vector<S> const& u) { return{ P.x + T(u.x),P.y + T(u.y), P.z + T(u.z) }; }
		template<class T, class S = T>
		double distance(Point<T> const& P, Point<S> const& Q = { 0,0 }) const { return double(P-Q); }
		template<class T>
		std::ostream& operator<<(std::ostream& os, Point<T> const& P) { os << "(" << P.x << "," << P.y << "," << P.z << ")"; return os; }

		//-------------------------------------------------------------------------------------------------------------------------
		struct math_line_t
		{
			math_line_t() = delete;
			template<class T = double>
			math_line_t(Point<T> const& P, Vector<T> const& u) : P(P), u(u) { if (norm2(u) == 0) throw; }

			template<class T = double>
			math_line_t(Point<T> const& P, Point<T> const& Q) : math_line_t(P, P - Q) {}

			explicit math_line_t(math_line_t const&) = default;

			Point<double> operator() (double t) const { return P + (t * u); }

			template<class T = double>
			Point<double> projection(Point<T> const& X) const { return operator()((u | P - X) / (u | u)); }
			template<class T = double>
			double distance(Point<T> const& X) const { return double(X - projection(X)); }

			Point<double> P;
			Vector<double> u;
		};

		using Line = math_line_t;

		template<class T>
		Point<T> projection(Point<T> const& P, Vector<T> const& u, Point<T> const& X) { return Line(P, u).projection(X); }

		template<class T>
		double distance(Point<T> const& P, Vector<T> const& u, Point<T> const& X) { return Line(P, u).distance(X); }


		template<class T>
		struct PointOpt
		{
			PointOpt(T const& value = {}, bool valid = false) : value(value), valid(valid) {}
			T value;
			bool valid = false;
			operator bool() const { return valid; }
			T operator*() const { return value; }
		};

		PointOpt<Point<double>> intersect(Line const& L, Line const& K)
		{
			if (L.P == K.P)	return{ L.P,true };
			auto sp = L.u | K.u;
			if (sp == 0.0)	return{ L.projection(K.P),true };
			auto l = double(L.u), k = double(K.u);
			if (std::abs(sp) == (l*k))	return{ L.projection(K.P),true };
			if (L.projection(K.P) == K.P)  return{ L.projection(K.P),true };
			return{};
		}
	}
}

namespace geo3 = geometry::D3;

