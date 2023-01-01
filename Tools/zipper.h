#pragma once
#include <iterator>
#include <tuple>
#include <vector>


namespace jh
{
	template<typename It>
	struct skip
	{
		struct Iterator
		{
			Iterator(It it, size_t skip, size_t start) : it(it), n(skip), k(start) {}
			auto& operator++() { inc();	return *this; }
			auto operator++(int) { auto t{ *this };	inc(); return t; }
			bool operator!=(const Iterator& other) const { return  k < other.k; }
			const auto& operator*() const { return *it; }
			auto& operator*() { return *it; }
		protected:
			void inc_1() { k++; it++; }
			void inc() { if ((k % n) == 1) inc_1();  inc_1();}
			It it;
			const size_t n;
			size_t k = 0;
		};

		skip(It start, It end, size_t n) : b(start, n, 0), e(end, n, std::distance(start, end)) {}
		auto begin() { return b; }
		auto end() { return e; }
		const auto begin() const { return b; }
		const auto end() const { return e; }
	private:
		Iterator b, e;
	};


	template<typename It>
	struct slice
	{
		struct Iterator
		{
			Iterator(It it, size_t skip, size_t start) : it(it), n(skip), k(start) {}
			auto& operator++() { inc();	return *this; }
			auto operator++(int) { auto t{ *this };	inc(); return t; }
			bool operator!=(const Iterator& other) const {return  k < other.k;}
			const auto& operator*() const { return *it; }
			auto& operator*() { return *it; }
		protected:
			void inc() { it = std::next(it, n); k += n; }
			It it;
			size_t n;
			size_t k = 0;
		};
		
		slice(It start, It end, size_t n) : b(start, n, 0), e(end, n, std::distance(start, end)) {}
		auto begin() { return b; }
		auto end() { return e; }
		const auto begin() const { return b;  }
		const auto end() const { return e;}
	private:
		Iterator b, e;
	};

	template<class Iterable>
	auto make_slice(Iterable& c, size_t n )
	{
		return slice(c.begin(), c.end(), n);
	}

	template<class Iterable>
	auto make_slice(Iterable const& c, size_t n)
	{
		return slice(c.begin(), c.end(), n);
	}

	template<typename It>
	struct span
	{
		span(It start, It end) : b(start), e(end) {}
		span(It start, size_t n) : span(start, std::next(start, n)) {}
		It begin() { return b; }
		It end() { return e; }
		const It begin() const { return b; }
		const It end() const { return e; }
	private:
		It b, e;
	};

	template<class It>
	auto drop(It b, It e, size_t n = 1u)
	{
		n = std::min<size_t>(n, std::distance(b,e));
		return span(std::next(b, n),e);
	}

	template<class Iterable>
	auto drop(Iterable const& c, size_t n=1u)
	{
		return drop(c.begin(), c.end(),n);
	}

	template<class Iterable>
	auto drop(Iterable& c, size_t n = 1u)
	{
		return drop(c.begin(), c.end(), n);
	}


	/// @brief Combines several equal sized containers. Provides STL forward iterators (as tuple).
	template <typename... T>
	struct zip
	{
		struct iterator 
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::tuple<decltype(*std::declval<T>().begin())...>;
			using pointer = value_type*;
			using reference = value_type&;

		private:
			std::tuple<decltype(std::declval<T>().begin())...> iterator_pack;

			template <std::size_t... I>
			auto deref(std::index_sequence<I...>) const
			{
				return value_type{ *std::get<I>(iterator_pack)... };
			}

			template <std::size_t... I>
			void increment(std::index_sequence<I...>)
			{
				auto l = { (++std::get<I>(iterator_pack), 0)... };
				l;
			}

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

}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec)	os << el << ' ';
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
