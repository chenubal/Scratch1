#include <algorithm>
#include <list>
#include <vector>
#include <iostream>
#include <numeric>
#include <random>
#include <functional>
#include <algorithm>
#include <optional>
#include <xutility>

auto print = [](auto const rem,  auto const& v) 
{
	std::cout << rem;
	for (auto const& e : v)
		std::cout << e << ' ';
	std::cout << '\n';
};

struct X
{
	X() { std::cout << "X default\n"; }
	X(X const &o) { c = o.c; std::cout << "X copy\n"; }
	X(X&& o) { c = std::move(o.c); std::cout << "X move\n"; }
	std::vector<int> c;
};

std::unique_ptr<X> mu1(X&& s)
{
	return std::make_unique<X>(s);
}

std::unique_ptr<X> mu2(X&& s)
{
	return std::make_unique<X>(std::forward<X>(s));
}

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

	X xx; xx.c = { 4,7,9 };
	auto yy = std::make_unique<X>(std::move(xx));
	auto vv = mu1(std::move(*yy));
	auto uu = mu2(std::move(*vv));
	std::cout << uu->c[1] << ' ' << uu->c[0] << '\n';
	system("pause");
}