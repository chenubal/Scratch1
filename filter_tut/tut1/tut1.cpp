#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <functional>

template<class C>
C filterC(C const& in, std::function<bool(typename C::value_type)> f)
{
	C out;
	for (auto x : in)
		if (f(x))
			out.push_back(x);
	return out;
}


template<class T>
std::vector<T> filterV(std::vector<T> const& in, std::function<bool(T)> f)
{
	std::vector<T> out;
	for (auto x : in)
		if (f(x))
			out.push_back(x);
	return out;
}

std::vector<int> filterVI(std::vector<int> const& in, std::function<bool(int)> f)
{
	std::vector<int> out;
	for (auto x : in)
		if (f(x))
			out.push_back(x);
	return out;
}

int main(int, char**)
{
	std::list<int> v = { 1,2,3,4,5,6,7,8,9 };

	auto w = filterC(v,[](int x) {return x % 2 == 0; });

	for (auto x : w) std::cout << x << ' ';
	std::cout <<  "\n";
	return 0;
}
//Problem: nur push_back container (sequential container)
