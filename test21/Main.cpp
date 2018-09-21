#include <iostream>
#include <numeric>
#include <ctime>
#include <functional>

template<class T>
using eval_fun = std::function<T(unsigned)>;

template<class T>
struct LambdaIterator
{
	LambdaIterator(eval_fun<T> e, unsigned start = 0) : eval(e), counter(start) {}
	T operator*() const { return eval(counter); }
	LambdaIterator<T>& operator++() { ++counter; return *this; }
	LambdaIterator<T> operator++(int) { auto t = counter++; return LambdaIterator<T>(eval, t); }
	bool operator!=(LambdaIterator<T> const& o) const { return o.counter != counter; }
	virtual ~LambdaIterator<T>() {}
protected:
	unsigned counter;
	eval_fun<T> eval;
};

/// /// Provides a sequence of natural numbers: n,n+1,n+2,...
struct SimpleNumbers
{
	SimpleNumbers(unsigned N = 0, unsigned start = 0) : N(N), N0(start) {}

	LambdaIterator<unsigned> begin() const { return LambdaIterator<unsigned>([](unsigned n) { return n; }, N0); }
	LambdaIterator<unsigned> end() const { return LambdaIterator<unsigned>(nullptr, N0 + N); }
private:
	const unsigned N, N0;
};

/// Provides a sequence of equidistant numbers
template<class T=int>
struct Numbers 
{
	Numbers(unsigned N, T start, T inc = 1) : N(N), S0(start), DS(inc) { if (N < 2) DS = 0; }
	Numbers(T start, T end, unsigned N) : Numbers(N, start, (end - start) / (N - 1)) { }
	LambdaIterator<T> begin() const { return LambdaIterator<T>([=](unsigned n)->T { return  S0 + DS*n; }, 0); }
	LambdaIterator<T> end() const { return LambdaIterator<T>(nullptr, N); }
private:
	const T S0;
	T DS;
	const unsigned N;
};
													    
/// Provides a sequence of random numbers from interval
template <class T = double>
struct Rands 
{
	Rands(unsigned N = 1, T scale = T(1)) : N(N), scale(scale) { std::srand(unsigned(std::time(0))); }
	LambdaIterator<T> begin() const { return LambdaIterator<T>([=](unsigned )->T {return T(scale * std::rand() / RAND_MAX); }, 0); }
	LambdaIterator<T> end() const { return LambdaIterator<T>(nullptr, N); }
private:
	T scale;
	const unsigned N;
};

int main()
{
	for (auto x : SimpleNumbers(3)) std::cout << "n=" << x << " ";
	std::cout << "\n";
	for (auto x : Numbers<int>(7u, -4, -1)) std::cout << "x=" << x << " ";
	std::cout << "\n";
	for (auto x : Numbers<double>(7, -4, 10u)) std::cout << "y=" << x << " ";
	std::cout << "\n";
	for (auto x : Rands<float>(10,50)) std::cout << "r=" << x << " ";

	SimpleNumbers h(101);
	std::cout << "\nSum1=" << std::accumulate(h.begin(), h.end(), 0.0);
	Numbers<float> g(100u,1);
	std::cout << "\nSum2=" << std::accumulate(g.begin(), g.end(), 0.0);

}