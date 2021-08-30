#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

template<class T>
struct MyContainer
{
	MyContainer(size_t n = 0) : d(n) {}
	MyContainer(MyContainer const&) = default;

	template<bool is_const = true>
	struct iterator 
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::conditional_t<is_const, T const, T>;
		using pointer = value_type*;
		using reference = value_type&;

		iterator(T* d, size_t n) : d(d), n(n) {}
		iterator(T const* d, size_t n) : d(d), n(n) {}

		reference operator*() { return d[n]; }
		pointer operator->() { return d+n; }
		auto& operator++() { n++; return *this; }
		auto operator++(int) { auto t = *this; ++(*this); return t; }
		friend bool operator== (const iterator& a, const iterator& b) { return a.n == b.n; }
		friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); }
		size_t n;
		pointer d;
	};

	auto begin() { return iterator<false>(d.data(), 0); }
	auto end() { return iterator<false>(d.data(), d.size()); }
	auto begin() const { return iterator<true>(d.data(), 0); }
	auto end() const { return iterator<true>(d.data(), d.size()); }

protected:
	std::vector<T> d;
};


int main()
{
	auto printJH = [](const char* s, auto const& C) {std::cout << s << " = "; for (auto const& x : C) { std::cout << x.a << " "; } std::cout << "\n"; };
	int i = 10;
	struct base_type
	{
		bool operator==(base_type const& bt) { return a == bt.a; }
		bool operator==(int x) { return a == x; }
		int a = 0; double b = 0;
	};

	MyContainer<base_type> C(5);

	for (auto& x : C) { x.a = i++; }
	printJH("C", C);

	// CC(C)
	const MyContainer< base_type> CC(C);
	printJH("CC", CC);

	// C += 20
	std::for_each(C.begin(), C.end(), [](auto& x) { x.a += 20; });
	printJH("C+=20", C);

	// C = CC
	std::transform(CC.begin(), CC.end(), C.begin(), [](auto& x) { return x; });
	printJH("C=CC", C);

	if (auto p = std::find(C.begin(), C.end(), 12); p != C.end())
		p->a = 77;
	printJH("C12-77", C);

	if (auto p = std::find_if(C.begin(), C.end(), [](auto x) { return x.a==13;}); p != C.end())
		(*p).a = 66;
	printJH("C13=66", C);

}
