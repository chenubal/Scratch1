#include <iostream>
#include <vector>
#include <algorithm>

namespace JH
{
	template<class S, bool is_const = true>
	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::conditional_t<is_const, S const, S>;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(S* d, size_t n) : d(d), n(n) {}
		Iterator(S const* d, size_t n) : d(d), n(n) {}

		reference operator*() { return d[n]; }
		pointer operator->() { return d + n; }
		auto& operator++() { n++; return *this; }
		auto operator++(int) { auto t = *this; ++(*this); return t; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.n == b.n; }
		friend bool operator!= (const Iterator& a, const Iterator& b) { return !(a == b); }
		size_t n;
		pointer d;
	};


	template<class T>
	struct Host
	{
		using value_type = T;

		explicit Host(size_t n = 0) : d(n) {}
		Host(Host const&) = default;

		auto begin() { return Iterator<value_type,false>(d.data(), 0); }
		auto end() { return Iterator<value_type,false>(d.data(), d.size()); }
		auto begin() const { return Iterator<value_type,true>(d.data(), 0); }
		auto end() const { return Iterator<value_type,true>(d.data(), d.size()); }

		auto size() const { return d.size(); }
		auto& at(size_t i) { return d.at(i); }
		auto& operator[](size_t i) { return d[i]; }

	protected:
		std::vector<value_type> d;
	};

	struct TestClient
	{
		bool operator==(TestClient const& bt) { return a == bt.a; }
		bool operator==(int x) { return a == x; }
		int a = 0; double b = 0;
	};

	using TestClass = Host<TestClient>;
}

using namespace JH;

int main()
{
	auto print = [](std::string const& prefix, auto const& container, std::ostream& os=std::cout) 
	{
		os << prefix << " = "; 
		for (auto const& e : container) os << e.a << " ";  
		os << "\n"; 
	};

	TestClass tc(5);

	using V = std::decay_t<decltype(tc.at(0))>; // :-)

	// tc = 10,11,....
	std::generate(tc.begin(), tc.end(), [k = 10]() mutable {return  V{ k++, 0.0 };
});
	print("tc", tc);

	// tcc(tc)
	const JH::TestClass tcc(tc);
	print("tcc", tcc);

	// tc += 20
	std::for_each(tc.begin(), tc.end(), [](auto& e) { e.a += 20; });
	print("tc+=20", tc);

	// tc = CC
	std::transform(tcc.begin(), tcc.end(), tc.begin(), [](auto& e) { return e; });
	print("tc=tcc", tc);

	// Use it->
	if (auto p = std::find(tc.begin(), tc.end(), 12); p != tc.end())
		p->a = 77;
	print("tc(12)=77", tc);

	// Use *it
	if (auto p = std::find_if(tc.begin(), tc.end(), [](auto const& e) { return e.a==13;}); p != tc.end())
		(*p).a = 66;
	print("tc(13)=66", tc);

}
