#include <iterator>
#include <tuple>

template <typename... T>
class zipper 
{
public:
	class iterator 
		: std::iterator<std::forward_iterator_tag,
		                std::tuple<decltype(*std::declval<T>().begin())...>> 
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
		}

	public:
		explicit iterator(decltype(iterator_pack) iters) 
			: iterator_pack{ std::move(iters) } {}

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

	zipper(T&... seqs) 
		: begin_{ std::make_tuple(seqs.begin()...) }
		, end_{ std::make_tuple(seqs.end()...) } {}

	iterator begin() const { return begin_; }
	iterator end() const { return end_; }

private:
	iterator begin_;
	iterator end_;
};

// User CTAD
template<typename ...T> 
zipper(T&&...)->zipper<T...>;

#include <list>
#include <vector>
#include <iostream>


int main() 
{
	std::vector<int> one{ {1,2} };
	auto two = [] { return std::vector<short>{ {2, 22}}; };
	const std::list<float> three{ {3, 33} };
	std::vector<int> four{ {4, 44} };

	// Change container 1,4
	for (auto a : zipper(one,four))
	{
		std::get<0>(a) *= 7;
		std::get<1>(a) += 10;
	}

	// Read all container
	for (auto a : zipper(one, two(), three, four))
	{
		std::cout << std::get<0>(a) << " " << std::get<short&>(a) <<" " << std::get<float const&>(a)<< " " <<std::get<3>(a) << "\n";
	}
}