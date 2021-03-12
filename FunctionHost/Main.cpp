#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace jh
{
	template<class F, class I = unsigned long> // for function object with R=F(I)
	struct Loop
	{
		using R = std::invoke_result_t<F, I>;
		struct iterator
		{
			iterator(F fun, I start = 0) : m_counter(start), m_function(fun) {}
			R operator*() const { return m_function(m_counter); }
			iterator& operator++() { ++m_counter; return *this; }
			iterator operator++(int) { auto t = *this; ++(*this); return t; }
			bool operator!=(iterator const& o) const { return o.m_counter != m_counter; }
		protected:
			I m_counter;
			F m_function; // R=f(I)
		};

		Loop(F f, I n) : m_begin(iterator(f, 0)), m_end(iterator(f, n)) {}
		Loop(I n) : Loop([](auto i) { return i; }, n) {}
		auto begin() { return m_begin; }
		auto end() { return m_end; }
		iterator m_begin, m_end;
	};

	// CTAD
	template <typename I>
	Loop(I)->Loop<std::function<I(I)>, I>;
}

void testHeader() { static int k = 1;  std::cout << "----------- Test " << (k++) << " --------------\n"; }

template<class F>
void runTest(F f) { testHeader(); f();  std::cout << "\n"; }

int main()
{
	using namespace jh;
	runTest([]{
		auto f = [](unsigned i)->int { return i * i; };
		for (auto&& x : Loop(f, 7))
			std::cout << x << " ";
	});
	runTest([] {
		for (auto&& x : Loop([](auto i){ return i + 5; }, 7))
			std::cout << x << " ";
	});
	runTest([] {
		auto f = [](unsigned i)->int { return i * 3+4; };
		for (auto&& x : Loop(f, 7))
			std::cout << x << " ";
	});
	runTest([] {
		struct X
		{
			int get(unsigned i) const { return data[i]; }
			auto size() const { return unsigned(data.size()); }
		private:
			std::vector<int> data = { 4,5,6,77,8 };
		} b;
		for (auto&& x : Loop([&b](unsigned i) { return b.get(i); }, b.size()))
			std::cout << x << " ";
	});
	runTest([] {
		auto b = Loop([](unsigned i) { return i + 77; }, 5).begin();
		std::cout << *(++b) << " ";
		std::cout << *b++ << " ";
		std::cout << *b++ << " ";
		std::cout << *b++ << " ";
	});
	runTest([] {
		for (auto&& x : Loop(12))
			std::cout << x << " ";
	});
}