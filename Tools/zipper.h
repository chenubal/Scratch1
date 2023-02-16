#pragma once
#include <iterator>
#include <tuple>
#include <vector>
#include <numeric>
#include <ostream>

namespace jh
{
	template<typename Iterator>
	struct ignore_nth
	{
		ignore_nth(Iterator start, Iterator end, size_t nth) : start_(start, nth, 0), end_(end, nth, (nth > 1 ? std::distance(start, end) : 0)) {}
		auto begin() { return start_; }
		auto end() { return end_; }
		const auto begin() const { return start_; }
		const auto end() const { return end_; }

	private:
		struct IteratorAdapter
		{
			IteratorAdapter() = default;
			IteratorAdapter(Iterator it, size_t nth, size_t start) : iter(it), nth(nth), counter(start) {}
			auto& operator++() { inc();	return *this; }
			auto operator++(int) { auto t{ *this }; inc(); return t; }
			bool operator!=(const IteratorAdapter& other) const { return  counter < other.counter; }
			const auto& operator*() const { return *iter; }
			auto& operator*() { return *iter; }
		protected:
			void inc_1() { counter++; iter++; }
			void inc() { if ((counter % nth) == (nth - 2)) inc_1();  inc_1(); }
			Iterator iter;
			size_t nth;
			size_t counter = 0;
		};

		IteratorAdapter start_, end_;
	};

	template<class Container>
	auto make_ignore(Container& container, size_t nth)
	{
		return ignore_nth(std::begin(container), std::end(container), nth);
	}

	template<class Container>
	auto make_ignore(Container const& container, size_t nth)
	{
		return ignore_nth(std::begin(container), std::end(container), nth);
	}

	template<typename Iterator>
	struct pick_nth
	{
		pick_nth(Iterator begin, Iterator end, size_t n) : begin_(begin, n, 0), end_(end, n, std::distance(begin, end)) {}
		auto begin() { return begin_; }
		auto end() { return end_; }
		const auto begin() const { return begin_; }
		const auto end() const { return end_; }

	private:
		struct iteratorA
		{
			iteratorA(Iterator it, size_t nth, size_t start) : iter(it), nth(nth), counter(start) {}
			auto& operator++() { inc();	return *this; }
			auto operator++(int) { auto t{ *this };	inc(); return t; }
			bool operator!=(const iteratorA& other) const { return  counter < other.counter; }
			const auto& operator*() const { return *iter; }
			auto& operator*() { return *iter; }
		protected:
			void inc() { iter = std::next(iter, nth); counter += nth; }
			Iterator iter;
			size_t nth;
			size_t counter = 0;
		};

		iteratorA begin_, end_;
	};

	template<class Container>
	auto make_slice(Container& container, size_t nth)
	{
		return pick_nth(std::begin(container), std::end(container), nth);
	}

	template<class Container>
	auto make_slice(Container const& container, size_t nth)
	{
		return pick_nth(std::begin(container), std::end(container), nth);
	}

	/// Provides an span adapter on iterators.
	template<typename Iterator>
	struct span
	{
		span(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
		span(Iterator begin, size_t count) : span(begin, std::next(begin, count)) {}
		Iterator begin() { return begin_; }
		Iterator end() { return end_; }
		const Iterator begin() const { return begin_; }
		const Iterator end() const { return end_; }
	private:
		Iterator begin_, end_;
	};

	/// Provides an drop adapter on iterators. Drops n elements on begin
	template<class Iterator>
	auto take(Iterator begin, Iterator end, size_t count = 1u)
	{
		count = std::min<size_t>(count, std::distance(begin, end));
		return span(begin, std::next(begin, count));
	}

	/// Provides an drop adapter on const container. Drops n elements on begin
	template<class Container>
	auto take(Container const& container, size_t count = 1u)
	{
		return take(std::begin(container), std::end(container), count);
	}

	/// Provides an drop adapter on container. Drops n elements on begin
	template<class Container>
	auto take(Container& container, size_t count = 1u)
	{
		return take(std::begin(container), std::end(container), count);
	}

	/// Provides an drop adapter on iterators. Drops n elements on begin
	template<class Iterator>
	auto drop(Iterator begin, Iterator end, size_t count = 1u)
	{
		count = std::min<size_t>(count, std::distance(begin, end));
		return span(std::next(begin, count), end);
	}

	/// Provides an drop adapter on const container. Drops n elements on begin
	template<class Container>
	auto drop(Container const& container, size_t count = 1u)
	{
		return drop(std::begin(container), std::end(container), count);
	}

	/// Provides an drop adapter on container. Drops n elements on begin
	template<class Container>
	auto drop(Container& container, size_t count = 1u)
	{
		return drop(std::begin(container), std::end(container), count);
	}


	/// @brief Combines several equal sized containers. Provides STL forward iterators (as tuple).
/// @brief Combines several equal sized containers. Provides STL forward iterators (as tuple).
	template <typename... T>
	struct zip
	{
		class iterator : std::iterator<std::forward_iterator_tag, std::tuple<decltype(*std::declval<T>().begin())...>>
		{
		private:
			std::tuple<decltype(std::declval<T>().begin())...> iterator_pack;

			template <std::size_t... I>
			auto deref(std::index_sequence<I...>) const
			{
				return typename iterator::value_type{ *std::get<I>(iterator_pack)... };
			}

			template <std::size_t... I>
			void increment(std::index_sequence<I...>)
			{
				auto l = { (++std::get<I>(iterator_pack), 0)... };
				l;
			}

		public:
			explicit iterator(decltype(iterator_pack) iters) : iterator_pack{ std::move(iters) } {}

			iterator& operator++()
			{
				increment(std::index_sequence_for<T...>{});
				return *this;
			}

			iterator operator++(int)
			{
				auto saved{ *this };
				increment(std::index_sequence_for<T...>{});
				return saved;
			}

			bool operator!=(const iterator& other) const
			{
				return iterator_pack != other.iterator_pack;
			}

			auto operator*() const { return deref(std::index_sequence_for<T...>{}); }
		};

		zip(T&&... seqs) : begin_{ std::make_tuple(seqs.begin()...) }, end_{ std::make_tuple(seqs.end()...) } {}

		const iterator begin() const { return begin_; }
		const iterator end() const { return end_; }
		iterator begin() { return begin_; }
		iterator end() { return end_; }

	private:
		iterator begin_;
		iterator end_;
	};

	/// @brief Short key with CTAD
	template<typename ...T>
	zip(T&&...)->zip<T...>;


	template<class Function, class Index = unsigned long>
	struct Loop
	{
		using result_t = std::invoke_result_t<Function, Index>;
		struct iterator
		{
			using value_type = result_t;
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			iterator(Function f, Index start = 0) : m_counter(start), m_function(f) {}
			result_t operator*() const { return m_function(m_counter); }
			iterator& operator++() { ++m_counter; return *this; }
			iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
			bool operator!=(iterator const& other) const { return other.m_counter != m_counter; }
			bool operator==(iterator const& other) const { return other.m_counter == m_counter; }
		protected:
			Index m_counter;
			Function m_function; // R=f(I)
		};

		Loop(Function f, Index endIndex) : begin_(iterator(f, 0)), end_(iterator(f, endIndex)) {}
		Loop(Index endIndex) : Loop([](auto i) { return i; }, endIndex) {}
		auto begin() { return begin_; }
		auto end() { return end_; }
		auto const begin() const { return begin_; }
		auto const end() const { return end_; }
		bool empty() const { return begin() == end(); }
		iterator begin_, end_;
	};

	// CTAD
	template <typename Index>
	Loop(Index)->Loop<std::function<Index(Index)>, Index>;


	
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
	{
		auto end_ = std::end(v);
		if (auto s = std::begin(v); s != end_)
		{
			os << *s;
			for (auto&& v : drop(s, end_)) os << '\n' << v;
		}
		return os;
	}

	template<class C, class R, class F> // Needs F with R=F(R,C::V,)
	R sum_f(C const c, F f) { return std::accumulate(std::begin(c), std::end(c), R(0), f); }

}
