#include <utility>
#include <xutility>

namespace jh
{
	using index_t = unsigned long;

	template<class Iterable>
	auto iterator_n(Iterable &c, index_t n) { auto iter = c.begin(); std::advance(iter, n); return iter; }

	template<class Iterable, bool is_const>
	struct crop
	{
		using iterator = std::conditional_t<is_const, typename Iterable::const_iterator, typename Iterable::iterator>;
		using const_iterator = const iterator;
		using value_type = typename Iterable::value_type;

		template<class S>
		crop(S & c, index_t first, index_t last)
		{
			m_begin = iterator_n(c, std::min(first, index_t(c.size())));
			m_end = iterator_n(c, std::min(last + 1, index_t(c.size())));
		}
		template<class S>
		crop(S&& c, index_t first, index_t last)
		{
			m_begin = iterator_n(c, std::min(first, index_t(c.size())));
			m_end = iterator_n(c, std::min(last + 1, index_t(c.size())));
		}
		template<class S>
		crop(S & c, index_t first = 0) : crop(c, first, index_t(c.size()) - 1) {}
		template<class S>
		crop(S&& c, index_t first = 0) : crop(c, first, index_t(c.size()) - 1) {}
		crop() = default;

		auto begin() { return m_begin; }
		auto end() { return m_end; }
		auto begin() const { return m_begin; }
		auto end()   const { return m_end; }
		auto size() const { return std::distance(m_begin, m_end); }
	private:
		iterator m_begin, m_end;
	};

	// User CTAD
	template <typename S>
	crop(S&, index_t, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	crop(S&, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	crop(S&)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	crop(S&&, index_t, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	crop(S&&, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	crop(S&&)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;

	template<class Iterable, bool is_const>
	struct slice
	{
		using value_type = typename Iterable::value_type;
		struct iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = slice::value_type;
			using pointer = std::conditional_t<is_const, typename Iterable::const_iterator, typename Iterable::iterator>;
			using reference = value_type & ;

			iterator() = default;
			iterator(pointer iter, index_t step, int count=-1) : m_iter(iter), m_step(step),m_count(count) {}
			auto& operator*() const { return *m_iter; }
			auto& operator++() {m_count -= m_step; std::advance(m_iter, m_step); return *this; }
			auto operator++(int) { return std::exchange(*this, ++(*this)); }
			friend bool operator== (const iterator& a, const iterator& b) { return a.m_iter == b.m_iter || (a.m_count*b.m_count) > 0; }
			friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); };

		private:
			index_t m_step = 1;
			int m_count = -1; // negative indicates 'end'
			pointer m_iter;
		};
		using const_iterator = const iterator;

		template<class S>
		slice(S& c, index_t step) : m_step(step)
		{
			m_begin = { c.begin(), m_step , int(c.size())};
			m_end = { c.end(), m_step };
		}
		template<class S>
		slice(S&& c, index_t step) : m_step(step)
		{
			m_begin = { c.begin(), m_step , int(c.size()) };
			m_end = { c.end(), m_step };
		}
		slice() = default;

		auto begin() { return m_begin; }
		auto end() { return m_end; }
		auto begin() const { return m_begin; }
		auto end()   const { return m_end; }
		auto size()  const { return std::distance(m_begin, m_end); }
	private:
		index_t m_step = 1;
		iterator m_begin, m_end;
	};

	// User CTAD
	template <typename S>
	slice(S&, index_t)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	slice(S&)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	slice(S&&, index_t)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S>
	slice(S&&)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
}
#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
	using namespace jh;
	using V = std::vector<double>;
	{
		std::cout << "\nTest: slice/crop on const vector\n";
		const V  v{ 1,2,3,4,5,6,7,8,9,0 };
		for (auto && x : slice(crop(v, 2), 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(v, 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : crop(v, 3, 6)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : crop(slice(v, 2), 2)) std::cout << x << " ";
	}
	{
		std::cout << "\nTest: slice/crop on non-const vector\n";
		V  w{ 1,2,3,4,5,6,7,8,9,0 };
		for (auto && x : slice(crop(w, 2), 2)) x = 99;
		for (auto const& x : w) std::cout << x << " ";
		std::cout << "\n---------\n";
		auto sc = slice(crop(w, 3), 2);
		std::for_each(sc.begin(), sc.end(), [](auto &x) {x = 77; });
		for (auto const& x : w) std::cout << x << " ";
	}
	{
		std::cout << "\nTest: slice\n";
		V  u{ 0,1,2,3,4,5,6,7,8,9 };
		for (auto && x : slice(u, 1)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 3)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 4)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 5)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 6)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 7)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 8)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 9)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(u, 10)) std::cout << x << " ";
	}
}
