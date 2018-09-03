#include <iostream>
#include <string>
#include <vector>
#include <numeric>

template<class T>
struct SuperIt
{
	SuperIt(T current = 0, T inc = 1) : current(current), inc(inc) {}
	T current, inc;
	SuperIt& operator++() { current += inc; return *this; }
	operator T() { return current; }
};

template<class T>
SuperIt<T> makeSuperIt(T current, T inc) { return SuperIt<T>(current, inc); }

int main()
{
	std::vector<int> V(20);
	std::iota(V.begin(), V.end(), makeSuperIt(100, 3));
	for (auto v : V) std::cout << int(v) << "\n";

}
