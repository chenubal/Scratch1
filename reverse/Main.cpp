#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

using Ints = std::vector<int>;

template<class ReverseIterable>
struct reverseAdapter
{
	reverseAdapter(ReverseIterable& r) : ref(r) {}

	auto begin() { return std::rbegin(ref); }
	auto end() { return std::rend(ref); }
	auto begin() const { return std::rbegin(ref); }
	auto end() const { return std::rend(ref); }
	ReverseIterable& ref;
};

template<class C>
void print(C const& container)
{
	for (auto const& v : container)
		std::cout << v << ", ";
	std::cout << std::endl;
}

template<class C>
void print_r(C const& container)
{
	print(reverseAdapter(container));
}

int main()
{
	Ints values = {1,2,3,4,5 };

	for (auto && v : values) v += 10;
	print(values);

	for (auto && v : reverseAdapter(values)) v += 10;
	print_r(values);

	double a[] = { -5.1, 10.2, 15.3 };
	print_r(a);

}
