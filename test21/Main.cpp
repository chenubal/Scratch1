#include <iostream>
#include <numeric>
#include <ctime>

/// Provides a sequence of integer numbers: n,n+1,n+2,...
struct Integers
{
	template<class R>
	struct Iterator
	{
		Iterator(R c = 0) : count(c) {}
		R operator*() const { return count; }
		Iterator<R> operator++() { return Iterator<R>(++count); }
		bool operator!=(Iterator<R> const& o) const { return o.count != count; }
		virtual ~Iterator<R>() {}
	protected:
		R count;
	};
	Integers(uint32_t n = 0, uint32_t s = 0) : size(n), start(s) {}
	virtual ~Integers() {}
	Iterator<uint32_t> begin() const { return Iterator<uint32_t>(start); }
	Iterator<uint32_t> end() const { return Iterator<uint32_t>(start + size); }

protected:
	uint32_t size, start;
};

/// Provides a sequence of real (floating point) numbers: r,r+dr,r+2*dr,...  
template <class T = double>
struct Reals : public Integers
{
	using InitIt = Integers::Iterator<T>;
	struct Iterator : public InitIt
	{
		Iterator(T n, T s, T d) : InitIt(n), delta(d), start(s) {}
		T operator*() const { return InitIt::count*delta + start; }
	private:
		T delta, start;
	};

	Reals(T s, T e, uint32_t n = 2) : Integers(n, 0), start(s), delta((e - s)) { if (size > 2) delta /= size - 1; }
	Iterator begin() const { return Iterator(T(0), start, delta); }
	Iterator end()   const { return Iterator(T(size), start, delta); }
private:
	T start, delta;
};

/// Provides a sequence of random numbers from interval [0,1]
template <class T = double>
struct Rands : public Integers
{
	struct Iterator : public Integers::Iterator<double>
	{
		Iterator(T n) : Integers::Iterator<T>(n) {}
		double operator*() const { return T(std::rand()) / RAND_MAX; }
	};

	Rands(uint32_t n = 1) : Integers(n) { std::srand(unsigned(std::time(0))); }
	Iterator begin() const { return Iterator(0); }
	Iterator end()   const { return Iterator(size); }
};

int main()
{
	for (auto x : Integers(3)) std::cout << "n=" << x << " ";
	std::cout << "\n";
	for (auto x : Reals<>(7, -10, 18)) std::cout << "x=" << x << " ";
	std::cout << "\n";
	for (auto x : Rands<>(10)) std::cout << "r=" << x << " ";

	Integers h(101);
	std::cout << "\nSum1=" << std::accumulate(h.begin(), h.end(), 0.0);
	Reals<float> g(1, 100, 100);
	std::cout << "\nSum2=" << std::accumulate(g.begin(), g.end(), 0.0);
}