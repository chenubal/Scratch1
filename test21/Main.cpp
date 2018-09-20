#include <iostream>
#include <numeric>
#include <ctime>

/// Provides a sequence of integer numbers: n,n+1,n+2,...
struct Numbers
{
	using value_type = unsigned;
	Numbers(value_type N = 0, value_type start = 0) : N(N), N0(start) {}
	virtual ~Numbers() {}

	template<class T = value_type>
	struct Iterator
	{
		Iterator(value_type start = 0) : value(start) {}
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
	unsigned N;
	value_type N0;
};

struct Ints : public Numbers
{
	using T = int;
	Ints(Numbers::value_type N, T start, T inc = 1) : Numbers(N), Z0(start), DZ(inc) {}

	struct Iterator : public Numbers::Iterator<T>
	{
		Iterator(T N, T start, T inc) : Numbers::Iterator<T>(N), Z0(start), DZ(inc) {}
		T operator*() const { return  Z0 +DZ*Numbers::Iterator<T>::value; }
	private:
		T Z0, DZ;
	};

	Iterator begin() const { return Iterator(0, Z0, DZ); }
	Iterator end()   const { return Iterator(N, 0, 0); }
private:
	T Z0, DZ;
};


/// Provides a sequence of real (floating point) numbers: r,r+dr,r+2*dr,...  
template <class T = double>
struct Reals : public Numbers
{
	Reals(T start, T end, Numbers::value_type N = 2) : Numbers(N), X0(start), DX(end - start) { if (N > 2) DX /= N - 1; }
	struct Iterator : public Numbers::Iterator<T>
	{
		Iterator(Numbers::value_type N, T start, T delta) : Numbers::Iterator<T>(N), X0(start), DX(delta) {}
		T operator*() const { return  X0 + DX*T(Numbers::Iterator<T>::value); }
	private:
		T X0;
		T DX;
	};

	Iterator begin() const { return Iterator(0, X0, DX); }
	Iterator end()   const { return Iterator(N, X0, DX); }
private:
	T X0;
	T DX;
};

/// Provides a sequence of random numbers from interval [0,1]
template <class T = double>
struct Rands : public Numbers
{
	struct Iterator : public Numbers::Iterator<T>
	{
		Iterator(Numbers::value_type N, T scale) : Numbers::Iterator<T>(N), scale(scale) {}
		T operator*() const { return T(scale * std::rand() / RAND_MAX); }
		T scale;
	};

	Rands(Numbers::value_type N = 1, T scale= T(1)) : Numbers(N), scale(scale) { std::srand(unsigned(std::time(0))); }
	Iterator begin() const { return Iterator(0,scale); }
	Iterator end()   const { return Iterator(N,0); }
	T scale;
};

int main()
{
	for (auto x : Numbers(3)) std::cout << "n=" << x << " ";
	std::cout << "\n";
	for (auto x : Ints(7, -4, -1)) std::cout << "x=" << x << " ";
	std::cout << "\n";
	for (auto x : Reals<>(7, -4, 10)) std::cout << "x=" << x << " ";
	std::cout << "\n";
	for (auto x : Rands<int>(10,50)) std::cout << "r=" << x << " ";

	Numbers h(101);
	std::cout << "\nSum1=" << std::accumulate(h.begin(), h.end(), 0.0);
	Reals<float> g(1, 100, 100);
	std::cout << "\nSum2=" << std::accumulate(g.begin(), g.end(), 0.0);

	Numbers nats(10,7);
	auto i = nats.begin();
	for (auto j : Numbers(10))
	{
		std::cout << "\n" << *(i++);

	}

}