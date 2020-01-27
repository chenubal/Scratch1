#include <iostream>
#include <vector>
#include <functional>

template<class T>
struct LambdaIterator
{
	using eval_fun = std::function<T(unsigned)>;
	LambdaIterator(eval_fun e, unsigned start = 0) : counter(start), eval(e) {}
	T operator*() const { return eval(counter); }
	LambdaIterator<T>& operator++() { ++counter; return *this; }
	LambdaIterator<T> operator++(int) { auto t = counter++; return LambdaIterator<T>(eval, t); }
	bool operator!=(LambdaIterator<T> const& o) const { return o.counter != counter; }
	virtual ~LambdaIterator<T>() = default;

protected:
	unsigned counter;
	eval_fun eval;
};


/// <summary>
/// Provides a generator for natural numbers M,M+1,...M+(N-1)
/// </summary>
struct SimpleNumbers
{
	using Iterator = LambdaIterator<unsigned>;

	SimpleNumbers(unsigned N = 0, unsigned M = 0) : N(N), M(M) {}

	Iterator begin() const { return Iterator([](unsigned k) { return k; }, M); }
	Iterator end() const { return Iterator(nullptr, M + N); }
private:
	const unsigned N, M;
};

int main(int, char**)
{
	for (auto x : SimpleNumbers(5,1))
		std::cout << x << "\n";
	return 0;

}

