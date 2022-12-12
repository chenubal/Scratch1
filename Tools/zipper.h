#pragma once
#include <iterator>
#include <tuple>

namespace jh
{
	/// @brief Combines several equal sized containers. Provides STL forward iterators (as tuple).
	template <typename... T>
	struct zip
	{
		struct iterator 
		{
			using vt = std::tuple<decltype(*std::declval<T>().begin())...>;
			using iterator_category = std::forward_iterator_tag;
			using value_type = vt;
			using difference_type = int;
			using pointer = vt*;
			using reference = vt&;

		private:
			std::tuple<decltype(std::declval<T>().begin())...> iterator_pack;

			template <std::size_t... I>
			auto deref(std::index_sequence<I...>) const
			{
				return vt{ *std::get<I>(iterator_pack)... };
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

namespace std
{

}