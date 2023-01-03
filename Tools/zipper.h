#pragma once
#include <iterator>
#include <tuple>
#include <vector>


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
	template <typename... T>
	struct zip
	{
		struct iterator;

		zip(T&&... ts) : begin_{ std::make_tuple(std::begin(ts)...) }, end_{ std::make_tuple(std::end(ts)...) } {}

		bool empty() const { return begin_ == end_; }
		const iterator begin() const { return begin_; }
		const iterator end() const { return end_; }
		iterator begin() { return begin_; }
		iterator end() { return end_; }

	private:
		iterator begin_, end_;

		struct iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::tuple<decltype(*std::begin(std::declval<T>()))...>;
			using pointer = value_type*;
			using reference = value_type&;

		private:
			std::tuple<decltype(std::begin(std::declval<T>()))...> iterator_pack;
		
		public:
			iterator() = default;
			explicit iterator(decltype(iterator_pack) iters) : iterator_pack{ std::move(iters) } {}

			iterator& operator++()
			{
				increment(std::index_sequence_for<T...>{});
				return *this;
			}

			iterator operator++(int)
			{
				auto t{ *this };
				increment(std::index_sequence_for<T...>{});
				return t;
			}

			bool operator==(const iterator& other) const
			{
				return iterator_pack == other.iterator_pack; 
			}
			bool operator!=(const iterator& other) const
			{
				return iterator_pack != other.iterator_pack;
			}

			auto operator*() const
			{ 
				return deref(std::index_sequence_for<T...>{}); 
			}

		private:
			template <std::size_t... I>
			auto deref(std::index_sequence<I...>) const
			{
				return value_type{ *std::get<I>(iterator_pack)... };
			}

			template <std::size_t... I>
			void increment(std::index_sequence<I...>)
			{
				auto l = { (++std::get<I>(iterator_pack), 0)... };	l;
			}
		};
	};

	/// @brief Short key with CTAD
	template<typename ...T>
	zip(T&&...)->zip<T...>;

}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	if (!vec.empty())
	{
		std::cout << vec.front();
		for (auto&& x : jh::drop(vec, 1)) std::cout << "," << x;
	}
	return os;
}
//
//namespace std
//{
//	template<typename ...Ts>
//	auto distance(typename jh::zip<Ts...>::iterator b, typename jh::zip<Ts...>::iterator e)
//	{
//		return distance(std::get<0>(b), std::get<0>(e));
//	}
//}
