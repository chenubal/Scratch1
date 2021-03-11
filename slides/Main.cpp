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
	template <typename S, typename ...Rest>
	crop(S&, Rest...)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S, typename ...Rest>
	crop(S&&, Rest...)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;

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
			friend bool operator== (const iterator& a, const iterator& b) { return a.m_iter == b.m_iter || (a.isEnd() && b.isEnd()); }
			friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); };

		private:
			bool isEnd() const { return m_count < 0; }
			index_t m_step = 1;
			int m_count = -1; // negative indicates 'end'
			pointer m_iter;
		};
		using const_iterator = const iterator;

		template<class S>
		slice(S& c, index_t step) : m_step(step), m_begin(c.begin(), m_step, int(c.size())), m_end(c.end(), m_step) {}
		template<class S>
		slice(S&& c, index_t step) : m_step(step), m_begin(c.begin(), m_step, int(c.size())), m_end(c.end(), m_step) {}

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
	template <typename S, typename... Rest>
	slice(S&, Rest...)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
	template <typename S, typename... Rest>
	slice(S&&, Rest...)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
}

//------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

int main()
{
	using namespace jh;
	using V = std::vector<double>;
	{
		std::cout << "\nTest: slice\n";
		const V  numbers{ 0,1,2,3,4,5,6,7,8,9 };
		for (auto && x : slice(numbers, 1)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 3)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 4)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 5)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 6)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 7)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 8)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 9)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 10)) std::cout << x << " ";
		std::cout << "\n---------\n";
		std::cout << "size=" << slice(numbers, 2).size() << "\n";
		std::cout << "size=" << slice(numbers, 3).size() << "\n";
	}
	{
		std::cout << "\nTest: slice/crop on const vector\n";
		const V  numbers{ 0,1,2,3,4,5,6,7,8,9 };
		for (auto && x : slice(crop(numbers, 2), 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : slice(numbers, 2)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : crop(numbers, 3, 6)) std::cout << x << " ";
		std::cout << "\n---------\n";
		for (auto && x : crop(slice(numbers, 2), 2)) std::cout << x << " ";
	}
	{
		std::cout << "\n\nTest: slice/crop on non-const vector";
		V  numbers{ 0, 1,2,3,4,5,6,7,8,9 };
		std::cout << "\nslice(crop(numbers, 2), 2) x=99: ";
		for (auto && x : slice(crop(numbers, 2), 2)) x = 99;
		for (auto const& x : numbers) std::cout << x << " ";
		std::cout << "\nslice(crop(numbers, 3), 2) x=77: ";
		auto sc = slice(crop(numbers, 2), 2);
		std::for_each(sc.begin(), sc.end(), [](auto &x) {x = 77; });
		for (auto const& x : numbers) std::cout << x << " ";
	}

	{
		std::cout << "\n\nTest: slice/crop on non-const map";
		std::map<int, double>  numbers{ {0,8},{1,7},{-4,4},{6,1},{8,0},{33,-5} };
		std::cout << "\nall: ";
		for (auto const&[a, b] : numbers) std::cout << a << "|" << b << " ";
		std::cout << "\ncrop(numbers, 2)); ";
		for (auto const&[a, b] : crop(numbers, 2)) std::cout << a << "|" <<b << " ";
		std::cout << "\nslice(numbers, 2)): ";
		for (auto const&[a, b] : slice(numbers, 2)) std::cout << a << "|" << b << " ";
		std::cout << "\ncrop(slice(numbers, 2), 1)): ";
		for (auto const&[a, b] : crop(slice(numbers, 2), 1)) std::cout << a << "|" << b << " ";
		std::cout << "\nslice(crop(numbers, 1), 2)): ";
		for (auto const&[a, b] : slice(crop(numbers, 1), 2)) std::cout << a << "|" << b << " ";
	}
}
