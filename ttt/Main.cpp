#include <iostream>     // std::cout
#include <iterator>     // std::iterator, std::input_iterator_tag
#include <vector>
#include <algorithm>

template <typename... T>
struct combine
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
		const auto operator*() const { return deref(std::index_sequence_for<T...>{}); }
		auto operator*() { return deref(std::index_sequence_for<T...>{}); }
	};

	combine(T&... seqs) : begin_{ std::make_tuple(seqs.begin()...) }, end_{ std::make_tuple(seqs.end()...) } {}

	const iterator begin() const { return begin_; }
	const iterator end() const { return end_; }
	iterator begin() { return begin_; }
	iterator end() { return end_; }

private:
	iterator begin_;
	iterator end_;
};

// User CTAD
template<typename ...T>
combine(T&&...)->combine<T...>;


template<bool is_const, class T>
struct IteratorT : public std::iterator<std::forward_iterator_tag, T>
{
	using TRef = typename std::conditional<is_const, const T&, T&>::type;
	IteratorT(T* x) :p(x) {}
	IteratorT(const IteratorT<false, T>& other) : IteratorT(other.p) {}
	IteratorT& operator++() { ++p; return *this; }
	IteratorT operator++(int) { IteratorT tmp(*this); operator++(); return tmp; }
	bool operator==(const IteratorT& rhs) const { return p == rhs.p; }
	bool operator!=(const IteratorT& rhs) const { return p != rhs.p; }
	TRef operator*() const { return *p; }
	TRef operator*() { return *p; }
	T* p;
};

struct P
{
	P(int* d, int n) : data(d), n(n) {}

	auto begin() const { return IteratorT<true, int>(data); }
	auto end() const { return IteratorT<true, int>(data + n); }
	auto begin() { return IteratorT<false, int>(data); }
	auto end() { return IteratorT<false, int>(data + n); }
	int *data;
	unsigned n;
};

int main()
{
	int numbers[] = { 10,20,30,40,50,60 };
	P p(numbers, 6);
	const P p1(numbers, 6);

	std::vector<int> vec = { 1,2,3,4,5,6 };
	for (auto&&[a, b, c] : combine(vec, p1, p))
		a = b + 4*c;

	for (auto const& x : vec)  std::cout << x << ' ';
	std::cout << '\n';

	return 0;
}