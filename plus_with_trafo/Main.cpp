#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

template<class T>
using V = std::vector<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const V<T>& v)
{
	for (auto& e : v) {	os << e << ' ';} return os;
}

template<class T, class S = T>
V<T>& apply(V<T>& v, V<S> const& w, std::function<T(T const& ,S const&)> fun)
{
	auto b = v.begin();
	std::transform(b, b + std::min(v.size(), w.size()), w.begin(),fun);
	return v;
}

template<class T, class S=T>
V<T>& operator+=(V<T>& v, V<S> const& w)
{
	auto b = v.begin();
	std::transform(b, b + std::min(v.size(), w.size()), w.begin(), b, [](T a, S b) {return a + T(b); });
	return v;
}

template<class T, class S = T>
V<T> operator+(V<T> const& v, V<S> const& w) { auto t = v; return t += w; }

int main()
{
	V<int> v{ 3,4,5,6,7,8,9 };
	V<float> w{ 7.1f,6,5,4,3 };
	std::cout << (v + w) << std::endl;
	// prints: 10 10 10 10 10 8 9 
	std::cout << (w + v) << std::endl;
	// prints: 10 10 10 10 10 
}
