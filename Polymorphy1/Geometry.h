#pragma once

namespace geometry
{
	namespace D2
	{
		template<class T = double> struct Vector
		{
			Vector(T x = 0, T y = 0) : x(x), y(y) {}
			template<class S = T>
			explicit Vector(Vector<S> const& u) : Vector(T(u.x), T(u.y)) {}

			Vector<T>& operator*=(double s) { x *= T(s); y *= T(s); return *this; }

			template<class S = T>
			Vector<T>& operator+=(Vector<S> const& u) { x += T(u.x); y += T(u.y); return *this; }

			template<class S = T>
			double operator*(Vector<S> const& u) const { return double(x*T(u.x) + y*T(u.y)); }

			template<class S = T>
			bool operator==(Vector<S> const& u) const { return u.x == T(x) && u.y == T(y); }

			Vector<T> operator-() const { return { -x, -y }; }

			operator double() const { return std::sqrt(*this *  *this); }
			T x, y;
		};

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
		std::ostream& operator<<(std::ostream& os, const Vector<T>& v) { os << "(" << v.x << "," << v.y << ")";	return os; }
		template<class T>
		double norm2(Vector<T> const& u) { return double(u); }

		//-------------------------------------------------------------------------------------------------------------------------
		template<class T = double> struct Point
		{
			Point(T x = 0, T y = 0) : x(x), y(y) {}
			template<class S = T>
			explicit Point(Point<S> const& P) : Point(T(P.x), T(P.y)) {}
			const T x, y;

			template<class S>
			bool operator==(Point<S> const& P) const { return x == P.x && y == P.y; }
		};

		template<class T, class S = T>
		bool operator!=(Point<T> const& P, Point<S> const& Q) { return !(P == Q); }
 		template<class T, class S = T>
		Vector<T> operator-(Point<T> const& P, Point<S> const& Q) { return{ T(Q.x) - P.x, T(Q.y) - P.y }; }
		template<class T, class S = T>
		Point<T> operator+(Point<T> const& P, Vector<S> const& u) { return{ P.x + T(u.x),P.y + T(u.y) }; }
		template<class T, class S = T>
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

			Point<double> operator() (double t) const { return P + (t * u); }

			template<class T = double>
			Point<double> projection(Point<T> const& X) const { return operator()((u * (P - X)) / (u * u)); }
			template<class T = double>
			double distance(Point<T> const& X) const { return double(X - projection(X)); }

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

namespace geo = geometry::D2;

