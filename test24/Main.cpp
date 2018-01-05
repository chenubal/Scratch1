#include <algorithm>
#include <iostream>


namespace jh
{
	template <typename T>
	T clamp(const T& x, const T& l, const T& u) 
	{
		return std::max(l, std::min(x, u));
	}

	template<class T = double>
	struct Interval
	{
		Interval() = delete;
		Interval(T x, T y) : lower(std::min(x, y)), upper(std::max(x, y)) {}
		template<class U>
		Interval(Interval<U> const& other) : Interval(T(other.lower), T(other.upper)) {}

		bool empty() const { return upper == lower; }
		T width() const { return upper - lower; }

		bool contains(T x) const { return x >= lower && x <= upper; }
		T clamp(T x) const { return jh::clamp(x, lower, upper); }

		template<class U>
		bool intersects(Interval<U> const& other) { return !(*this | other).empty(); }

		const T lower, upper;
	};

	// Intersection:	A and B
	template<class T, class U>
	Interval<T> operator|(Interval<T> const& i1, Interval<U> const& i2)
	{
		auto l = std::max<T>(i1.lower, i2.lower), u = std::min<T>(i1.upper, i2.upper);
		return (l < u ? Interval<T>(l, u) : Interval<T>(T(0), T(0)));
	}
	// Hull 
	template<class T, class U>
	Interval<T> operator&(Interval<T> const& i1, Interval<U> const& i2)
	{
		auto l = std::min<T>(i1.lower, i2.lower), u = std::max<T>(i1.upper, i2.upper);
		return Interval<T>(l, u) ;
	}

	// Difference: A and !B
	template<class T, class U>
	Interval<T> operator/(Interval<T> const& i1, Interval<U> const& i2)
	{
		auto d = i1 | i2;
		if (d.empty())
			return i1;
		else if (d.contains(i1.lower)) 
			return Interval<T>(d.upper, i1.upper);
		else
			return Interval<T>(i1.lower, d.lower);
 	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Interval<T>& i)
	{
		os << "R[" << i.lower << "," << i.upper << "]";
		return os;
	}
}
		 
int main()
{
	using jh::Interval;

	Interval<> r{ 7.55, 5.1 };
	auto r2 = r;

	Interval<int> r3(r2);

	Interval<> r4(5.1, 5.1);
	std::cout << r4.empty() << std::endl;
	std::cout << r.contains(7.0) << std::endl;
	std::cout << r2.clamp(-1.0) << std::endl;
	std::cout << Interval<>(7.55, 5.1).clamp(6.0) << std::endl;
	std::cout << r3.clamp(100) << std::endl;
	std::cout << r3 << std::endl;
	std::cout << Interval<>(0, 2).intersects(Interval<int>(1, 4)) << std::endl;
	std::cout << (Interval<>(0, 2) | Interval<int>(1, 4)) << std::endl;
	std::cout << (Interval<>(0, 2) & Interval<int>(3, 4)) << std::endl;
	std::cout << (Interval<>(0,7) / Interval<int>(3,8)) << std::endl;
}
