#pragma once

namespace Geometry
{
	namespace D2
	{
		template<class T = double> struct Vector
		{
			Vector(T x = 0, T y = 0) : x(x), y(y) {}
			template<class S = T>
			explicit Vector(Vector<S> const& u) : Vector(T(u.x), T(u.y)) {}

			template<class S = T>
			Vector<T>& operator+=(Vector<S> const& u) { x += T(u.x); y += T(u.y); return *this; }

			Vector<T>& operator*=(double s) { x *= T(s); y *= T(s); return *this; }
  
			template<class S = T> // u+v
			Vector<T> operator+(Vector<S> const& v) const { return{ x + v.x, y + v.y }; }

			//s*u
			Vector<T> operator*(double s) const { return{ x*s,y*s }; }

			template<class S = T>// u*v
			double operator*(Vector<S> const& u) const { return double(x*T(u.x) + y*T(u.y)); }

			template<class S = T>
			bool operator==(Vector<S> const& u) const { return u.x == T(x) && u.y == T(y); }

			T x, y;
		};

		template<class T>	// -u
		Vector<T> operator-(Vector<T> const& u) { return{ -u.x, -u.y }; }
		template<class T>  // norm(u)
		double norm2(Vector<T> const& u) { return std::sqrt(u*u); }
		template<class T, class S>	// u!=v
		bool operator!=(Vector<T> const& u, Vector<S> const& v) { return !(u == v); }
		template<class T, class S>	// u-v
		Vector<T> operator-(Vector<T> const& u, Vector<S> const& v) { return u+(-v); }
		template<class T>	// s*u
		Vector<T> operator*(double s, Vector<T> const& u) { return u*s; }

		template<class T>
		std::ostream& operator<<(std::ostream& os, const Vector<T>& v) { os << "(" << v.x << "," << v.y << ")";	return os; }


		//-------------------------------------------------------------------------------------------------------------------------
		template<class T = double> struct Point
		{
			Point(T x = 0, T y = 0) : x(x), y(y) {}
			template<class S>
			explicit Point(Point<S> const& P) : Point(T(P.x), T(P.y)) {}
			const T x, y;

			template<class S=T>
			bool operator==(Point<S> const& P) const { return x == P.x && y == P.y; }
			template<class S=T>
			Vector<T> operator-( Point<S> const& P) const { return{ T(P.x) - x, T(P.y) - y }; }
			template<class S=T>
			Point<T> operator+(Vector<S> const& u) const { return{ x + T(u.x), y + T(u.y) }; }
		};

		template<class T, class S=T>
		bool operator!=(Point<T> const& P, Point<S> const& Q) { return !(P == Q); }
		template<class T, class S=T>
		double operator||(Point<T> const& P, Point<S> const& Q)  { return double(P - Q); }
		template<class T>
		std::ostream& operator<<(std::ostream& os, Point<T> const& P) { os << "(" << P.x << "," << P.y << ")"; return os; }

		//-------------------------------------------------------------------------------------------------------------------------
		struct Line
		{
			Line() = delete;
			template<class T = double>
			Line(Point<T> const& P, Vector<T> const& u) : P(P), u(u) { if (norm2(u) == 0) throw; }

			template<class T = double>
			Line(Point<T> const& P, Point<T> const& Q) : Line(P, P - Q) {}

			explicit Line(Line const&) = default;

			Point<double> operator() (double t) const { return P + (u*t); }

			template<class T = double>
			Point<double> projection(Point<T> const& X) const { return operator()((u * (P - X)) / (u * u)); }
			template<class T = double>
			double distance(Point<T> const& X) const { return norm2(X - projection(X)); }

			Point<double> P;
			Vector<double> u;
		};


		template<class T>
		Point<T> projection(Point<T> const& P, Vector<T> const& u, Point<T> const& X) { return Line(P, u).projection(X); }

		template<class T>
		double distance(Point<T> const& P, Vector<T> const& u, Point<T> const& X) { return Line(P, u).distance(X); }

  		template<class T>
		struct Optional
		{
			Optional(T const& value = {}, bool valid = false) : value(value), valid(valid) {}
			T value;
			bool valid = false;
			operator bool() const { return valid; }
			T operator*() const { return value; }
		};

		Optional<Point<double>> intersect(Line const& L, Line const& K)
		{
			if (L.P == K.P)	return{ L.P,true };
			auto kl = L.u * K.u;
			if (kl == 0.0)	return{ L.projection(K.P),true };
			auto l = norm2(L.u), k = norm2(K.u);
			if (std::abs(kl) == (k*l))	return{ L.projection(K.P),true };
			if (L.projection(K.P) == K.P)  return{ L.projection(K.P),true };
			return{};
		}
	}
}

namespace geo = Geometry::D2;

