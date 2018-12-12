#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <iterator>

template<class T>
struct SuperIt
{
	SuperIt(T current = 0, T inc = 1) : current(current), inc(inc) {}
	T current, inc;
	SuperIt& operator++() { current += inc; return *this; }
	operator T() { return current; }
};

template<class T>
SuperIt<T> makeSuperIt(T current, T inc) { return SuperIt<T>(current, inc); }

template<class LIter, class RIter>
struct pair_iter
{
	using L = typename std::iterator_traits<LIter>::value_type;
	using R = typename std::iterator_traits<RIter>::value_type;

	pair_iter(LIter l, RIter r) : l0(l), r0(r){}

	std::pair<L,R> operator*() const { return std::make_pair(*l0,*r0); }
	pair_iter<LIter, RIter>& operator++() { ++l0; ++r0; return *this; }      // prefix ++
	pair_iter<LIter, RIter> operator++(int) { auto t = *this; return ++t; }  // postfix ++
	bool operator==(pair_iter<LIter, RIter> const& o) const { return o.l0 == l0; }
	bool operator!=(pair_iter<LIter, RIter> const& o) const { return  !(o==*this); }
	LIter l0;
	RIter r0;
};

template<class LIter, class RIter>
struct zipper_t
{
	zipper_t(LIter s1, LIter e1, RIter s2) : s1(s1), e1(e1),s2(s2){}
	pair_iter<LIter, RIter> begin() { return pair_iter<LIter, RIter>(s1, s2); }
	pair_iter<LIter, RIter> end() { return pair_iter<LIter, RIter>(e1, s2); }
	LIter s1, e1;
	RIter s2;
};

template<class LIter, class RIter>
zipper_t<LIter, RIter> make_zipper(LIter s1, LIter e1, RIter s2) { return zipper_t<LIter, RIter>(s1, e1, s2); }

int main()
{
	std::vector<int> Ints(20);
	std::iota(Ints.begin(), Ints.end(), makeSuperIt(100, 3));
	for (auto v : Ints) std::cout << v << " ";
	std::cout  << "\n\n";

	std::vector<double> Doubles(20);
	std::iota(Doubles.begin(), Doubles.end(), makeSuperIt(10.0, 3.3));
	for (auto v : Doubles) std::cout << v << " ";
	std::cout << "\n\n";

	for (auto v : make_zipper(Ints.begin(),Ints.end(),Doubles.begin())) 
		std::cout << v.first << "-" << v.second << "\n";
	std::cout << "\n";

}
