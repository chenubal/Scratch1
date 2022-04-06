#include <algorithm>
#include <list>
#include <vector>
#include <iostream>
#include <numeric>
#include <random>
#include <functional>
#include <algorithm>
#include <optional>

auto print = [](auto const rem,  auto const& v) 
{
	std::cout << rem;
	for (auto const& e : v)
		std::cout << e << ' ';
	std::cout << '\n';
};

int main()
{
	using V = int;
	using RV = std::reference_wrapper<V>;

	std::list<V> l(10);
	std::iota(l.begin(), l.end(), -4);

	// can't use shuffle on a list (requires random access), but can use it on a vector
	std::vector<RV> v(l.begin(), l.end());
	std::shuffle(v.begin(), v.end(), std::mt19937{ std::random_device{}() });

	print("L: ", l);
	print("V: ", v);
	std::for_each(l.begin(),l.end(), [](V& i) { i *= 2; });
	print("V*2: ", v);
	print("L#: ", l);
	std::optional<RV> x (*l.begin());
	if (x)
		std::cout << *x << '\n';
	system("pause");
}