#include <iostream>
#include <numeric>
#include <ctime>

/// Provides a sequence of natural numbers: n,n+1,n+2,...
struct SimpleNumbers
{
	using value_type = unsigned;
	SimpleNumbers(value_type N = 0, value_type start = 0) : N(N), N0(start) {}
	virtual ~SimpleNumbers() {}

	template<class T = value_type>
	struct Iterator
	{
		explicit Iterator(value_type start = 0) : value(start) {}
		T operator*() const { return value; }
		Iterator<T> operator++() { ++value; return *this; }
		Iterator<T> operator++(int) {auto t = value++; return Iterator<T>(t); }
		bool operator!=(Iterator<T> const& o) const { return o.value != value; }
		virtual ~Iterator<T>() {}
	protected:
		value_type value;
	};
	Iterator<> begin() const { return Iterator<>(N0); }
	Iterator<> end() const { return Iterator<>(N0 + N); }

protected:
	const unsigned N;
	const value_type N0;
};

/// Provides a sequence of equidistant numbers
template<class T=int>
struct Sequence : public SimpleNumbers
{
	Sequence(SimpleNumbers::value_type N, T start, T inc = 1) : SimpleNumbers(N), S0(start), DS(inc) { if (N < 2) DS = 0; }
	Sequence(T start, T end, SimpleNumbers::value_type N) : Sequence(N, start, (end - start) / (N - 1)) { }

	struct Iterator : public SimpleNumbers::Iterator<T>
	{
		Iterator(SimpleNumbers::value_type N, T start, T inc) : SimpleNumbers::Iterator<T>(N), s0(start), ds(inc) {}
		T operator*() const { return  s0 + ds*T(SimpleNumbers::Iterator<T>::value); }
	private:
		const T s0, ds;
	};

	Iterator begin() const { return Iterator(0, S0, DS); }
	Iterator end()   const { return Iterator(N, 0, 0); }
private:
	const T S0;
	T DS;
};
													    

/// Provides a sequence of random numbers from interval
template <class T = double>
struct Rands : public SimpleNumbers
{
	Rands(SimpleNumbers::value_type N = 1, T scale = T(1)) : SimpleNumbers(N), scale(scale) { std::srand(unsigned(std::time(0))); }
	struct Iterator : public SimpleNumbers::Iterator<T>
	{
		Iterator(SimpleNumbers::value_type N, T scale) : SimpleNumbers::Iterator<T>(N), scale(scale) {}
		T operator*() const { return T(scale * std::rand() / RAND_MAX); }
		T scale;
	};

	Iterator begin() const { return Iterator(0,scale); }
	Iterator end()   const { return Iterator(N,0); }
	T scale;
};

int main()
{
	for (auto x : SimpleNumbers(3)) std::cout << "n=" << x << " ";
	std::cout << "\n";
	for (auto x : Sequence<int>(7u, -4, -1)) std::cout << "x=" << x << " ";
	std::cout << "\n";
	for (auto x : Sequence<double>(7, -4, 10u)) std::cout << "y=" << x << " ";
	std::cout << "\n";
	for (auto x : Rands<int>(10,50)) std::cout << "r=" << x << " ";

	SimpleNumbers h(101);
	std::cout << "\nSum1=" << std::accumulate(h.begin(), h.end(), 0.0);
	Sequence<float> g(100u,1);
	std::cout << "\nSum2=" << std::accumulate(g.begin(), g.end(), 0.0);

	SimpleNumbers nats(10,7);
	auto i = nats.begin();
	for (auto j : SimpleNumbers(10))
	{
		std::cout << "\n" << *(i++);

	}

}