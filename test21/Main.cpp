#include <iostream>
#include <numeric>
#include <ctime>
#include <functional>

namespace detail
{
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
}
/// Provides a sequence of natural numbers: n,n+1,n+2,...
struct SimpleNumbers
{
	using Iterator = detail::LambdaIterator<unsigned>;
	SimpleNumbers(unsigned N = 0, unsigned start = 0) : N(N), N0(start) {}

	Iterator begin() const { return Iterator([](unsigned n) { return n; }, N0); }
	Iterator end() const { return Iterator(nullptr, N0 + N); }
private:
	const unsigned N, N0;
};
/// Provides a sequence of equidistant numbers
template<class T=int>
struct Numbers 
{
	using Iterator = detail::LambdaIterator<T>;
	Numbers(unsigned N, T start, T inc = 1) : N(N), S0(start), DS(inc) { if (N < 2) DS = 0; }
	Numbers(T start, T end, unsigned N) : Numbers(N, start, (end - start) / (N - 1)) { }
	Iterator begin() const { return Iterator([=](unsigned n)->T { return  S0 + DS*n; }, 0); }
	Iterator end() const { return Iterator(nullptr, N); }
private:
	const T S0;
	T DS;
	const unsigned N;
};													    
/// Provides a sequence of random numbers from interval
template <class T = double>
struct Rands 
{
	using Iterator = detail::LambdaIterator<T>;
	Rands(unsigned N = 1, T scale = T(1)) : N(N), scale(scale) { std::srand(unsigned(std::time(0))); }
	Iterator begin() const { return Iterator([=](unsigned )->T {return T(scale * std::rand() / RAND_MAX); }, 0); }
	Iterator end() const { return Iterator(nullptr, N); }
private:
	T scale;
	const unsigned N;
};

int main()
{
	for (auto x : SimpleNumbers(3u)) 
		std::cout << "n=" << x << " ";
	std::cout << "\n";

	for (auto x : Numbers<>(7u, -4, 3)) 
		std::cout << "x=" << x << " ";
	std::cout << "\n";

	for (auto x : Numbers<double>(7.0, -4.0, 10u)) 
		std::cout << "y=" << x << " ";
	std::cout << "\n";

	for (auto x : Rands<float>(10u)) 
		std::cout << "r=" << x << " ";
	std::cout << "\n";

	SimpleNumbers h(100u);
	std::cout << "Sum1=" << std::accumulate(h.begin(), h.end(), 0.0);
	std::cout << "\n";

	Numbers<float> g(100u,1);
	std::cout << "Sum2=" << std::accumulate(g.begin(), g.end(), 0.0);
	std::cout << "\n";

}